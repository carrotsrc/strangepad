#include "Waveform.hpp"

Waveform::Waveform(unsigned int blockSize, QPixmap graph, QString hash) {
	mBlockSize=blockSize; mGraph=graph; mHash = hash;
}

QString Waveform::hash() {
	return mHash;
}

QPixmap Waveform::waveform() {
	return mGraph;
}

unsigned int Waveform::blocksize() {
	return mBlockSize;
}
