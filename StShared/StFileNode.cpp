/**
 * Copyright © 2010-2011 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#if (defined(__APPLE__))
    // stat64 deprecated since Mac OS X 10.6
    #define _DARWIN_USE_64_BIT_INODE
#endif

#include <StFile/StFileNode.h>
#include <StFile/StMIMEList.h>

#if(defined(_WIN32) || defined(__WIN32__))
    #include <windows.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

StFileNode::~StFileNode() {
    //
}

StHandle<StFileNode> StFileNode::detach() const {
    StHandle<StFileNode> aCopy = new StFileNode(getPath());
    aCopy->setMIME(getMIME());
    for(size_t aSubId = 0; aSubId < size(); ++aSubId) {
        const StFileNode* aSubNode = getValue(aSubId);
        aCopy->add(new StFileNode(aSubNode->getSubPath(), aCopy.access()));
    }
    return aCopy;
}

bool StFileNode::isFileExists(const StString& thePath) {
#if(defined(_WIN32) || defined(__WIN32__))
    struct __stat64 aStatBuffer;
    return _wstat64(thePath.toUtfWide().toCString(), &aStatBuffer) == 0;
#elif (defined(__APPLE__))
    struct stat aStatBuffer;
    return stat(thePath.toCString(), &aStatBuffer) == 0;
#else
    struct stat64 aStatBuffer;
    return stat64(thePath.toCString(), &aStatBuffer) == 0;
#endif
}

bool StFileNode::removeFile(const StString& thePath) {
#if(defined(_WIN32) || defined(__WIN32__))
    return DeleteFileW(thePath.toUtfWide().toCString()) != 0;
#else
    return ::remove(thePath.toCString()) == 0;
#endif
}

/**bool StFileNode::copyFile(const StString& thePathFrom,
                          const StString& thePathTo) {
#if(defined(_WIN32) || defined(__WIN32__))
    return CopyFileW(thePathFrom.toUtfWide().toCString(),
                     thePathTo.toUtfWide().toCString(), TRUE) != 0;
#else

#endif
}*/

bool StFileNode::moveFile(const StString& thePathFrom,
                          const StString& thePathTo) {
#if(defined(_WIN32) || defined(__WIN32__))
    return MoveFileW(thePathFrom.toUtfWide().toCString(),
                     thePathTo.toUtfWide().toCString()) != 0;
#else
    return ::rename(thePathFrom.toCString(),
                    thePathTo.toCString()) == 0;
#endif
}

StString StFileNode::getCompatibleName(const StString& theFileName) {
#if(defined(_WIN32) || defined(__WIN32__))
    /// TODO (Kirill Gavrilov#1) if result is empty - not a filesystem element or no DOS-names enabled
    stUtfWide_t aShortNameWide[MAX_PATH];
    GetShortPathName(theFileName.toUtfWide().toCString(), aShortNameWide, MAX_PATH);
    return *aShortNameWide != L'\0' ? StString(aShortNameWide) : theFileName;
#else
    return theFileName;
#endif
}

StString StFileNode::getExtension(const StString& theFileName) {
    size_t aPntId = size_t(-1);
    for(StUtf8Iter anIter = theFileName.iterator(); *anIter != 0; ++anIter) {
        if(*anIter == stUtf32_t('.')) {
            aPntId = anIter.getIndex();
        }
    }
    return (aPntId != size_t(-1)) ? theFileName.subString(aPntId + 1, theFileName.getLength()) : StString();
}

void StFileNode::getNameAndExtension(const StString& theFileName,
                                     StString& theName,
                                     StString& theExtension) {
    size_t aLastPnt = size_t(-1);
    for(StUtf8Iter anIter = theFileName.iterator(); *anIter != 0; ++anIter) {
        if(*anIter == stUtf32_t('.')) {
            aLastPnt = anIter.getIndex();
        }
    }
    if(aLastPnt != size_t(-1)) {
        theName      = theFileName.subString(0, aLastPnt);
        theExtension = theFileName.subString(aLastPnt + 1, theFileName.getLength());
    } else {
        theName      = theFileName;
        theExtension = StString();
    }
}

void StFileNode::getFolderAndFile(const StString& theFilePath,
                                  StString& theFolder,
                                  StString& theFileName) {
    size_t aLastSplit = size_t(-1);
    for(StUtf8Iter anIter = theFilePath.iterator(); *anIter != 0; ++anIter) {
        if(*anIter == stUtf32_t(SYS_FS_SPLITTER)) {
            aLastSplit = anIter.getIndex();
        }
    }

    if(aLastSplit != size_t(-1)) {
        theFolder   = theFilePath.subString(0, aLastSplit);
        theFileName = theFilePath.subString(aLastSplit + 1, theFilePath.getLength());
    } else {
        theFolder   = StString();
        theFileName = theFilePath;
    }
}