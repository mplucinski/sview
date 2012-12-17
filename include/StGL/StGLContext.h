/**
 * Copyright © 2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLContext_h_
#define __StGLContext_h_

#include <StTemplates/StHandle.h>
#include <StTemplates/StRect.h>

#include <StStrings/StString.h>

#ifdef __APPLE__
    #include <StLibrary.h>
#endif

// forward declarations - you should include appropriate header to use required GL version
struct StGLFunctions;
struct StGLArbFbo;

struct StGLCore11;
struct StGLCore11Fwd;

template<typename theBaseClass_t> struct stglTmplCore12;
typedef stglTmplCore12<StGLCore11>     StGLCore12;
typedef stglTmplCore12<StGLCore11Fwd>  StGLCore12Fwd;

struct StGLCore13;
struct StGLCore13Fwd;

template<typename theBaseClass_t> struct stglTmplCore14;
typedef stglTmplCore14<StGLCore13>     StGLCore14;
typedef stglTmplCore14<StGLCore13Fwd>  StGLCore14Fwd;

template<typename theBaseClass_t> struct stglTmplCore15;
typedef stglTmplCore15<StGLCore14>     StGLCore15;
typedef stglTmplCore15<StGLCore14Fwd>  StGLCore15Fwd;

template<typename theBaseClass_t> struct stglTmplCore20;
typedef stglTmplCore20<StGLCore15>     StGLCore20;
typedef stglTmplCore20<StGLCore15Fwd>  StGLCore20Fwd;

template<typename theBaseClass_t> struct stglTmplCore21;
typedef stglTmplCore21<StGLCore20>     StGLCore21;
typedef stglTmplCore21<StGLCore20Fwd>  StGLCore21Fwd;

template<typename theBaseClass_t> struct stglTmplCore30;
typedef stglTmplCore30<StGLCore21>     StGLCore30;
typedef stglTmplCore30<StGLCore21Fwd>  StGLCore30Fwd;

template<typename theBaseClass_t> struct stglTmplCore31;
typedef stglTmplCore31<StGLCore30>     StGLCore31Back;
typedef stglTmplCore31<StGLCore30Fwd>  StGLCore31;

template<typename theBaseClass_t> struct stglTmplCore32;
typedef stglTmplCore32<StGLCore31Back> StGLCore32Back;
typedef stglTmplCore32<StGLCore31>     StGLCore32;

template<typename theBaseClass_t> struct stglTmplCore33;
typedef stglTmplCore33<StGLCore32Back> StGLCore33Back;
typedef stglTmplCore33<StGLCore32>     StGLCore33;

template<typename theBaseClass_t> struct stglTmplCore40;
typedef stglTmplCore40<StGLCore33Back> StGLCore40Back;
typedef stglTmplCore40<StGLCore33>     StGLCore40;

template<typename theBaseClass_t> struct stglTmplCore41;
typedef stglTmplCore41<StGLCore40Back> StGLCore41Back;
typedef stglTmplCore41<StGLCore40>     StGLCore41;

template<typename theBaseClass_t> struct stglTmplCore42;
typedef stglTmplCore42<StGLCore41Back> StGLCore42Back;
typedef stglTmplCore42<StGLCore41>     StGLCore42;

/**
 * Class provides access to OpenGL functions.
 */
class ST_LOCAL StGLContext {

        public:    //! @name OpenGL functions - core versions

    StGLCore11*     core11;     //!< OpenGL 1.1 core functionality
    StGLCore11Fwd*  core11fwd;  //!< OpenGL 1.1 without deprecated entry points
    StGLCore20*     core20;     //!< OpenGL 2.0 core functionality (includes 1.5)
    StGLCore20Fwd*  core20fwd;  //!< OpenGL 2.0 without deprecated entry points
    StGLCore32*     core32;     //!< OpenGL 3.2 core profile
    StGLCore32Back* core32back; //!< OpenGL 3.2 backward compatibility profile
    StGLCore41*     core41;     //!< OpenGL 4.1 core profile
    StGLCore41Back* core41back; //!< OpenGL 4.1 backward compatibility profile
    StGLCore42*     core42;     //!< OpenGL 4.2 core profile
    StGLCore42Back* core42back; //!< OpenGL 4.2 backward compatibility profile

