#ifndef __RIGDESC_HPP_1437073162__
#define __RIGDESC_HPP_1437073162__
#include <QMap>
#include <QString>

typedef struct {
	QString label;
} HudDesc;

class RigDesc {
public:
	void addHud(QString label);
private:
	QMap<QString, HudDesc*> huds;
};
#endif

