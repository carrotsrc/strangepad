#include <openssl/sha.h>
#include <qmath.h>
#include <QPainter>
#include <QPixmap>
#include "Waveform.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

void Waveform::setRaw(signed short *raw, long long samples) {
	mRaw = raw;
	mLenR = samples;
}

QString Waveform::hash() {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX c;

	SHA256_Init(&c);
	SHA256_Update(&c, mRaw, mLenR/2);
	SHA256_Final(hash, &c);

	std::stringstream ss;
	for(auto i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}

	return QString(ss.str().c_str());
}

void Waveform::minCompression() {
	auto waveUnits = Waveform::MaxSize/2;
	auto blockSize = mLenR / waveUnits;
	long long sampleIndex = 0;
	signed short sample;

	mCompressed = new short[Waveform::MaxSize];

	for(int block = 0; block < Waveform::MaxSize;) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;
		for(int i = 0; i < blockSize; i++) {
			if((sample = mRaw[sampleIndex]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}

			sampleIndex += 2;
		}

		mCompressed[block++] = blockPs ? qFloor((accPs/blockPs)) : 0;
		mCompressed[block++] = blockNg ? qFloor((accNg/blockNg)) : 0;
	}

}

QImage Waveform::generate(int width, int height) {

	QPixmap graph(width, height);
	QPainter painter(&graph);
	painter.setRenderHints(QPainter::Antialiasing);
	QPen pen(QColor("#ffffff"));
	pen.setWidth(1);
	painter.setPen(pen);

	auto mid = (int) height/2;
	auto pScale = height/32768.0;
	auto nScale = height/32767.0;

	auto blockSize = qFloor(Waveform::MaxSize/width);
	long long sampleIndex = 0;
	signed short sample;

	for(int x = 0; x < width; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;

		for(int i = 0; i < blockSize; i++) {

			if((sample = mCompressed[sampleIndex]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}

			sampleIndex++;
		}

		auto yp = blockPs ? (qFloor((accPs/blockPs) * pScale)) : 0;
		auto yn = blockNg ? (qFloor((accNg/blockNg) * nScale)) : 0;
		painter.drawLine(x,mid-yp, x,mid-yn);

	}

	return graph.toImage();
}
