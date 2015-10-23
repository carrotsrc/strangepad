#ifndef __WAVEFORM_HPP_1437591484__
#define __WAVEFORM_HPP_1437591484__

#include <QPixmap>
#include <QString>


class Waveform {
public:
	Waveform(unsigned int blockSize, QPixmap graph, QString hash);
	QString hash();
	QPixmap waveform();
	unsigned int blocksize();

private:
	unsigned int mBlockSize;
	QPixmap mGraph;
	QString mHash;

};
#endif
