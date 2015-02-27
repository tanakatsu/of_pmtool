//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofMain.h"
#include "ofxImageROI.h"

ofxImageROI::ofxImageROI()
{
    anchorSize = 8;
    selectedCornerIndex = -1;
    highlightCornerIndex = -1;
    bShow = false;
    bMouseEnabled = false;
    bKeyboardShortcuts = false;
    mode = OFX_IMAGE_ROI_HANDLE_TWOPOINTS;
    bSetROI = false;
    offsetMat.newIdentityMatrix();
}

ofxImageROI::~ofxImageROI()
{
    disableMouseControls();
    disableKeyboardShortcuts();
}

void ofxImageROI::setMode(int mode)
{
    switch(mode) {
    case OFX_IMAGE_ROI_HANDLE_TWOPOINTS:
    case OFX_IMAGE_ROI_HANDLE_FOURPOINTS:
        this->mode = mode;
        break;
    default:
        ofLog(OF_LOG_ERROR, "invalid mode");
        break;
    }
}

void ofxImageROI::setROI()
{
    if (isAllocated()) {
        cornerPoints[0].set(0, 0);
        cornerPoints[1].set(this->width, 0);
        cornerPoints[2].set(this->width, this->height);
        cornerPoints[3].set(0, this->height);

        bSetROI = true;
    }
}

void ofxImageROI::setROI(ofPoint &p, float w, float h)
{
    cornerPoints[0].set(p.x, p.y);
    cornerPoints[1].set(p.x + w, p.y);
    cornerPoints[2].set(p.x + w, p.y + h);
    cornerPoints[3].set(p.x, p.y + h);
    bSetROI = true;
}

void ofxImageROI::setROI(ofRectangle rect)
{
    cornerPoints[0].set(rect.x, rect.y);
    cornerPoints[1].set(rect.x + rect.width, rect.y);
    cornerPoints[2].set(rect.x + rect.width, rect.y + rect.height);
    cornerPoints[3].set(rect.x, rect.y + rect.height);
    bSetROI = true;
}

void ofxImageROI::setAnchorSize(float size)
{
    if (size > 0) {
        anchorSize = size;
    } else {
        ofLog(OF_LOG_ERROR, "invalid anchor size");
    }
}

void ofxImageROI::showHandles()
{
    if (bShow) {
        return;
    }

    toggleHandles();
}

void ofxImageROI::hideHandles()
{
    if (!bShow) {
        return;
    }

    toggleHandles();
}

void ofxImageROI::toggleHandles()
{
    bShow = !bShow;

    if (bShow) {
        enableMouseControls();
//        enableKeyboardShortcuts();
    } else {
        disableMouseControls();
        disableKeyboardShortcuts();
    }
}

bool ofxImageROI::isShowingHandles()
{
    return bShow;
}

bool ofxImageROI::isMouseEnabled()
{
    return bMouseEnabled;
}

ofRectangle ofxImageROI::getROI()
{
    ofRectangle roi;
    float min_x, min_y, max_x, max_y;

    if (bSetROI) {
        min_x = cornerPoints[0].x < cornerPoints[3].x ? cornerPoints[0].x : cornerPoints[3].x;
        max_x = cornerPoints[1].x > cornerPoints[2].x ? cornerPoints[1].x : cornerPoints[2].x;
        min_y = cornerPoints[0].y < cornerPoints[1].y ? cornerPoints[0].y : cornerPoints[1].y;
        max_y = cornerPoints[2].y > cornerPoints[3].y ? cornerPoints[2].y : cornerPoints[3].y;
        roi.set(cornerPoints[0], max_x - min_x, max_y - min_y);
    }
    return roi;
}

void ofxImageROI::resetROI()
{
    bSetROI = false;
}

bool ofxImageROI::isSetROI()
{
    return bSetROI;
}

