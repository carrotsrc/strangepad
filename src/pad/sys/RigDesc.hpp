#ifndef __RIGDESC_HPP_1437073162__
#define __RIGDESC_HPP_1437073162__
#include <QVector>
#include <QString>

typedef struct {
	QString collection;
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
	RigDesc();
	void addHud(QString label);
	void addPad(QString collection, QString type, QString label, QString unit); 

	QString getLabel();

	void setRackConfig(QString path);
	QString getRackConfig();

	QVector<HudDesc>::const_iterator begin() const;
	QVector<HudDesc>::const_iterator end() const;
private:
	QVector<HudDesc> mHuds;
	HudDesc* mCurrentHud;
	QString mRackConfig;
};
#endif

