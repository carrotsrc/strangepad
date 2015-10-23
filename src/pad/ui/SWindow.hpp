#ifndef __WINDOW_HPP_1436914978__
#define __WINDOW_HPP_1436914978__
#include <QWidget>
#include <QVector>
#include <QTabWidget>
#include <QGridLayout>
#include <QPushButton>
#include "SHud.hpp"
#include "../sys/RigDesc.hpp"
#include "framework/alias.hpp"
#include "framework/component/rack.hpp"
#include "framework/midi/midi_handler.hpp"

class SWindow : public QWidget
{
Q_OBJECT
public:
	explicit SWindow(QWidget *parent = 0);

	void addHeadsup(SHud *widget);
	void bindMidi(const QVector<MidiDesc> & bindings, siomid::midi_handler *handler);

signals:
	public slots:

private:
	QGridLayout mContainer;
	QTabWidget mHud;
	QPushButton mButton;

	void setupUi();
	void setupStyles();
};
  
#endif