void ofxImageROI::draw(int x, int y, float w, float h)
{
    ofPushStyle();

    ofSetColor(ofColor::white);
    ofImage::draw(x, y, w, h);
    position.set(x, y);

    if (this->bAllocated()) {
        scale_w = w / this->width;
        scale_h = h / this->height;
    }

    // ROIがセットされてない場合は画像サイズでROIをセットする
    if (bSetROI == false) {
        setROI();
    }

    if (bShow) {
        ofSetColor(ofColor::yellow);
        ofPushMatrix();
        ofTranslate(x, y);

        // ofScaleでスケーリングするとマニピュレーターも小さくなってしまう
        for (int i = 0; i < 4; i++) {
            tmpCornerPoints[i].set(cornerPoints[i].x * scale_w, cornerPoints[i].y * scale_h);
        }

        for (int i = 0; i < 4; i++) {
            int j = (i + 1) % 4;
            ofLine(tmpCornerPoints[i], tmpCornerPoints[j]);
        }

        for (int i = 0; i < 4; i++) {
            if (i == selectedCornerIndex) {
                ofSetColor(ofColor::magenta);
            } else if (i == highlightCornerIndex) {
                ofSetColor(ofColor::orange);
            } else {
                ofSetColor(ofColor::yellow);
            }

            if (mode == OFX_IMAGE_ROI_HANDLE_TWOPOINTS) {
                if (i % 2 == 0) {
                    ofRect(tmpCornerPoints[i].x - anchorSize / 2, tmpCornerPoints[i].y - anchorSize / 2, anchorSize, anchorSize);
                }
            } else {
                ofRect(tmpCornerPoints[i].x - anchorSize / 2, tmpCornerPoints[i].y - anchorSize / 2, anchorSize, anchorSize);
            }
        }
        ofPopMatrix();
    }

    ofPopStyle();
}

void ofxImageROI::draw(int x, int y)
{
    draw(x, y, this->width, this->height);
}

void ofxImageROI::enableMouseControls()
{
    if(bMouseEnabled == true) {
        return;
    }
    bMouseEnabled = true;
    ofAddListener(ofEvents().mouseMoved, this, &ofxImageROI::onMouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &ofxImageROI::onMousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxImageROI::onMouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxImageROI::onMouseReleased);
}

void ofxImageROI::disableMouseControls()
{
    if(bMouseEnabled == false) {
        return;
    }
    bMouseEnabled = false;
    try {
        ofRemoveListener(ofEvents().mouseMoved, this, &ofxImageROI::onMouseMoved);
        ofRemoveListener(ofEvents().mousePressed, this, &ofxImageROI::onMousePressed);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxImageROI::onMouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxImageROI::onMouseReleased);
    }
    catch(Poco::SystemException) {
        return;
    }
}

void ofxImageROI::enableKeyboardShortcuts() {
    if(bKeyboardShortcuts == true) {
        return;
    }
    bKeyboardShortcuts = true;
    ofAddListener(ofEvents().keyPressed, this, &ofxImageROI::keyPressed);
}

void ofxImageROI::disableKeyboardShortcuts() {
    if(bKeyboardShortcuts == false) {
        return;
    }
    bKeyboardShortcuts = false;
    try {
        ofRemoveListener(ofEvents().keyPressed, this, &ofxImageROI::keyPressed);
    }
    catch(Poco::SystemException) {
        return;
    }
}

void ofxImageROI::onMouseMoved(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }

    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;

    ofVec3f offset = offsetMat.getTranslation() * ofVec3f(-1, -1, 0);
    mousePoint += ofVec2f(offset.x, offset.y);

    for(int i=0; i<4; i++) {
		if(mousePoint.distance(tmpCornerPoints[i]) <= anchorSize / 2) {
            highlightCornerIndex = i;
            return;
		}
	}
    highlightCornerIndex = -1;
}

void ofxImageROI::onMousePressed(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }

    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;

    ofVec3f offset = offsetMat.getTranslation() * ofVec3f(-1, -1, 0);
    mousePoint += ofVec2f(offset.x, offset.y);

	for(int i=0; i<4; i++) {
		if(mousePoint.distance(tmpCornerPoints[i]) <= anchorSize / 2) {
            if (mode == OFX_IMAGE_ROI_HANDLE_TWOPOINTS) {
                if (i % 2 == 0) {
                    selectedCornerIndex = i;
                    return;
                }
            } else {
                selectedCornerIndex = i;
                return;
            }
		}
	}
    selectedCornerIndex = -1;
}

void ofxImageROI::onMouseDragged(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }

    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }

    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;

    ofVec3f offset = offsetMat.getTranslation() * ofVec3f(-1, -1, 0);
    mousePoint += ofVec2f(offset.x, offset.y);

    cornerPoints[selectedCornerIndex].set(mousePoint);

    cornerPoints[selectedCornerIndex].x /= scale_w;
    cornerPoints[selectedCornerIndex].y /= scale_h;

    if (mode == OFX_IMAGE_ROI_HANDLE_TWOPOINTS && selectedCornerIndex % 2 == 0) {
        cornerPoints[3].set(cornerPoints[0].x, cornerPoints[2].y);
        cornerPoints[1].set(cornerPoints[2].x, cornerPoints[0].y);
    }
}

void ofxImageROI::onMouseReleased(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }

    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }

    for (int i = 0; i < 4; i ++) {
        cornerPoints[i].x = ofClamp(cornerPoints[i].x, 0, this->width);
        cornerPoints[i].y = ofClamp(cornerPoints[i].y, 0, this->height);
    }
}

