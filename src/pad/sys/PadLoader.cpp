#include <QFile>
#include "PadLoader.hpp"
#include <iostream>

using TokenType = QXmlStreamReader::TokenType;
PadLoader::PadLoader() {
	initStateTokens();
}

void PadLoader::initStateTokens() {
	mStateTokens.insert(State::Rig, QString("rig"));
	mStateTokens.insert(State::Hud, QString("hud"));
	mStateTokens.insert(State::Pad, QString("pad"));
}

bool PadLoader::loadConfig(const QString &path) {
	QFile file(path);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	mXml.addData(file.readAll());

	while(!mXml.atEnd()) {
		auto type = mXml.readNext();
		switchType(type);
	}
	mXml.clear();
	file.close();
	return true;
}

void PadLoader::switchType(const TokenType & type) {
	switch(type) {
	case TokenType::StartElement:
		switchStartElement(mXml.name().toString());
		break;
	case TokenType::EndElement:
		switchEndElement(mXml.name().toString());
		break;
	default:
		break;
	}
}

void PadLoader::switchStartElement(const QString & element) {

	if(element  == "hud") {
		readHudElement();
	} else if(element == "") {

	}

	// push the state
	for(auto it = mStateTokens.constBegin(); it != mStateTokens.constEnd(); ++it) {
		if(it.value() == element) {
			mState.push_back(it.key());
			return;
		}
	}
}

void PadLoader::switchEndElement(const QString & element) {

	// pop the state
	if(mStateTokens[mState.last()] == element) {
		mState.pop_back();
		std::cout << "Popped " << qPrintable(element) << std::endl;
	} else {
		std::cerr << "Pad Config: Malformed XML description" << std::endl;
	}
}

void PadLoader::readHudElement() {
	auto at = mXml.attributes();
	if(at.hasAttribute("label")) {
		std::cout << qPrintable(at.value("label").toString()) << std::endl;
	}
}
