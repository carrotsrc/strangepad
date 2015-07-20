#include <openssl/sha.h>
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
	auto blockSize = mLenR/Waveform::StoreSize;
}
