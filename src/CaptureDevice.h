#pragma once

//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"

#define CAPTURE_DEVICE_BLACKMAGIC_OFFSET 128

class CaptureDevice
{
    public:
        CaptureDevice();
        virtual ~CaptureDevice();
		void openUSB(int deviceId, int width, int height);
//		void openBlackMagic(int deviceId, int width, int height, ofxBlackmagic::DeviceDefinition device);
//		BMDDisplayMode decideBMDMode(int width, int height);
        void close();
        void update();
        ofPixelsRef getPixelsRef();
        bool isFrameNew();
        bool isInitialized();
		bool isUSBDevice();
		bool isBlackMagicDevice();

    protected:
        int deviceId;
        int captureWidth, captureHeight;
        ofVideoGrabber usbGrabber;
//		shared_ptr<ofxBlackmagic::Input> blackMagicGrabber;

    private:
};
