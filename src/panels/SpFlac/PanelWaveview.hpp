#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QGridLayout>
#include "ui/SPad.hpp"
#include "ui/SWaveform.hpp"
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


	std::shared_ptr<SuflCbStateChange> mfStateChangePtr;
	SWaveform mWave;


	void onRegisterUnit();
	void onUnitStateChange(SuFlacLoad::WorkState state);
};

#endif

