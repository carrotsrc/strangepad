#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QMutex>

#include "ui/SPad.hpp"
#include "ui/SLevel.hpp"
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
	QGridLayout mLayout;
	QTimer mProbeTrigger;
	QMutex mMutex;


	SLevel mLevelsLeft, mLevelsRight;
	SKnob mGainLeft, mGainRight, mGainMaster;
	SSlider mFader;
	void onRegisterUnit();
	std::shared_ptr<SuMixerCbGainChange> cbGainChange;
	void onGainChange(SuMixer::GainType type, int value);
};

#endif

