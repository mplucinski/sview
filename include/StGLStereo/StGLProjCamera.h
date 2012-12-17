/**
 * Copyright © 2010-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLProjCamera_h_
#define __StGLProjCamera_h_

#include <StTemplates/StRect.h>
#include <StGL/StGLEnums.h>
#include <StGL/StGLMatrix.h>

class StGLContext;

/**
 * This class represents the PROJECTION camera settings and defines projection matrix as result.
 * That is - it defines:
 *  - projection type (orthographic or perspective);
 *  - projection options (near / far clipping planes, field of view);
 *  - stereoscopic distortions for perspective projection
 *    (Z-screen position of zero parallax and intra-ocular distance);
 * To setup the camera position, direction and rotation you should use another matrix (model view)!
 */
class ST_LOCAL StGLProjCamera {

        public:

    /**
     * Setup the default projection camera.
     */
    StGLProjCamera(const GLfloat theFOVy    = 45.0f,
                   const GLfloat theZNear   = 3.0f,
                   const GLfloat theZFar    = 30.0f,
                   const GLfloat theZScreen = 10.0f);

    /**
     * Get projection type.
     */
    bool isPerspective() const {
        return myIsPersp;
    }

    /**
     * Changes the projection type.
     */
    void setPerspective(const bool theIsPerspective);

    /**
     * Returns the linear zoom factor.
     * @return zoom factor.
     */
    GLfloat getZoom() const {
        return myZoom;
    }

    /**
     * Changes the linear zoom factor.
     * @param theZoom - zoom factor.
     */
    void setZoom(const GLfloat theZoom) {
        myZoom = theZoom;
    }

    /**
     * Returns the intraocular distance (stereo separation).
     * @return the intraocular distance.
     */
    GLfloat getIOD() const {
        return myIOD;
    }

    /**
     * Sets the intraocular distance (stereo separation).
     * @param theIOD - the intraocular distance.
     */
    void setIOD(const GLfloat theIOD) {
        myIOD = theIOD;
    }

    /**
     * Return the current Field Of View in y-axis.
     * @return FOVy - Field Of View in y-axis.
     */
    GLfloat getFOVy() const {
        return myFOVy;
    }

    /**
     * Setup the Field Of View in y-axis (degrees). Should be in 0 < FOVy < 180.
     * @param theFOVy - new Field Of View value.
     */
    void setFOVy(const GLfloat theFOVy) {
        myFOVy = theFOVy;
    }

    /**
     * Return the Field Of View in y-axis with linear zoom factor applied.
     * @return FOVy - Field Of View in y-axis (zoomed).
     */
    GLfloat getFOVyZoomed() const;

    /**
     * Returns the Near Z-clipping plane position.
     * @return Z-position.
     */
    GLfloat getZNear() const {
        return myFrustM.zNear;
    }

    /**
     * Get current Screen Z plane position. This is a stereoscopic focus (zero-parallax plane).
     * @return Z-position.
     */
    GLfloat getZScreen() const {
        return myZScreen;
    }

    /**
     * Setup the Screen Z plane position. This is a stereoscopic focus (zero-parallax plane).
     * Z-near and IOD will be proportionally changed!
     * @param theZScreen (GLfloat ) - Z-position.
     */
    void setZScreen(const GLfloat theZScreen);

    /**
     * Returns the Far Z-clipping plane position.
     * @return Z-position.
     */
    GLfloat getZFar() const {
        return myFrustM.zFar;
    }

    /**
     * Setup the Far Z-clipping plane. Does NOT affects the perspective distortions.
     * Should be larger than Near Z-clipping plane position and enough to fit whole scene.
     * Infinity values are OK but cause lower Z-buffer accuracy.
     * @param theZFar - Z-position.
     */
    void setZFar(const GLfloat theZFar) {
        myFrustM.zFar = myFrustL.zFar = myFrustR.zFar = theZFar;
    }

    /**
     * Compute the frustum section (rectangle) at current Z-screen position.
     */
    void getZParams(StRectD_t& theSectRect) const {
        getZParams(myZScreen, theSectRect);
    }

    /**
     * Compute the frustum section (rectangle) at specified Z-position
     * (perspective distortion).
     * @param theZValue   - Z position;
     * @param theSectRect - computed frustum section.
     */
    void getZParams(const GLdouble theZValue,
                    StRectD_t&     theSectRect) const;

    /**
     * Returns the GL projection matrix. It generally should NOT be changed outside.
     * @return the projection matrix.
     */
    const StGLMatrix& getProjMatrix() const {
        return myMatrix;
    }

    /**
     * Returns the mono GL projection matrix.
     * It could be used if some object should be always rendered in mono even in stereoscopic mode.
     * For example - the stereo-pairs which are prerendered stereoscopic scenes and should not be modified.
     * Use this method only when you sure!
     * @return the mono projection matrix.
     */
    const StGLMatrix& getProjMatrixMono() const {
        return myMatrixMono;
    }

    /**
     * Returns the display aspect ratio.
     */
    GLfloat getAspect() const {
        return myAspect;
    }

    /**
     * Update GL viewport and recompute projection matrix.
     */
    void resize(StGLContext&  theCtx,
                const GLsizei theSizeX,
                const GLsizei theSizeY);

    /**
     * Setup projection frustum.
     */
    void updateFrustum();

    /**
     * Setup camera projection matrices according to specified view.
     * @param theView (unsigned int ) - stereo flag (left eye / right eye / mono).
     */
    void setView(const unsigned int theView);

    /**
     * Setup projection matrix according to computed frustum.
     */
    void setupMatrix();

    /**
     * Setup current global OpenGL projection matrix (fixed pipeline!).
     * @deprecated This method uses deprecated OpenGL API!
     */
    void setupFixed(StGLContext& theCtx);

    const StGLVolume* getMonoFrustrum() const {
        return &myFrustM;
    }

    /**
     * Returns the string description for the camera.
     * For debug purposes...
     */
    StString toString() const;

        private:

    StGLMatrix  myMatrix;     //!< current projection matrix
    StGLMatrix  myMatrixMono; //!< we store mono projection matrix to allow draw special object
    GLfloat     myFOVy;       //!< field of view in y-axis (degrees)
    GLfloat     myZoom;       //!< linear zoom factor (changes the effective FOVy value!)
    GLfloat     myAspect;     //!< screen aspect ratio, recomputed on window size change
    GLfloat     myZScreen;    //!< screen projection plane
    GLfloat     myIOD;        //!< intraocular distance
    StGLVolume  myFrustL;     //!< frustum for left view
    StGLVolume  myFrustR;     //!< frustum for right view
    StGLVolume  myFrustM;     //!< frustum for mono view
    StGLVolume* myFrust;      //!< active frustum
    bool        myIsPersp;    //!< projection matrix type: perspective or orthogonal

};

#endif //__StGLProjCamera_h_