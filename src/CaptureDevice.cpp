//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"
#include "CaptureDevice.h"

CaptureDevice::CaptureDevice()
{
    deviceId = -1;
//	blackMagicGrabber = shared_ptr<ofxBlackmagic::Input>(new ofxBlackmagic::Input());
}

CaptureDevice::~CaptureDevice()
{
}

void CaptureDevice::openUSB(int deviceId, int width, int height)
{
    close();

    this->deviceId = deviceId;

	if (isUSBDevice()) {
		usbGrabber.setDeviceID(deviceId);
		usbGrabber.setDesiredFrameRate(60);
		usbGrabber.initGrabber(width, height);
	} else {
		ofLog(OF_LOG_ERROR, "Not USB device");
	}
}

/*
void CaptureDevice::openBlackMagic(int deviceId, int width, int height, ofxBlackmagic::DeviceDefinition device)
{
    close();

    this->deviceId = deviceId;

	if (isBlackMagicDevice()) {
		blackMagicGrabber->startCapture(device, decideBMDMode(width, height));
	} else {
		ofLog(OF_LOG_ERROR, "Not Blackmagic device");
	}
}
*/

/*
BMDDisplayMode CaptureDevice::decideBMDMode(int width, int height)
{
	BMDDisplayMode bmdMode = bmdModeHD1080p2997;

	if (width == 1280 && height == 720) {
		bmdMode = bmdModeHD720p5994;
	} else if (width == 1920 && height == 1080) {
		bmdMode = bmdModeHD1080p2997;
	} else {
		ofLog(OF_LOG_ERROR, "Unknown capture mode");
	}

	return bmdMode;
}
*/

void CaptureDevice::close()
{
    if (isUSBDevice()) {
        if (usbGrabber.isInitialized()) {
            usbGrabber.close();
        }
    } else if (isBlackMagicDevice()) {
//		blackMagicGrabber->stopCapture();
	}
}

void CaptureDevice::update()
{
    if (isUSBDevice()) {
        usbGrabber.update();
    } else if (isBlackMagicDevice()) {
//		blackMagicGrabber->update();
	}
}

ofPixelsRef CaptureDevice::getPixelsRef()
{
	if (isUSBDevice()) {
		return usbGrabber.getPixelsRef();
	} else if (isBlackMagicDevice()) {
//		return blackMagicGrabber->getPixelsRef();
	}

	ofPixels empty;
	return empty;
}

bool CaptureDevice::isFrameNew()
{
	if (isUSBDevice()) {
		return usbGrabber.isFrameNew();
	} else if (isBlackMagicDevice()) {
//		return blackMagicGrabber->isFrameNew();
	}

	return false;
}

bool CaptureDevice::isInitialized()
{
	if (isUSBDevice()) {
		return usbGrabber.isInitialized();
	} else if (isBlackMagicDevice()) {
		return true;
	}

	return false;
}

bool CaptureDevice::isUSBDevice()
{
	return deviceId >= 0 && deviceId < CAPTURE_DEVICE_BLACKMAGIC_OFFSET;
}

bool CaptureDevice::isBlackMagicDevice()
{
	return deviceId >= CAPTURE_DEVICE_BLACKMAGIC_OFFSET;
}
