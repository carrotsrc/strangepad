#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QTimer>

#include "ui/SPad.hpp"
#include "ui/SVIndicator.hpp"
#include "ui/SKnob.hpp"

#include "SuMixer.h"

class SpMixerController : public SPad
{
Q_OBJECT
public:
	explicit SpMixerController(QWidget *parent = 0);

signals:
	public slots:
	void probeLevels();

private:
	QHBoxLayout mContainer;
	QVBoxLayout mGainBar;
	QTimer mProbeTrigger;

	SVIndicator mLevelsLeft, mLevelsRight;
	SKnob mGainLeft, mGainRight;

	void onRegisterUnit();
};

#endif

