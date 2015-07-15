#ifndef __PANELOVERVIEW_HPP_1436981032__
#define __PANELOVERVIEW_HPP_1436981032__
#include <QWidget>
#include <QGridLayout>
#include "ui/SPad.hpp"

class SpSineOverview : public SPad
{
public:
	explicit SpSineOverview(QWidget *parent = 0);
signals:
	public slots:

private:
	QGridLayout mContainer;
};
#endif

