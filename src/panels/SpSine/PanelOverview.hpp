#ifndef __PANELOVERVIEW_HPP_1436981032__
#define __PANELOVERVIEW_HPP_1436981032__
#include <QWidget>
#include <QGridLayout>
class SpSineOverview : public QWidget
{
public:
	explicit SpSineOverview(QWidget *parent = 0);
signals:
	public slots:

private:
	QGridLayout mContainer;
};
#endif