        public:    //! @name OpenGL functions - extensions

    StGLArbFbo*     arbFbo;     //!< GL_ARB_framebuffer_object

        public:    //! @name class interface

    /**
     * Default constructor.
     */
    StGLContext(const bool theToInitialize = false);

    /**
     * Destructor.
     */
    virtual ~StGLContext();

    /**
     * Initialize class with currently bound context.
     */
    bool stglInit();

    /**
     * Check string in GL extensions.
     */
    bool stglCheckExtension(const char* theExtName) const;

    /**
     * Auxiliary template to retrieve GL function pointer + cast it to specified prototype.
     * Notice that not NULL pointer doesn't means success.
     * You should check extension availability before this!
     */
    template <typename Function_t>
    bool stglFindProc(const char* theName,
                      Function_t& theFunction) const {
        theFunction = (Function_t )stglFindProc(theName);
        return (theFunction != NULL);
    }

    /**
     * @return true if detected GL version is greater or equal to requested one.
     */
    inline bool isGlGreaterEqual(const GLint theMajor,
                                 const GLint theMinor) {
        return (myVerMajor >  theMajor)
            || (myVerMajor == theMajor && myVerMinor >= theMinor);
    }

    inline GLint getVersionMajor() const {
        return myVerMajor;
    }

    inline GLint getVersionMinor() const {
        return myVerMinor;
    }

    /**
     * @return value for GL_MAX_TEXTURE_SIZE.
     */
    inline GLint getMaxTextureSize() const {
        return myMaxTexDim;
    }

    /**
     * Retrieve info from OpenGL context and create info string.
     */
    static StString stglInfo();

    /**
     * Retrieve info from OpenGL context and create info string.
     */
    StString stglFullInfo() const;

    /**
     * Setup viewport.
     */
    void stglResizeViewport(GLsizei theSizeX,
                            GLsizei theSizeY);

    /**
     * Setup viewport.
     */
    inline void stglResize(const StRect<GLint>& theWinRect) {
        stglResizeViewport(theWinRect.width(), theWinRect.height());
    }

    /**
     * Control VSync.
     */
    bool stglSetVSync(const bool theVSyncOn);

    /**
     * Clean up errors stack for this GL context (glGetError() in loop).
     */
    void stglResetErrors();

    /**
     * Method to detect obsolete hardware.
     */
    bool stglIsRectangularFboSupported() const {
        return myIsRectFboSupported;
    }

        protected: //! @name auxiliary methods

    /**
     * Calls system function to retrieve GL function pointer by name.
     */
    void* stglFindProc(const char* theName) const;

    /**
     * Check string in specified string
     * (old way with huge string for all extensions).
     */
    bool stglCheckExtension(const char* theStringList,
                            const char* theName) const;

    /**
     * Read OpenGL version information from active context.
     */
    void stglReadVersion();

        private:   //! @name copying is forbidden

    StGLContext           (const StGLContext& theCopy);
    StGLContext& operator=(const StGLContext& theCopy);

        protected: //! @name class fields

#ifdef __APPLE__
    StLibrary               mySysLib;             //!< optional handle to system GL library (MacOS X specific)
#endif
    StHandle<StGLFunctions> myFuncs;              //!< mega structure for all GL functions
    GLint                   myVerMajor;           //!< cached GL version major number
    GLint                   myVerMinor;           //!< cached GL version minor number
    GLint                   myMaxTexDim;          //!< maximum texture dimension
    bool                    myIsRectFboSupported; //!< compatibility flag
    bool                    myWasInit;            //!< initialization state

};

#endif // __StGLContext_h_