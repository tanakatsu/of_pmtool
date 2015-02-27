//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"
#include "CaptureDeviceManager.h"

CaptureDeviceManager::CaptureDeviceManager()
{
    for (int i = 0; i < 2; i++) {
        selectedDeviceId[i] = -1;
    }

	captureWidth = 1920;
	captureHeight = 1080;
}

CaptureDeviceManager::~CaptureDeviceManager()
{
}

void CaptureDeviceManager::setup(int width, int height)
{
	captureWidth = width;
	captureHeight = height;

	// USB�J�����̃��X�g�擾
	usbDevices = dummyGrabber.listDevices();

    for(int i = 0; i < usbDevices.size(); i++){
        string deviceName = usbDevices[i].deviceName;

		cout << usbDevices[i].id << ": " << deviceName;
        if( usbDevices[i].bAvailable
           && deviceName != "Blackmagic WDM Capture"
           && deviceName != "Decklink Video Capture"){
            availableDeviceNames.push_back(deviceName);
            deviceIds.push_back(i);
            deviceOwnerCameraNo.push_back(-1);
            cout << endl;
        }else{
			deviceOwnerCameraNo.push_back(-1);
            cout << " - unavailable " << endl;
        }
	}

	// BlackMagic�L���v�`���[�{�[�h�̃��X�g�擾
	/*
	auto deviceList = ofxBlackmagic::Iterator::getDeviceList();
	int no = CAPTURE_DEVICE_BLACKMAGIC_OFFSET; // USB�J�����Ƌ�ʂ��邽�߂ɃI�t�Z�b�g����

	for(auto device : deviceList) {
		cout << no << ": " << device.modelName << endl;

        availableDeviceNames.push_back(device.modelName);
        deviceIds.push_back(no);
        deviceOwnerCameraNo.push_back(-1);
		blackMagicDevices.push_back(device);

		++no;
	}
	*/
}

void CaptureDeviceManager::setup()
{
	setup(1920, 1080);
}

vector<string> CaptureDeviceManager::getDeviceList()
{
    return availableDeviceNames;
}

int CaptureDeviceManager::getDeviceIdByIndex(int index)
{
    return deviceIds[index];
}

void CaptureDeviceManager::onDeviceSelected(int cameraNo, int deviceId)
{
    if (cameraNo < 0) return;
    if (selectedDeviceId[cameraNo] == deviceId) return;

    // ���ݐڑ����̃f�o�C�X�����邩�`�F�b�N����
    int deviceIdOwnedByMe = -1;
    for (int i = 0; i < deviceOwnerCameraNo.size(); i++) {
        if (deviceOwnerCameraNo[i] == cameraNo) {
            deviceIdOwnedByMe = i;
            break;
        }
    }

    // ���ݐڑ����̃f�o�C�X���������
    if (deviceIdOwnedByMe >= 0) {
        selectedDeviceId[cameraNo] = -1; // ��U�N���A
        deviceOwnerCameraNo[deviceIdOwnedByMe] = -1; // �N���A

        // ��������f�o�C�X�𑼂̃J�������I�����Ă����ꍇ
        for (int i = 0; i < 2; i++) {
            if (selectedDeviceId[i] == deviceId) {
				open(i, deviceId, captureWidth, captureHeight);
                deviceOwnerCameraNo[deviceId] = i; // �I�[�i�[�����X�V����
                break;
            }
        }
    }

    if (deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)] < 0) { // ���̃J�����Ŏg�p����ĂȂ��ꍇ
        deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)] = cameraNo;
        open(cameraNo, deviceId, captureWidth, captureHeight);
    }
    selectedDeviceId[cameraNo] = deviceId;
}

int CaptureDeviceManager::getIndexFromDeviceId(int deviceId)
{
	int index = deviceId;

	if (deviceId >= CAPTURE_DEVICE_BLACKMAGIC_OFFSET) {
		index = deviceId - CAPTURE_DEVICE_BLACKMAGIC_OFFSET + usbDevices.size();
	}

	return index;
}

void CaptureDeviceManager::open(int cameraNo, int deviceId, int width, int height)
{
	if (deviceId >= 0 && deviceId < CAPTURE_DEVICE_BLACKMAGIC_OFFSET) {
		captureDevice[cameraNo].openUSB(deviceId, width, height);
	} else if (deviceId >= CAPTURE_DEVICE_BLACKMAGIC_OFFSET) {
//		captureDevice[cameraNo].openBlackMagic(deviceId, width, height, blackMagicDevices[deviceId - CAPTURE_DEVICE_BLACKMAGIC_OFFSET]);
	}
}

void CaptureDeviceManager::update(int cameraNo)
{
    if (cameraNo < 0) return;

    int deviceId = selectedDeviceId[cameraNo];

    if (deviceId >= 0 && deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)] == cameraNo) {
        captureDevice[cameraNo].update();
    }
}

void CaptureDeviceManager::updateAll()
{
    for (int i = 0; i < 2; i++) {
        update(i);
    }
}

bool CaptureDeviceManager::isDeviceActive(int cameraNo)
{
    return selectedDeviceId[cameraNo] >= 0;
}

ofPixelsRef CaptureDeviceManager::getPixelsRef(int cameraNo)
{
    int deviceId = selectedDeviceId[cameraNo];
    int ownerCameraNo = deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)];

    return captureDevice[ownerCameraNo].getPixelsRef();
}

bool CaptureDeviceManager::isFrameNew(int cameraNo)
{
    int deviceId = selectedDeviceId[cameraNo];

    if (deviceId < 0) {
        return false;
    }

    int ownerCameraNo = deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)];
    return captureDevice[ownerCameraNo].isFrameNew();
}

bool CaptureDeviceManager::isInitialized(int cameraNo)
{
    int deviceId = selectedDeviceId[cameraNo];

    if (deviceId < 0) {
        return false;
    }

    int ownerCameraNo = deviceOwnerCameraNo[getIndexFromDeviceId(deviceId)];
    return captureDevice[ownerCameraNo].isInitialized();
}
