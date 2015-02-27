#include "ofApp.h"
#include "DeviceState.h"
#include "ProjectionState.h"
#include "HelpState.h"
#include "ofxImageROI.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// システム設定のロード
    loadSystemSettings();

    int screenWidth = stateMachine.getSharedData().screenWidth;
    int screenHeight = stateMachine.getSharedData().screenHeight;
    int captureWidth = stateMachine.getSharedData().captureWidth;
    int captureHeight = stateMachine.getSharedData().captureHeight;
    int previewWidth = stateMachine.getSharedData().previewWidth;
    int previewHeight = stateMachine.getSharedData().previewHeight;

    // 初期ウインドウサイズのセット
    ofSetWindowShape(screenWidth, screenHeight);

    // デバイス名リスト作成
    stateMachine.getSharedData().capDeviceMgr.setup(captureWidth, captureHeight);

    // 共有データの初期化
    stateMachine.getSharedData().activeCameraNo = -1;

    // GUIの構築
    vector<string> device_names = stateMachine.getSharedData().capDeviceMgr.getDeviceList();
    for (int i = 0; i < 2; i++) {
        ddlGui[i] = new ofxUISuperCanvas("Camera" + ofToString(i + 1));
        ofxUIDropDownList *ddl = ddlGui[i]->addDropDownList("DEVICE LIST", device_names);
        ddl->setAllowMultiple(false);
        ddl->setAutoClose(true);
        ddl->setShowCurrentSelected(true);
        ddlGui[i]->autoSizeToFitWidgets();
        ddlGui[i]->setPosition(ofGetWidth() / 2 * i, 0);
        ofAddListener(ddlGui[i]->newGUIEvent, this, &ofApp::guiEvent);

        btnGui[i] = new ofxUICanvas();
        btnGui[i]->addLabelButton("Select", false);
        btnGui[i]->setModal(true);
        btnGui[i]->autoSizeToFitWidgets();
        int posX = screenWidth / 2 * i + screenWidth / 4 - btnGui[i]->getGlobalCanvasWidth() / 2;
        int posY = screenHeight / 2 + previewHeight;
        btnGui[i]->setPosition(posX, posY);
        btnGui[i]->setVisible(false);
        ofAddListener(btnGui[i]->newGUIEvent, this, &ofApp::guiEvent);
    }

    // 領域指定モジュールの初期化
    for (int i = 0; i < 2; i++) {
        stateMachine.getSharedData().roiImage[i].setROI(ofRectangle(0, 0, captureWidth, captureHeight));
        stateMachine.getSharedData().roiImage[i].showHandles();
    }

	// ステートマシン初期化
	stateMachine.addState<DeviceState>();
	stateMachine.addState<ProjectionState>();
	stateMachine.addState<HelpState>();
	stateMachine.changeState("device");

    // ディレイジョブ用の変数初期化
	changeStateFiredAt = 0;
	tmpActiveCameraNo = -1;
}

