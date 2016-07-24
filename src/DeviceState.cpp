//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "DeviceState.h"

void DeviceState::setup()
{
    for (int i = 0; i < 2; i++) {
        ofMatrix4x4 m;
        m.setTranslation(getSharedData().screenWidth / 2 * i, 0, 0);
        getSharedData().roiImage[i].setOffsetMatrix(m);
    }
}

void DeviceState::update()
{
}

void DeviceState::draw()
{
    ofSetColor(255);
    ofBackgroundHex(0xcccccc);

	int screenWidth = getSharedData().screenWidth;
    int screenHeight = getSharedData().screenHeight;
    int previewWidth = getSharedData().previewWidth;
    int previewHeight = getSharedData().previewHeight;

    int x = (screenWidth / 2 - previewWidth) / 2;
    int y = (screenHeight - previewHeight) / 2;

    for (int i = 0; i < 2; i++) {
        if (getSharedData().capDeviceMgr.isDeviceActive(i)) {
            // ofPushMatrixを使うとマウス座標が正常に取れなくなる
            ofPushMatrix();
            ofTranslate(screenWidth / 2 * i, 0);

            if (getSharedData().capDeviceMgr.isInitialized(i)) {
                if (getSharedData().roiImage[i].isAllocated()) {
                    getSharedData().roiImage[i].draw(x, y, previewWidth, previewHeight);
                }
            }
            ofPopMatrix();
        }
    }

    // デバッグ情報表示
    getSharedData().dispDebugInfo();
}

string DeviceState::getName()
{
	return "device";
}

