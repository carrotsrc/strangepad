#include <openssl/sha.h>
#include <qmath.h>
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
	auto blockSize = (int)(mLenR/Waveform::StoreSize);
	long long sampleIndex = 0.0;
	mMin = new signed short[Waveform::StoreSize];

	for(auto x = 0; x < Waveform::StoreSize; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;
		signed short sample;

		for(int i = 0; i < blockSize; i++) {

			if(i%2) sampleIndex++;

			if((sample = mRaw[sampleIndex++]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}
		}

		auto pos = blockPs ? (short)qFloor(accPs/blockPs) : 0;
		auto neg = blockNg ? (short)qFloor(accNg/blockNg) : 0;
		mMin[x++] = pos; mMin[x] = neg;
	}
}
