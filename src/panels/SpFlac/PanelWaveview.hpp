#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "ui/SPad.hpp"
#include "ui/SWaveform.hpp"
#include "ui/SButton.hpp"

#define SUFLACLOAD_GUARDED 1
#include "SuFlacLoad.h"
#undef SUFLACLOAD_GUARDED

class SpFlacWaveview : public SPad
{
public:
	explicit SpFlacWaveview(QWidget *parent = 0);
signals:
	public slots:

private:
	QVBoxLayout mContainer;
	QHBoxLayout mToolbar, mButtonBar;
	SWaveform mWave;
	QFrame mTools;
	QLabel mTitle;
	SButton mPlay, mPause;


	std::shared_ptr<SuflCbStateChange> mfStateChangePtr;




	void onRegisterUnit();
	void onUnitStateChange(SuFlacLoad::WorkState state);
};

#endif

