/**
 * Copyright © 2011 Kirill Gavrilov <kirill@sview.ru>
 *
 * StMoviePlayer program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StMoviePlayer program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "StSubtitleQueue.h"

#include <StGLWidgets/StSubQueue.h>

namespace {
    static const StString ST_CRLF_REDUNDANT   = "\x0D\x0A";
    static const StString ST_CRLF_REPLACEMENT = " \x0A";
};

/**
 * Thread function just call decodeLoop() function.
 */
static SV_THREAD_FUNCTION threadFunction(void* theSubtitleQueue) {
    StSubtitleQueue* aSubtitleQueue = (StSubtitleQueue* )theSubtitleQueue;
    aSubtitleQueue->decodeLoop();
    return SV_THREAD_RETURN 0;
}

StSubtitleQueue::StSubtitleQueue(const StHandle<StSubQueue>& theSubtitlesQueue)
: StAVPacketQueue(512),
  myOutQueue(theSubtitlesQueue),
  myThread(NULL),
  myASS(),
  evDowntime(true),
  toQuit(false) {
    //
    myThread = new StThread(threadFunction, (void* )this);
}

StSubtitleQueue::~StSubtitleQueue() {
    toQuit = true;
    pushQuit();

    myThread->wait();
    delete myThread;

    deinit();
}

bool StSubtitleQueue::init(AVFormatContext*   theFormatCtx,
                           const unsigned int theStreamId) {
    if(!StAVPacketQueue::init(theFormatCtx, theStreamId)
    || myCodecCtx->codec_type != AVMEDIA_TYPE_SUBTITLE) {
        signals.onError("FFmpeg: invalid stream");
        deinit();
        return false;
    }

    if(myCodecCtx->codec_id != CODEC_ID_TEXT) {
        // find the decoder for the subtitles stream
        myCodec = avcodec_find_decoder(myCodecCtx->codec_id);
        if(myCodec == NULL) {
            stError("FFmpeg: Subtitle decoder not found");
            deinit();
            return false;
        }

        // open SUBTITLE codec
    #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
        if(avcodec_open2(myCodecCtx, myCodec, NULL) < 0) {
    #else
        if(avcodec_open(myCodecCtx, myCodec) < 0) {
    #endif
            stError("FFmpeg: Could not open subtitle codec");
            deinit();
            return false;
        }

        // initialize ASS parser
    #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 95, 0))
        /// TODO (Kirill Gavrilov#6) when we should do that?
        myASS.init((char* )myCodecCtx->subtitle_header,
                   myCodecCtx->subtitle_header_size);
    #endif
    }
    return true;
}

void StSubtitleQueue::deinit() {
    StAVPacketQueue::deinit();
    myASS.init(NULL, 0);
}

void StSubtitleQueue::decodeLoop() {
    int isFrameFinished = 0;
    double aPts = 0.0;
    double aDuration = 0.0;
    AVSubtitle aSubtitle;

    for(;;) {
        if(isEmpty()) {
            evDowntime.set();
            StThread::sleep(10);
            continue;
        }
        evDowntime.reset();

        StHandle<StAVPacket> aPacket = pop();
        if(aPacket.isNull()) {
            continue;
        }
        switch(aPacket->getType()) {
            case StAVPacket::FLUSH_PACKET: {
                // got the special FLUSH packet - flush FFmpeg codec buffers
                if(myCodecCtx != NULL && myCodec != NULL) {
                    avcodec_flush_buffers(myCodecCtx);
                }
                myOutQueue->clear();
                continue;
            }
            case StAVPacket::START_PACKET: {
                myOutQueue->clear();
                continue;
            }
            case StAVPacket::END_PACKET: {
                if(toQuit) {
                    return;
                }
                continue;
            }
            case StAVPacket::QUIT_PACKET: {
                return;
            }
        }

        aPts      = unitsToSeconds(aPacket->getPts()) - myPtsStartBase;
        aDuration = unitsToSeconds(aPacket->getConvergenceDuration());
        if(myCodec != NULL) {
            // decode subtitle item
        #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 23, 0))
            avcodec_decode_subtitle2(myCodecCtx, &aSubtitle,
                                     &isFrameFinished, aPacket->getAVpkt());
        #else
            avcodec_decode_subtitle(myCodecCtx, &aSubtitle,
                                    &isFrameFinished,
                                    aPacket->getData(), aPacket->getSize());
        #endif

            if(isFrameFinished != 0 && aPacket->getPts() != stLibAV::NOPTS_VALUE) {
                for(unsigned aRectId = 0; aRectId < aSubtitle.num_rects; ++aRectId) {
                    AVSubtitleRect* aRect = aSubtitle.rects[aRectId];
                    if(aRect == NULL) {
                        // should not happens
                        continue;
                    }

                    switch(aRect->type) {
                        case SUBTITLE_BITMAP: {
                            // unsupported yet
                            break;
                        }
                        case SUBTITLE_TEXT: {
                            StString aText = aRect->text;
                            aText.replaceFast(ST_CRLF_REDUNDANT, ST_CRLF_REPLACEMENT); // remove redundant CR symbols
                            StHandle<StSubItem> aNewSubItem = new StSubItem(aText,
                                                                            aPts, aPts + aDuration);
                            myOutQueue->push(aNewSubItem);
                            break;
                        }
                        case SUBTITLE_ASS: {
                            StString aTextData = aRect->ass;
                            StHandle<StSubItem> aNewSubItem = myASS.parseEvent(aTextData, aPts);
                            if(!aNewSubItem.isNull()) {
                                myOutQueue->push(aNewSubItem);
                            }
                            break;
                        }
                        case SUBTITLE_NONE:
                        default:
                            break;
                    }
                }
            }
        #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 82, 0))
            avsubtitle_free(&aSubtitle);
        #else
            for(unsigned aRectId = 0; aRectId < aSubtitle.num_rects; ++aRectId) {
                av_freep(&aSubtitle.rects[aRectId]->pict.data[0]);
                av_freep(&aSubtitle.rects[aRectId]->pict.data[1]);
                av_freep(&aSubtitle.rects[aRectId]->pict.data[2]);
                av_freep(&aSubtitle.rects[aRectId]->pict.data[3]);
                av_freep(&aSubtitle.rects[aRectId]->text);
                av_freep(&aSubtitle.rects[aRectId]->ass);
                av_freep(&aSubtitle.rects[aRectId]);
            }
            av_freep(&aSubtitle.rects);
            stMemSet(&aSubtitle, 0, sizeof(AVSubtitle));
        #endif
        } else {
            // just plain text
            StString aText = (const char* )aPacket->getData();
            aText.replaceFast(ST_CRLF_REDUNDANT, ST_CRLF_REPLACEMENT); // remove redundant CR symbols
            StHandle<StSubItem> aNewSubItem = new StSubItem(aText,
                                                            aPts, aPts + aDuration);
            myOutQueue->push(aNewSubItem);
        }

        // and now packet finished
        aPacket.nullify();
    }
}