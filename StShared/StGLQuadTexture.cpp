/**
 * Copyright © 2009-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <StGLStereo/StGLQuadTexture.h>

#include <StGLCore/StGLCore11.h>
#include <StGL/StGLContext.h>
#include <stAssert.h>

StGLFrameTexture::StGLFrameTexture()
: StGLTexture(GL_RGB8),
  myDataSize(1.0f, 1.0f),
  myDisplayRatio(1.0f) {
    //
}

StGLFrameTextures::StGLFrameTextures()
: myParams(),
  myImgCM(StImage::ImgColor_RGB) {
    //
}

StGLFrameTextures::~StGLFrameTextures() {
    ST_ASSERT(!myTextures[0].isValid()
           && !myTextures[1].isValid()
           && !myTextures[2].isValid()
           && !myTextures[3].isValid(),
              "~StGLFrameTextures() with unreleased GL resources");
}

void StGLFrameTextures::release(StGLContext& theCtx) {
    myTextures[0].release(theCtx);
    myTextures[1].release(theCtx);
    myTextures[2].release(theCtx);
    myTextures[3].release(theCtx);
}

void StGLFrameTextures::increaseSize(StGLContext&      theCtx,
                                     StGLFrameTexture& theTexture,
                                     const GLsizei     theTextureSizeX,
                                     const GLsizei     theTextureSizeY) {
    // test existing size / new size
    /// TODO (Kirill Gavrilov#8) we can automatically reduce texture size here
    if((theTexture.getSizeX() < theTextureSizeX) ||
       (theTexture.getSizeY() < theTextureSizeY) ||
       !theTexture.isValid()) {
        ST_DEBUG_LOG("Requested texture size (" + theTextureSizeX + 'x' + theTextureSizeY
                   + ") larger than current texture size(" + theTexture.getSizeX() + 'x' + theTexture.getSizeY() + ')');
        const GLsizei anOriginalSizeX = theTexture.getSizeX();
        const GLsizei anOriginalSizeY = theTexture.getSizeY();

        const GLint aMaxTexDim = theCtx.getMaxTextureSize();
        GLsizei aNewSizeX = getPowerOfTwo(theTextureSizeX, GLsizei(aMaxTexDim));
        GLsizei aNewSizeY = getPowerOfTwo(theTextureSizeY, GLsizei(aMaxTexDim));
        if((aNewSizeY != anOriginalSizeY)
        || (aNewSizeX != anOriginalSizeX)) {
            if(!theTexture.initTrash(theCtx, aNewSizeX, aNewSizeY)) {
                theTexture.initTrash(theCtx,
                                     (anOriginalSizeX > 0) ? anOriginalSizeX : 512,
                                     (anOriginalSizeY > 0) ? anOriginalSizeY : 512);
                ST_DEBUG_LOG("FAILED to Increase the texture size to (" + aNewSizeX + 'x' +  aNewSizeY + ")!");
            } else {
                ST_DEBUG_LOG("Increase the texture size to (" + aNewSizeX + 'x' +  aNewSizeY + ") success!");
            }
        } else {
            ST_DEBUG_LOG("Not possible to Increase the texture size!");
        }
    }
}

void StGLFrameTextures::preparePlane(StGLContext&  theCtx,
                                     const size_t  thePlaneId,
                                     const GLsizei theSizeX,
                                     const GLsizei theizeY,
                                     const GLint   theInternalFormat) {

    if(myTextures[thePlaneId].getTextureFormat() != theInternalFormat) {
        // wrong texture format
        myTextures[thePlaneId].release(theCtx);
        myTextures[thePlaneId].setTextureFormat(theInternalFormat);
    }
    increaseSize(theCtx, myTextures[thePlaneId], theSizeX, theizeY);
}

void StGLFrameTextures::setMinMagFilter(StGLContext& theCtx,
                                        const GLenum theMinMagFilter) {
    myTextures[0].setMinMagFilter(theCtx, theMinMagFilter);
    /// TODO (Kirill Gavrilov#4) investigate
    myTextures[1].setMinMagFilter(theCtx, GL_LINEAR);
    myTextures[2].setMinMagFilter(theCtx, GL_LINEAR);
    myTextures[3].setMinMagFilter(theCtx, GL_LINEAR);
}

void StGLQuadTexture::setMinMagFilter(StGLContext& theCtx,
                                      const GLenum theMinMagFilter) {
    myTextures[FRONT_TEXTURE +  LEFT_TEXTURE].setMinMagFilter(theCtx, theMinMagFilter);
    myTextures[FRONT_TEXTURE + RIGHT_TEXTURE].setMinMagFilter(theCtx, theMinMagFilter);
    myTextures[BACK_TEXTURE  +  LEFT_TEXTURE].setMinMagFilter(theCtx, theMinMagFilter);
    myTextures[BACK_TEXTURE  + RIGHT_TEXTURE].setMinMagFilter(theCtx, theMinMagFilter);
}

StGLQuadTexture::StGLQuadTexture()
: myActive(true) {
    //
}

StGLQuadTexture::~StGLQuadTexture() {
    ST_ASSERT(!myTextures[0].isValid()
           && !myTextures[1].isValid()
           && !myTextures[2].isValid()
           && !myTextures[3].isValid(),
              "~StGLQuadTexture() with unreleased GL resources");
}

void StGLQuadTexture::release(StGLContext& theCtx) {
    myTextures[0].release(theCtx);
    myTextures[1].release(theCtx);
    myTextures[2].release(theCtx);
    myTextures[3].release(theCtx);
}