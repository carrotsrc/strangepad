#ifndef __HUD_HPP_1436948779__
#define __HUD_HPP_1436948779__
#include <QWidget>
#include <QGridLayout>

class SHud : public QWidget
{
public:
	explicit SHud(const QString & label, QWidget *parent = 0);
	void addWidget(QWidget *widget);
	const QString getLabel();

signals:
	public slots:

private:
	QGridLayout mContainer;
	QString mLabel;
};
#endif

