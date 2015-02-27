#pragma once

//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"
#include "ofxOpenCv.h"

#define OFX_IMAGE_ROI_HANDLE_TWOPOINTS   0x0001
#define OFX_IMAGE_ROI_HANDLE_FOURPOINTS  0x0002

class ofxImageROI : public ofImage
{
    public:
        ofxImageROI();
        virtual ~ofxImageROI();

        ofPoint cornerPoints[4];
        void setROI();
        void setROI(ofPoint &p, float w, float h);
        void setROI(ofRectangle rect);
        void draw(int x, int y);
        void draw(int x, int y, float w, float h);
        void enableMouseControls();
        void disableMouseControls();
        void enableKeyboardShortcuts();
        void disableKeyboardShortcuts();
        ofRectangle getROI();
        void setMode(int mode);
        void setAnchorSize(float size);
        void showHandles();
        void hideHandles();
        void toggleHandles();
        bool isShowingHandles();
        bool isMouseEnabled();
        void resetROI();
        bool isSetROI();
        void setOffsetMatrix(ofMatrix4x4 offset);

        void projectToTarget(ofPoint *dstPoints);
        void projectToSourceRect();
        ofImage getProjectedImage();

    protected:
        void onMouseMoved(ofMouseEventArgs & mouseArgs);
        void onMousePressed(ofMouseEventArgs & mouseArgs);
        void onMouseDragged(ofMouseEventArgs & mouseArgs);
        void onMouseReleased(ofMouseEventArgs & mouseArgs);
        void keyPressed(ofKeyEventArgs & keyArgs);
        float anchorSize;
        bool bShow;
        bool bMouseEnabled;
        bool bKeyboardShortcuts;
        ofPoint position;
        int selectedCornerIndex;
        int highlightCornerIndex;
        int mode;
        ofMatrix4x4 offsetMat;

        ofxCvColorImage inputImg;
        ofxCvColorImage outputImg;
        ofImage projectedImage;

    private:
        bool bSetROI;
        ofPoint tmpCornerPoints[4];
        float scale_w;
        float scale_h;
};
