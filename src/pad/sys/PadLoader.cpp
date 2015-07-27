#include "PadLoader.hpp"


std::unique_ptr<SPad> PadLoader::load(const QString & collection, const QString & pad) {
	PadBuilder func;

	auto it = mRepo.constFind(collection);
	if(it != mRepo.constEnd()) {
		func = it.value();
	} else {
		try {
			func = loadCollection(collection);
		} catch(const char*) {
		}
	}

	if(func == nullptr) {
		throw "Collection `" + collection.toStdString() + "`: builder symbol is unresolved";
	}

	auto obj = (SPad*) func(pad);
	if(!obj) {
		throw "Collection `" + collection.toStdString() + "`:  pad `" + pad.toStdString() + "` not found";
	}
	return std::unique_ptr<SPad>(obj);
}

PadBuilder PadLoader::loadCollection(const QString & collection) {
	auto libPath = QString("./pads/lib"+QString(collection));
	auto builder = collection + QString("Build");
	QLibrary lib(libPath);
	if(!lib.load()) {
		throw "Collection: `" + collection.toStdString() + "` not found";
	}

	auto symbol = (PadBuilder) lib.resolve(builder.toStdString().c_str());
	if(!symbol) {
		symbol = nullptr;
	}
	
	mRepo.insert(collection, symbol);
	
	return symbol;
}
