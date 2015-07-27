#ifndef __WINDOW_HPP_1436914978__
#define __WINDOW_HPP_1436914978__
#include <QWidget>
#include <QTabWidget>
#include <QGridLayout>
#include <QPushButton>
#include "SHud.hpp"

class SWindow : public QWidget
{
Q_OBJECT
public:
	explicit SWindow(QWidget *parent = 0);

	void addHeadsup(SHud *widget);

signals:
	public slots:

private:
	QGridLayout mContainer;
	QTabWidget mHud;
	QPushButton mButton;

	void setupUi();
	void setupStyles();
};
  
#endif