void ofxImageROI::keyPressed(ofKeyEventArgs& keyArgs) {
    if(bShow == false) {
        return;
    }

    switch (keyArgs.key) {
        case '1':
            selectedCornerIndex = 0;
            break;
        case '2':
            if (mode == OFX_IMAGE_ROI_HANDLE_FOURPOINTS) {
                selectedCornerIndex = 1;
            }
            break;
        case '3':
            selectedCornerIndex = 2;
            break;
        case '4':
            if (mode == OFX_IMAGE_ROI_HANDLE_FOURPOINTS) {
                selectedCornerIndex = 3;
            }
            break;
        default:
            break;
    }

    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }

    float nudgeAmount = 0.3;
    ofPoint & selectedPoint = cornerPoints[selectedCornerIndex];

    switch (keyArgs.key) {
        case OF_KEY_LEFT:
            selectedPoint.x -= nudgeAmount;
            break;
        case OF_KEY_RIGHT:
            selectedPoint.x += nudgeAmount;
            break;
        case OF_KEY_UP:
            selectedPoint.y -= nudgeAmount;
            break;
        case OF_KEY_DOWN:
            selectedPoint.y += nudgeAmount;
            break;
        default:
            break;
    }

    selectedPoint.x = ofClamp(selectedPoint.x, 0, this->width);
    selectedPoint.y = ofClamp(selectedPoint.y, 0, this->height);

    if (mode == OFX_IMAGE_ROI_HANDLE_TWOPOINTS && selectedCornerIndex % 2 == 0) {
        cornerPoints[3].set(cornerPoints[0].x, cornerPoints[2].y);
        cornerPoints[1].set(cornerPoints[2].x, cornerPoints[0].y);
    }
}

void ofxImageROI::projectToTarget(ofPoint *dstPoints)
{
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];

	cvsrc[0].x = cornerPoints[0].x;
	cvsrc[0].y = cornerPoints[0].y;
	cvsrc[1].x = cornerPoints[1].x;
	cvsrc[1].y = cornerPoints[1].y;
	cvsrc[2].x = cornerPoints[2].x;
	cvsrc[2].y = cornerPoints[2].y;
	cvsrc[3].x = cornerPoints[3].x;
	cvsrc[3].y = cornerPoints[3].y;

	cvdst[0].x = dstPoints[0].x;
	cvdst[0].y = dstPoints[0].y;
	cvdst[1].x = dstPoints[1].x;
	cvdst[1].y = dstPoints[1].y;
	cvdst[2].x = dstPoints[2].x;
	cvdst[2].y = dstPoints[2].y;
	cvdst[3].x = dstPoints[3].x;
	cvdst[3].y = dstPoints[3].y;

	CvMat* src_mat = cvCreateMat(4, 2, CV_32FC1);
	CvMat* dst_mat = cvCreateMat(4, 2, CV_32FC1);

	cvSetData(src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData(dst_mat, cvdst, sizeof(CvPoint2D32f));

	CvMat * translate = cvCreateMat(3,3,CV_32FC1); // 領域確保
    cvFindHomography(src_mat, dst_mat, translate); // ホモグラフィ計算

    // 画像サイズが変わったとき
    if (this->bAllocated() == true && inputImg.bAllocated == false) {
        projectedImage.allocate(this->width, this->height, OF_IMAGE_COLOR);
        inputImg.allocate(this->width, this->height);
        outputImg.allocate(this->width, this->height);
    }

    inputImg.setFromPixels(this->getPixelsRef()); // 画像のロード
    cvWarpPerspective(inputImg.getCvImage(), outputImg.getCvImage(), translate, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(255)); // 透視投影変換

    //出力結果をofImageに変換
    projectedImage.setFromPixels(outputImg.getPixels(), outputImg.getWidth(), outputImg.getHeight(), OF_IMAGE_COLOR);

    cvReleaseMat(&translate);
    cvReleaseMat(&src_mat);
    cvReleaseMat(&dst_mat);
}

void ofxImageROI::projectToSourceRect()
{
    ofPoint dstPoints[4];

    dstPoints[0].set(0, 0);
    dstPoints[1].set(this->width, 0);
    dstPoints[2].set(this->width, this->height);
    dstPoints[3].set(0, this->height);

    projectToTarget(dstPoints);
}

void ofxImageROI::setOffsetMatrix(ofMatrix4x4 mat)
{
    this->offsetMat = mat;
}

ofImage ofxImageROI::getProjectedImage()
{
    return projectedImage;
}




