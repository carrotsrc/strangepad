#ifndef __RIGDESC_HPP_1437073162__
#define __RIGDESC_HPP_1437073162__
#include <QVector>
#include <QString>

typedef struct {
	QString type;
	QString label;
	QString unit;
} PadDesc;

typedef struct {
	QString label;
	QVector<PadDesc> pads;
} HudDesc;

class RigDesc {
public:
	void addHud(QString label);
	void addPad(QString type, QString label, QString unit); 

	QString getLabel();
	QVector<HudDesc>::const_iterator begin();
	QVector<HudDesc>::const_iterator end();
private:
	QVector<HudDesc> mHuds;
	HudDesc* mCurrentHud;
};
#endif

