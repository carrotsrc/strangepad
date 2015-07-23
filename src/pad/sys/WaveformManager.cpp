#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>

#include <openssl/sha.h>
#include <qmath.h>
#include <QPainter>
#include <QPixmap>
#include <QIODevice>
#include <QFile>

#include "WaveformManager.hpp"

QString WaveformManager::hash(const pcm_sample *raw, unsigned long long spc) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX c;

	SHA256_Init(&c);
	SHA256_Update(&c, raw, spc/2);
	SHA256_Final(hash, &c);

	std::stringstream ss;
	for(auto i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}

	return QString(ss.str().c_str());
}

pcm_sample *WaveformManager::storeCompress(const pcm_sample *raw, unsigned long long spc, unsigned int *blockSize) {
	auto waveUnits = WaveformManager::MaxSize/2;
	*blockSize = qFloor(spc / waveUnits);
	auto sampleIndex = 0ull;
	pcm_sample sample;

	pcm_sample *compressed = new pcm_sample[WaveformManager::MaxSize];

	for(int block = 0; block < WaveformManager::MaxSize;) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;
		for(auto i = 0u; i < *blockSize; i++) {
			if((sample = raw[sampleIndex]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}

			sampleIndex += 2;
		}

		compressed[block++] = blockPs ? qFloor((accPs/blockPs)) : 0;
		compressed[block++] = blockNg ? qFloor((accNg/blockNg)) : 0;
	}

	return compressed;

}

QPixmap WaveformManager::compress(int width, int height, const pcm_sample *compressed, unsigned int *blockSize) {

	QPixmap graph(width, height);
	graph.fill(Qt::transparent);

	QPen pen(QColor("#ffffff"));
	pen.setWidth(1);

	QPainter painter(&graph);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(pen);

	auto mid = height/2;
	auto pScale = height/32768.0f;
	auto nScale = height/32767.0f;

	*blockSize = (unsigned int) qFloor((WaveformManager::MaxSize)/width);
	auto sampleIndex = 0ull;
	pcm_sample sample;

	for(int x = 0; x < width; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;

		for(auto i = 0u; i < *blockSize; i++) {

			if((sample = compressed[sampleIndex]) > 0) {
				accPs += sample;
				blockPs++;
			} else if(sample < 0) {
				accNg += sample;
				blockNg++;
			}

			sampleIndex++;
		}
		auto yp = blockPs ? (qFloor((accPs/blockPs) * pScale)) : 0;
		auto yn = blockNg ? (qFloor((accNg/blockNg) * nScale)) : 0;
		painter.drawLine(x,mid-yp, x,mid-yn);

	}

	return graph;
}

std::unique_ptr<Waveform> WaveformManager::generate(int width, int height, const pcm_sample *raw, unsigned long long spc) {
	if( (width|height) == 0) return nullptr;
	auto hashValue = hash(raw, spc);
	unsigned int blockSize;

	WaveStore wfs;
	Waveform *wf;

	QFile store(".store/"+hashValue+".wfs");
	if(!store.exists()) {
		auto compressed = storeCompress(raw, spc, &blockSize); 
		wfs.blockSize = blockSize,
		std::memcpy(&wfs.waveform, compressed, WaveformManager::MaxSize*2);
		store.open(QIODevice::WriteOnly);
		store.write((char*)&wfs, sizeof(WaveStore));
		store.close();
	} else {
		store.open(QIODevice::ReadOnly);
		store.read((char*)&wfs, sizeof(WaveStore));
		store.close();
	}
	auto graph = compress(width, height, wfs.waveform, &blockSize);
	blockSize = wfs.blockSize * blockSize;

	std::cout << blockSize << "\t" << wfs.blockSize << std::endl;
	wf = new Waveform(blockSize, graph, hashValue);

	return std::unique_ptr<Waveform>(wf);
}
