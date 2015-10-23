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

typedef struct {
	QString device, code, function;
} MidiDesc;

class RigDesc {
public:
	RigDesc();
	void addHud(QString label);
	void addPad(QString collection, QString type, QString label, QString unit);
	void addMidi(QString device, QString code, QString function);

	QString getLabel();

	void setRackConfig(QString path);
	QString getRackConfig();

	const QVector<MidiDesc>& getMidi() const;

	QVector<HudDesc>::const_iterator begin() const;
	QVector<HudDesc>::const_iterator end() const;
private:
	QVector<HudDesc> mHuds;
	QVector<MidiDesc> mMidi;
	HudDesc* mCurrentHud;
	QString mRackConfig;
};
#endif

