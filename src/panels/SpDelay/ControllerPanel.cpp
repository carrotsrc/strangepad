#include "ControllerPanel.hpp"

SpDelayController::SpDelayController(QWidget* parent)
	: SPad(parent)
	, mToggled(false)
{
	mOnChangePtr.reset(new sudelay_onchange_cb(
		std::bind(&SpDelayController::listenOnChange, this, std::placeholders::_1)
	));

	mOnValuePtr.reset(new sudelay_onvalue_cb(
		std::bind(&SpDelayController::listenOnValue, this, std::placeholders::_1, std::placeholders::_2)
	));

	mDecayLabel.setText("Decay");
	mDecayLabel.setAlignment(Qt::AlignCenter);

	mBufferSizeLabel.setText("Buffer Size");
	mBufferSizeLabel.setAlignment(Qt::AlignCenter);

	mInputLabel.setText("Input");
	mInputLabel.setAlignment(Qt::AlignCenter);


	mToggledButton.setText(QChar(0x25CF));
	mToggledButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mStateLabel.setText("Passing");
	mStateLabel.setAlignment(Qt::AlignCenter);

	mBufferSizeBox.addWidget(&mBufferSizeKnob);
	mBufferSizeBox.addWidget(&mBufferSizeLabel);

	mInputBox.addWidget(&mInputKnob);
	mInputBox.addWidget(&mInputLabel);

	mDecayBox.addWidget(&mDecayKnob);
	mDecayBox.addWidget(&mDecayLabel);

	mStateBox.addWidget(&mToggledButton, 0, Qt::AlignHCenter);
	mStateBox.addWidget(&mStateLabel);

	mToolBar.addLayout(&mBufferSizeBox);
	mToolBar.addLayout(&mInputBox);
	mToolBar.addLayout(&mDecayBox);
	mToolBar.addLayout(&mStateBox);

	setLayout(&mToolBar);

}

void SpDelayController::onRegisterUnit() {
	connect(this, SIGNAL(guiUpdate()), this, SLOT(onGuiUpdate()));
	if(auto u = unit<SuDelay>()) {
		u->listen_onchange(mOnChangePtr);
		u->listen_onvalue(mOnValuePtr);
		
		auto ms = u->probe_buffer_time();
		auto bpm = 60000/ms;
		QString msg(QString::number(ms) + QString("ms\n") + QString::number(bpm)+QString("bpm"));
		mBufferSizeLabel.setText(msg);
	}
}

void SpDelayController::listenOnChange(SuDelay::working_state state) {

	switch(state) {
	case SuDelay::working_state::priming:
		mStateLabel.setText("Priming");
		break;

	case SuDelay::working_state::ready:
		mStateLabel.setText("Ready");
		mToggled = false;
		emit guiUpdate();
		break;

	case SuDelay::working_state::filtering:
		mStateLabel.setText("Filtering");
		mToggled = true;
		emit guiUpdate();
		break;

	case SuDelay::working_state::passing:
		mStateLabel.setText("Passing");
		mToggled = false;
		emit guiUpdate();
		break;

	case SuDelay::working_state::resetting:
		mStateLabel.setText("Resetting");
		break;
	}
	emit update();
}


void SpDelayController::listenOnValue(SuDelay::value_change type, int value) {

	switch(type) {
	case SuDelay::value_change::buffer:
		mBufferSizeKnob.setValue(value);

		if(auto u = unit<SuDelay>()) {
			auto ms = u->probe_buffer_time();
			auto bpm = 60000/ms;
			QString msg(QString::number(ms) + QString("ms\n") + QString::number(bpm)+QString("bpm"));
			mBufferSizeLabel.setText(msg);
		}

		break;

	case SuDelay::value_change::input:
		mInputKnob.setValue(value);
		break;

	case SuDelay::value_change::decay:
		mDecayKnob.setValue(value);
		break;
	}

	emit update();
}

void SpDelayController::onGuiUpdate() {
	if(mToggled) {
		mToggledButton.setStyleSheet("color: #F97FFF;");
	} else {
		mToggledButton.setStyleSheet("color: #8E06A0;");
	}
}

QSize SpDelayController::sizeHint() const {
	return QSize(width(), 200);
}

