#ifndef __SPAD_HPP_1436984479__
#define __SPAD_HPP_1436984479__
#include <QWidget>

class SPad : public QWidget {
Q_OBJECT
public:
	explicit SPad(QWidget *parent = 0);
	void paintEvent(QPaintEvent *);
signals:
	public slots:
};
#endif

