//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "SharedData.h"

void SharedData::showROIHandles()
{
    for (int i = 0; i < 2; i++) {
        if (roiImage[i].isMouseEnabled() == false) {
            roiImage[i].enableMouseControls();
            roiImage[i].showHandles();
        }
    }
}

void SharedData::hideROIHandles()
{
    for (int i = 0; i < 2; i++) {
        if (roiImage[i].isMouseEnabled()) {
            roiImage[i].disableMouseControls();
            roiImage[i].hideHandles();
        }
    }
}

void SharedData::updateROI()
{
    for (int i = 0; i < 2; i++) {
        ofRectangle roi = roiImage[i].getROI();
        fbo[i].allocate(roi.width, roi.height);
        warperOut[i].setSourceRect(ofRectangle(0, 0, roi.width, roi.height));

        if (i == activeCameraNo) {
            // 操作対象のカメラの場合
            warperActive.setSourceRect(ofRectangle(0, 0, roi.width, roi.height));
        }
    }
}

void SharedData::updateActiveDstPoints()
{
    for (int i = 0; i < 4; i++) {
        warperActive.dstPoints[i] = warperOut[activeCameraNo].dstPoints[i];
    }
}

void SharedData::toggleDebugMode()
{
    bDebugMode = !bDebugMode;
}

void SharedData::dispDebugInfo()
{
    if (bDebugMode) {
        ofPushStyle();
        ofSetColor(ofColor::white);
        ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 10, ofGetHeight() - 10);
        ofPopStyle();
    }
}
