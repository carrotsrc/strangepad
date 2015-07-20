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
	auto blockSize = qFloor(((mLenR)/(Waveform::StoreSize/2)));
	unsigned long long sampleIndex = 0.0;
	mCompressed = new signed short[Waveform::StoreSize];

	for(auto x = 0; x < Waveform::StoreSize; x++) {

		int blockPs = 0, blockNg = 0;
		signed long long accPs = 0.0, accNg = 0.0;
		signed short sample;

		for(int i = 0; i < blockSize; i++) {
			if(i%2) { sampleIndex++; continue; }

			if(sampleIndex == mLenR) break;

			if((sample = mRaw[sampleIndex++]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}
		}
		mCompressed[x++] = blockPs ? (short)qFloor(accPs/blockPs) : 0;
		mCompressed[x] = blockNg ? (short)qFloor(accNg/blockNg) : 0;
		//std::cout << x << ":" << mCompressed[x-1] << "\t" << mCompressed[x] << std::endl;
	}

}

QImage Waveform::generate(int width, int height) {

	auto dataLength = width*2;
	auto blockSize = qFloor(Waveform::StoreSize/width);
	std::cout << "Blocksize: " << blockSize << std::endl;
	unsigned long long sampleIndex = 0.0;
	auto form = new signed short[dataLength];
	auto mid = (int) height/2;

	auto scalePs = mid/32768.0;
	auto scaleNg = mid/32767.0;

	for(auto x = 0; x < dataLength; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;
		signed short sample;

		for(int i = 0; i < blockSize; i++) {
			if((sample = mCompressed[sampleIndex++]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}
		}

		form[x++] = blockPs ? (short)qFloor((accPs/blockPs)*scalePs) : 0;
		form[x] = blockNg ? (short)qFloor((accNg/blockNg)*-scaleNg) : 0;
	}

	std::cout << "sample: " << sampleIndex << "\t" << Waveform::StoreSize << std::endl;
	QPixmap pixmap(width, height);
	QPainter painter(&pixmap);
	painter.setRenderHints(QPainter::Antialiasing);
	QPen pen(QColor("#ffffff"));
	pen.setWidth(1);
	painter.setPen(pen);
	int sindex = 0;
	for(int x = 0; x < width; x++) {
		auto pos = form[sindex++];
		auto neg = form[sindex++];
		painter.drawLine(x,mid, x,mid-pos);
		painter.drawLine(x,mid, x,mid+neg);
	}
	std::cout << "s: " << sindex << "\t" << dataLength << std::endl;

	return pixmap.toImage();
}
