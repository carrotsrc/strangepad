#ifndef __RIGDESC_HPP_1437073162__
#define __RIGDESC_HPP_1437073162__
#include <QVector>
#include <QString>

typedef struct {
	QString label;
} HudDesc;

class RigDesc {
public:
	void addHud(QString label);
private:
	QVector<HudDesc*> mHuds;
	HudDesc* mCurrentHud;
};
#endif

