#pragma once

//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"
#include "CaptureDevice.h"

class CaptureDeviceManager
{
    public:
        CaptureDeviceManager();
        virtual ~CaptureDeviceManager();
		void setup();
		void setup(int width, int height);
        vector<string> getDeviceList();
        int getDeviceIdByIndex(int index);
        void onDeviceSelected(int cameraNo, int deviceId);
		void open(int cameraNo, int deviceId, int width, int height);
        void update(int cameraNo);
        void updateAll();
        bool isDeviceActive(int cameraNo);
        ofPixelsRef getPixelsRef(int cameraNo);
        bool isFrameNew(int cameraNo);
        bool isInitialized(int cameraNo);

    protected:
        CaptureDevice captureDevice[2];

        vector<string> availableDeviceNames;
        vector<int> deviceIds;
        vector<int> deviceOwnerCameraNo;

        ofVideoGrabber dummyGrabber;
        vector<ofVideoDevice> usbDevices;
//		vector<ofxBlackmagic::DeviceDefinition> blackMagicDevices;

		int captureWidth;
		int captureHeight;
        int selectedDeviceId[2];

    private:
		int getIndexFromDeviceId(int deviceId);
};
