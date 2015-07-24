#ifndef __SPAD_HPP_1436984479__
#define __SPAD_HPP_1436984479__
#include <QWidget>
#include "framework/rack/RackUnit.h"

class SPad : public QWidget {
Q_OBJECT
public:
	explicit SPad(QWidget *parent = 0);
	void paintEvent(QPaintEvent *);

	void registerUnit(std::weak_ptr<RackoonIO::RackUnit> unit);
signals:
	public slots:

protected:
	std::weak_ptr<RackoonIO::RackUnit> mUnit;

	template<typename T>
	std::shared_ptr<T> unit() {
		return std::static_pointer_cast<T>(mUnit.lock());
	};

	void virtual onRegisterUnit() = 0;
};
#endif

