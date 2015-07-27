#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>

#include "ui/SPad.hpp"
#include "ui/SVIndicator.hpp"
#include "ui/SKnob.hpp"

#include "SuMixer.h"

class SpMixerController : public SPad
{
public:
	explicit SpMixerController(QWidget *parent = 0);

signals:
	public slots:

private:
	QHBoxLayout mContainer;
	QVBoxLayout mGainBar;

	SVIndicator mLevelsLeft, mLevelsRight;
	SKnob mGainLeft, mGainRight;

	void onRegisterUnit();
};

#endif

