#include <QFile>
#include "ConfigLoader.hpp"
#include <iostream>

using TokenType = QXmlStreamReader::TokenType;
ConfigLoader::ConfigLoader() {
	initStateTokens();
}

void ConfigLoader::initStateTokens() {
	mStateTokens.insert(State::Rig, QString("rig"));
	mStateTokens.insert(State::Hud, QString("hud"));
	mStateTokens.insert(State::Hud, QString("midi"));
}

bool ConfigLoader::load(const QString &path, RigDesc *description) {
	QFile file(path);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	mXml.addData(file.readAll());

	mRig = description;

	while(!mXml.atEnd()) {
		auto type = mXml.readNext();
		switchType(type);
	}
	//mXml.clear();
	file.close();
	return true;
}

void ConfigLoader::switchType(const TokenType & type) {
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

void ConfigLoader::switchStartElement(const QString & element) {

	if(element  == "hud") {
		readHudElement();
	} else if(element == "pad") {
		readPadElement();
	} else if(element == "rack") {
		readRackElement();
	} else if(element == "midi") {
		readMidiElement();
	}
	// push the state
	for(auto it = mStateTokens.constBegin(); it != mStateTokens.constEnd(); ++it) {
		if(it.value() == element) {
			mState.push_back(it.key());
			return;
		}
	}
}

void ConfigLoader::switchEndElement(const QString & element) {

	// pop the state
	if(mStateTokens[mState.last()] == element) {
		mState.pop_back();
	}
}

void ConfigLoader::readHudElement() {
	auto at = mXml.attributes();

	if(!at.hasAttribute("label")) {
		std::cerr << "Pad Config: hud element" 
			<< " has malformed description" << std::endl;
		return;
	}

	mRig->addHud(at.value("label").toString());
}

void ConfigLoader::readRackElement() {
	auto at = mXml.attributes();

	if(!at.hasAttribute("config")) {
		std::cerr << "Pad Config: Rack element" 
			<< " has malformed description" << std::endl;
		return;
	}

	mRig->setRackConfig(at.value("config").toString());
}

void ConfigLoader::readPadElement() {
	auto at = mXml.attributes();

	if(!at.hasAttribute("label")
	|| !at.hasAttribute("collection")
	|| !at.hasAttribute("type")
	|| !at.hasAttribute("unit")) {
		std::cerr << "Pad Config: pad element" 
			<< " has malformed description" << std::endl;
		return;
	}

	mRig->addPad(at.value("collection").toString(),
		at.value("type").toString(),
		at.value("label").toString(),
		at.value("unit").toString());
}

void ConfigLoader::readMidiElement() {
		auto at = mXml.attributes();

	if(!at.hasAttribute("device")
	|| !at.hasAttribute("code")
	|| !at.hasAttribute("function")) {
		std::cerr << "Pad Config: midi element" 
			<< " has malformed description" << std::endl;
		return;
	}

	mRig->addMidi(at.value("device").toString(),
				at.value("code").toString(),
				at.value("function").toString());
}
