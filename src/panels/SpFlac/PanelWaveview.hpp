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
#include <QTimer>

#include "ui/SPad.hpp"
#include "ui/SWaveform.hpp"
#include "ui/SButton.hpp"

#include "strange/SuFlac.hpp"
#include "tag_manager.hpp"

class SpFlacWaveview : public SPad
{
Q_OBJECT
public:
	explicit SpFlacWaveview(QWidget *parent = 0);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);

	void mouseReleaseEvent(QMouseEvent* e);

public slots:
	void triggerMidiPlay();
	void onGuiUpdate();
	void probeProgress();

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
	QTimer mProgressTrigger;
	
	int mSampleStep, m_bpm;
	std::atomic<int> mNextStep;
	tag_manager m_tags;

	suflac_onchange_sptr mOnChangePtr;
	
	QString mTrackPath, mTrackArtist, mTrackTitle, mTrackAlbum;

	void onRegisterUnit();
	void listenOnChange(SuFlac::working_state state);

	void organise_tags();
	
	// listeners
	void listener_onchange(SuFlac::working_state);
};

#endif

