#ifndef __PANELWAVEVIEW_HPP_1437740056__
#define __PANELWAVEVIEW_HPP_1437740056__
 
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QPushButton>
#include <QMutex>

#include "ui/SPad.hpp"
#include "ui/SWaveform.hpp"
#include "ui/SButton.hpp"

#define SUFLACLOAD_GUARDED 1
#include "SuFlacLoad.h"
#undef SUFLACLOAD_GUARDED

class SpFlacWaveview : public SPad
{
Q_OBJECT
public:
	explicit SpFlacWaveview(QWidget *parent = 0);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);

	public slots:
	void triggerMidiPlay();
	void onGuiUpdate();

	signals:
	void guiUpdate();


private:
	QVBoxLayout mContainer;
	QHBoxLayout mToolbar, mButtonBar;
	SWaveform mWave;
	QFrame mTools;
	QLabel mTitle;
	SButton mPlay, mPause;
	QMutex mMut;
	std::atomic<bool> mPlaying;


	std::shared_ptr<SuflCbStateChange> mfStateChangePtr;
	void onRegisterUnit();
	void onUnitStateChange(SuFlacLoad::WorkState state);
};

#endif

