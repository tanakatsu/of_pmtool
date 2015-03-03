//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ProjectionState.h"

void ProjectionState::setup()
{
    int screenWidth = getSharedData().screenWidth;
    int screenHeight = getSharedData().screenHeight;

    getSharedData().warperActive.setTopLeftCornerPosition(ofPoint(0, 0));             // this is position of the quad warp corners, centering the image on the screen.
    getSharedData().warperActive.setTopRightCornerPosition(ofPoint(screenWidth, 0));        // this is position of the quad warp corners, centering the image on the screen.
    getSharedData().warperActive.setBottomRightCornerPosition(ofPoint(screenWidth, screenHeight)); // this is position of the quad warp corners, centering the image on the screen.
    getSharedData().warperActive.setBottomLeftCornerPosition(ofPoint(0, screenHeight));      // this is position of the quad warp corners, centering the image on the screen.
    getSharedData().warperActive.setup();

    for (int i = 0; i < 2; i++) {
        getSharedData().warperOut[i].setTopLeftCornerPosition(ofPoint(0, 0));
        getSharedData().warperOut[i].setTopRightCornerPosition(ofPoint(screenWidth, 0));
        getSharedData().warperOut[i].setBottomRightCornerPosition(ofPoint(screenWidth, screenHeight));
        getSharedData().warperOut[i].setBottomLeftCornerPosition(ofPoint(0, screenHeight));
    }
}

void ProjectionState::update()
{
    for (int i = 0; i < 2; i++) {
        if (getSharedData().capDeviceMgr.isFrameNew(i)) {
            ofRectangle roi = getSharedData().roiImage[i].getROI();

            getSharedData().roiImage[i].setFromPixels(getSharedData().capDeviceMgr.getPixelsRef(i));
            getSharedData().roiImage[i].crop(roi.x, roi.y, roi.width, roi.height);
        }
    }

    // 操作中の変換ターゲット領域を反映
    int activeCameraNo = getSharedData().activeCameraNo;
    if (activeCameraNo >= 0 && activeCameraNo < 2) {
        for (int i = 0; i < 4; i++) {
            getSharedData().warperOut[activeCameraNo].dstPoints[i] = getSharedData().warperActive.dstPoints[i];
        }
    }
}

void ProjectionState::draw()
{
    int activeCameraNo = getSharedData().activeCameraNo;

    if (activeCameraNo < 0 || activeCameraNo >= 2) return;

    ofSetColor(255);
	ofBackgroundHex(0x000000);

    //======================== draw image into fbo.

    for (int i = 0; i < 2; i++) {
        getSharedData().fbo[i].begin();
        if (getSharedData().roiImage[i].isAllocated()) {
            getSharedData().roiImage[i].draw(0, 0);
        }
        getSharedData().fbo[i].end();
    }

    //======================== get our quad warp matrix.

    ofMatrix4x4 mat = getSharedData().warperActive.getMatrix();
    ofMatrix4x4 matOut[2];
    for (int i =0; i < 2; i++) {
        matOut[i] = getSharedData().warperOut[i].getMatrix();
    }

    //======================== use the matrix to transform our fbo.

    if (getSharedData().capDeviceMgr.isDeviceActive(activeCameraNo)) {
        glPushMatrix();
        glMultMatrixf(mat.getPtr());
        getSharedData().fbo[activeCameraNo].draw(0, 0);
        glPopMatrix();
    }

    for (int i = 0; i < 2; i++) {
        if (getSharedData().capDeviceMgr.isDeviceActive(i)) {
            glPushMatrix();
            ofTranslate(getSharedData().screenWidth * (i + 1), 0);
            glMultMatrixf(matOut[i].getPtr());
            getSharedData().fbo[i].draw(0, 0);
            glPopMatrix();
        }
    }

    if (getSharedData().capDeviceMgr.isDeviceActive(activeCameraNo)) {
        //======================== use the matrix to transform points.

        ofSetLineWidth(2);
        ofSetColor(ofColor::cyan);

        //======================== draw quad warp ui.

        ofSetColor(ofColor::magenta);
        getSharedData().warperActive.drawQuadOutline();

        ofSetColor(ofColor::yellow);
        getSharedData().warperActive.drawCorners();

        ofSetColor(ofColor::magenta);
        getSharedData().warperActive.drawHighlightedCorner();

        ofSetColor(ofColor::red);
        getSharedData().warperActive.drawSelectedCorner();
    }

    // デバッグ情報表示
    getSharedData().dispDebugInfo();
}

void ProjectionState::keyPressed(int key)
{

}

string ProjectionState::getName()
{
	return "projection";
}