//--------------------------------------------------------------
void ofApp::update(){
    stateMachine.getSharedData().capDeviceMgr.updateAll();

    for (int i = 0; i < 2; i++) {
        if (stateMachine.getSharedData().capDeviceMgr.isFrameNew(i)) {
            stateMachine.getSharedData().roiImage[i].setFromPixels(stateMachine.getSharedData().capDeviceMgr.getPixelsRef(i));
        }
    }

    // ボタンのエフェクトが終了してから状態遷移する
    if (changeStateFiredAt > 0 && ofGetElapsedTimeMillis() > changeStateFiredAt) {
        changeStateFiredAt = 0;
        changeState("projection", tmpActiveCameraNo);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::exit()
{
    for (int i = 0; i < 2; i++) {
        delete ddlGui[i];
        delete btnGui[i];
    }
}
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    int cameraNo = -1;
    string name = e.widget->getName();
    int kind = e.widget->getKind();

    if (kind == OFX_UI_WIDGET_LABELTOGGLE) {
        ofxUISuperCanvas *parent = (ofxUISuperCanvas *)e.widget->getParent();
        ofxUISuperCanvas *canvasParent = (ofxUISuperCanvas *)e.widget->getCanvasParent();

        if (parent->getName() == "DEVICE LIST") {
            for (int i = 0; i < 2; i++) {
                if (canvasParent->getCanvasTitle() == ddlGui[i]->getCanvasTitle()) {
                    cameraNo = i;
                    break;
                }
            }
        }

        if (cameraNo < 0) return;

        cout << "CAMERA NO: " << cameraNo << endl;
        cout << "SELECTED: " << name << endl;

        vector<string> device_names = stateMachine.getSharedData().capDeviceMgr.getDeviceList();
        for (int i = 0; i < device_names.size(); i++) {
            if (device_names[i] == name) {
                int deviceId = stateMachine.getSharedData().capDeviceMgr.getDeviceIdByIndex(i);

                cout << "SELECTED DEVICE ID: " << deviceId << endl;

                onDeviceSelected(cameraNo, deviceId);
                break;
            }
        }
    } else if (kind == OFX_UI_WIDGET_LABELBUTTON) {
        ofxUISuperCanvas *canvasParent = (ofxUISuperCanvas *)e.widget->getCanvasParent();

        for (int i = 0; i < 2; i++) {
            if (canvasParent->getCanvasTitle() == ((ofxUISuperCanvas *)btnGui[i])->getCanvasTitle()) {
                if (stateMachine.getSharedData().capDeviceMgr.isDeviceActive(i)) {
                    // ディレイをつけて遷移しないとボタンのstateがおかしくなる
                    changeStateFiredAt = ofGetElapsedTimeMillis() + 150;
                    tmpActiveCameraNo = i;
                    break;
                }
            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::onDeviceSelected(int cameraNo, int deviceId)
{
    if (cameraNo < 0) return;

    // キャプチャデバイスを初期化
    stateMachine.getSharedData().capDeviceMgr.onDeviceSelected(cameraNo, deviceId);

	// ROIリセット
	ofxImageROI *roiImage = &stateMachine.getSharedData().roiImage[cameraNo];
    roiImage->resetROI();

    // ボタン表示
    btnGui[cameraNo]->setVisible(true);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
    case '0':
    case OF_KEY_BACKSPACE:
        changeState("device");
        break;
    case '1':
    case '2':
        changeState("projection", key - '1');
        break;
    case 's':
        saveUserSettings();
        break;
    case 'l':
        loadUserSettings();
        break;
    case 'd':
        stateMachine.getSharedData().toggleDebugMode();
        break;
    case 'h':
        changeState("help");
        break;
    case 'q':
        ofExit(0);
        break;
    }
}

void ofApp::changeState(string name, ...) {
    va_list args;

    va_start(args, name);

    if (name == "device") {
		ofSetWindowShape(stateMachine.getSharedData().screenWidth, stateMachine.getSharedData().screenHeight);
        stateMachine.changeState("device");
        for (int i = 0; i < 2; i++) {
            ddlGui[i]->setVisible(true);
            if (stateMachine.getSharedData().capDeviceMgr.isDeviceActive(i)) {
                btnGui[i]->setVisible(true);
            }
        }
        stateMachine.getSharedData().showROIHandles();
    } else if (name == "projection") {
		int fullScreenWidth = stateMachine.getSharedData().screenWidth * 3;
        ofSetWindowShape(fullScreenWidth, stateMachine.getSharedData().screenHeight);
        stateMachine.changeState("projection");
        for (int i = 0; i < 2; i++) {
            ddlGui[i]->setVisible(false);
            btnGui[i]->setVisible(false);
        }
        stateMachine.getSharedData().hideROIHandles();
        stateMachine.getSharedData().activeCameraNo = va_arg(args, int); // 0 or 1
        stateMachine.getSharedData().updateROI();
        stateMachine.getSharedData().updateActiveDstPoints();
    } else if (name == "help") {
        ofSetWindowShape(stateMachine.getSharedData().screenWidth, stateMachine.getSharedData().screenHeight);
        stateMachine.changeState("help");
        for (int i = 0; i < 2; i++) {
            ddlGui[i]->setVisible(false);
            btnGui[i]->setVisible(false);
        }
    }

    va_end(args);
}

void ofApp::saveUserSettings() {
    ofxXmlSettings settings;

    int screenWidth = stateMachine.getSharedData().screenWidth;
    int screenHeight = stateMachine.getSharedData().screenHeight;
    int captureWidth = stateMachine.getSharedData().captureWidth;
    int captureHeight = stateMachine.getSharedData().captureHeight;

    settings.addTag("userSettings");
    settings.pushTag("userSettings");
    settings.addTag("cameras");
    settings.pushTag("cameras");
    for (int i = 0; i < 2; i++) {
        settings.addTag("camera");
        settings.pushTag("camera", i);
        settings.addTag("clip");
        settings.pushTag("clip");
        ofRectangle roi = stateMachine.getSharedData().roiImage[i].getROI();
        settings.addValue("x", roi.x / captureWidth);
        settings.addValue("y", roi.y / captureHeight);
        settings.addValue("width", roi.width / captureWidth);
        settings.addValue("height", roi.height / captureHeight);
        settings.popTag(); // clip
        settings.addTag("projection");
        settings.pushTag("projection");

        for (int j = 0; j < 4; j++) {
            settings.addTag("point");
            settings.pushTag("point", j);
            settings.addValue("x", stateMachine.getSharedData().warperOut[i].dstPoints[j].x / screenWidth);
            settings.addValue("y", stateMachine.getSharedData().warperOut[i].dstPoints[j].y / screenHeight);
            settings.popTag();
        }
        settings.popTag(); // projection
        settings.popTag(); // camera
    }
    settings.popTag(); // cameras
    settings.popTag(); // userSettings
    settings.saveFile("user_settings.xml");

}

void ofApp::loadUserSettings() {
    ofxXmlSettings settings;

    int screenWidth = stateMachine.getSharedData().screenWidth;
    int screenHeight = stateMachine.getSharedData().screenHeight;
    int captureWidth = stateMachine.getSharedData().captureWidth;
    int captureHeight = stateMachine.getSharedData().captureHeight;

    if (settings.loadFile("user_settings.xml")) {
        settings.pushTag("userSettings");
        settings.pushTag("cameras");
        int nCameras = settings.getNumTags("camera");
        for (int i = 0; i < nCameras && i < 2; i++) {
            settings.pushTag("camera", i);
            settings.pushTag("clip");

            ofRectangle rect;
            rect.x = (float)settings.getValue("x", 0.0);
            rect.y = settings.getValue("y", 0.0);
            rect.width = settings.getValue("width", 0.0);
            rect.height = settings.getValue("height", 0.0);

            rect.x *= captureWidth;
            rect.y *= captureHeight;
            rect.width *= captureWidth;
            rect.height *= captureHeight;

            stateMachine.getSharedData().roiImage[i].setROI(rect);
            settings.popTag(); // clip

            settings.pushTag("projection");
            for (int j = 0; j < 4; j++) {
                settings.pushTag("point", j);

                ofPoint p;
                p.x = settings.getValue("x", 0.0) * screenWidth;
                p.y = settings.getValue("y", 0.0) * screenHeight;

                stateMachine.getSharedData().warperOut[i].dstPoints[j] = p;
                settings.popTag(); // point
            }
            settings.popTag(); // projection
            settings.popTag(); // camera
        }
        settings.popTag(); // cameras
        settings.popTag(); // userSettings
    } else {
        ofLogError("cannot load user_settings.xml");
    }
}

void ofApp::loadSystemSettings() {
    ofxXmlSettings settings;

    int screenWidth, screenHeight;
    int captureWidth, captureHeight;
    int previewWidth, previewHeight;

    if (settings.loadFile("system_settings.xml")) {
        settings.pushTag("systemSettings");

        screenWidth = settings.getValue("screenWidth", 1920);
        screenHeight = settings.getValue("screenHeight", 1080);
		captureWidth = settings.getValue("captureWidth", 1920);
		captureHeight = settings.getValue("captureHeight", 1080);
        previewWidth = screenWidth / 4;
        previewHeight = previewWidth * captureHeight / captureWidth;

        stateMachine.getSharedData().screenWidth = screenWidth;
        stateMachine.getSharedData().screenHeight = screenHeight;
        stateMachine.getSharedData().captureWidth = captureWidth;
        stateMachine.getSharedData().captureHeight = captureHeight;
        stateMachine.getSharedData().previewWidth = previewWidth;
        stateMachine.getSharedData().previewHeight = previewHeight;

        settings.popTag(); // systemSettings
    } else {
        ofLogError("cannot load system_settings.xml");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
