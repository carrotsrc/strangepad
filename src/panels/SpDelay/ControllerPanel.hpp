#ifndef CONTROLLERPANEL_HPP__
#define CONTROLLERPANEL_HPP__
 
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMutex>
#include <QTimer>

#include "ui/SPad.hpp"
#include "ui/SKnob.hpp"
#include "ui/SButton.hpp"

#include "strange/SuDelay.hpp"

class SpDelayController : public SPad
{
Q_OBJECT
public:
	explicit SpDelayController(QWidget *parent = 0);
	
public slots:
	void onGuiUpdate();
	
signals:
	void guiUpdate();

public:
	QSize sizeHint() const;

private:
	QVBoxLayout mBufferSizeBox, mDecayBox, mStateBox, mInputBox;
	QHBoxLayout mToolBar;

	SKnob mBufferSizeKnob, mDecayKnob, mInputKnob;
	QLabel mBufferSizeLabel, mDecayLabel, mInputLabel, mStateLabel;
	SButton mToggledButton;
	std::atomic<bool> mToggled;
	
	sudelay_onchange_sptr mOnChangePtr;
	sudelay_onvalue_sptr mOnValuePtr;

	void onRegisterUnit();
	void listenOnChange(SuDelay::working_state state);
	void listenOnValue(SuDelay::value_change type, int value);
};

#endif