#ifndef __PADLOADER_HPP_1437057484__
#define __PADLOADER_HPP_1437057484__
#include <QXmlStreamReader>
#include <QVector>
#include <QMap>

class PadLoader {

	enum State {
		Doc, Rig, Hud, Pad
	};

	QVector<State> mState;
	QMap<State, QString> mStateTokens;

public:
	PadLoader();
	bool loadConfig(const QString & path);
private:
	QXmlStreamReader mXml;
	
	void initStateTokens();
	void switchType(const QXmlStreamReader::TokenType & type);
	void switchStartElement(const QString & element);
	void switchEndElement(const QString & element);
};
#endif

