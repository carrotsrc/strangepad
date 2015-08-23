#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include "SWaveform.hpp"
#include <iostream>
SWaveform::SWaveform(QWidget* parent)
: QWidget(parent) {
	mWaveData = nullptr;
	mWaveform = nullptr;
	mHoverPosition = -1;
	setMouseTracking(true);
	mReset = mBgHighlight = isLoaded = false;
	mCurrentStep = mProgress = 0;
}

void SWaveform::paintEvent(QPaintEvent*) {

	QPainter painter(this);
	QPen pen;

	painter.setPen(Qt::NoPen);
	painter.setRenderHints(QPainter::Antialiasing);

	if(mBgHighlight) {
		painter.setBrush(QColor("#656565"));
		if(!isLoaded) mWaveRect.setRect(0,0,width(),height());
		painter.drawRoundedRect(mWaveRect, 10, 10);
	}


	if(!isLoaded) return;
	if(mReset) generateWaveform();





	painter.setBrush(mWaveform->waveform());
	painter.drawRect(mWaveRect);

	if(mProgress == 0) return;

	auto prog = mProgress/mSampleStep;
	if(prog < mCurrentStep+1)
		return;

	mCurrentStep++;
	QRectF rect(0,0,prog, mWaveRect.size().height());
	auto wf = mWaveform->waveform().copy(0,0,prog,mWaveRect.size().height());
	painter.setBrush(QBrush("#FF0000",wf));
	painter.drawRect(rect);
/*
	if(mHoverPosition >= 0) {
		pen.setColor("#8E06A0");
		painter.setPen(pen);
		painter.drawLine(mHoverPosition,0, mHoverPosition, mWaveRect.height());
	}
*/
}

void SWaveform::setWaveData(const float* data, long long length) {
	mWaveData = (float*)data;
	mWaveLength = length;
	isLoaded = true;
	mReset = true;
	emit update();
}

void SWaveform::toggleBgHighlight(bool flag) {
	mBgHighlight = flag;
}

void SWaveform::mouseMoveEvent(QMouseEvent* event) {
	if(rect().contains(event->pos())) {
		mHoverPosition = event->x();
	} else {
		mHoverPosition = -1;
	}
	update();

}

void SWaveform::generateWaveform() {
	mWaveRect.setSize(QSize(width(), height()));
	WaveformManager wfm;
	auto geo = mWaveRect.size();
	mWaveform = wfm.generate(geo.width(), geo.height(), mWaveData, mWaveLength);
	mSampleStep = mWaveform->blocksize();
	mReset = false;
}

QSize SWaveform::sizeHint() const {
	return QSize(width(), height());
}

QSize SWaveform::minimumSize() const {
	return QSize(width(), 50);
}

int SWaveform::getSampleStep() {
	return mSampleStep;
}

void SWaveform::updateProgress(int progress) {

	// This needs to keep track of it's own progress
	// otherwise all waveforms update exactly the same
	mProgress = progress;
}
