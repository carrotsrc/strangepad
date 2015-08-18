#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QMutex>

#include "ui/SPad.hpp"
#include "ui/SVIndicator.hpp"
#include "ui/SKnob.hpp"
#include "ui/SSlider.hpp"

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
	QHBoxLayout mContainer, mGainBar;
	QVBoxLayout mSplit, mVgl, mVgr, mVgm;
	QTimer mProbeTrigger;
	QMutex mMutex;


	SVIndicator mLevelsLeft, mLevelsRight;
	SKnob mGainLeft, mGainRight, mGainMaster;
	SSlider mFader;
	void onRegisterUnit();
};

#endif

