#include <QMap>
#include <QTextStream>
#include <QMutexLocker>
#include <qt5/QtCore/qiodevice.h>
#include <qt5/QtCore/qfiledevice.h>
#include "tag_manager.hpp"

tag_manager::tag_manager()
	: m_hash(QString(""))
{
	m_fields.clear();
}

tag_manager::~tag_manager() {
}

void tag_manager::load(QString hash) {
	m_fields.clear();
	m_hash = hash;
	QFile f(".tags/"+hash+".spt");
	if(!f.exists()) {
		f.open(QIODevice::WriteOnly);
		f.close();
	} else {
		f.open(QIODevice::ReadOnly);
		parse(f);
		f.close();
	}
}

void tag_manager::save() {
	if(m_hash == "") return;
	QFile f(".tags/"+m_hash+".spt");

	if(f.open(QFile::WriteOnly|QFile::Text)) {
		QTextStream ts(&f);
		auto it = m_fields.begin();
		
		
		for(it = m_fields.begin(); it != m_fields.end(); it++) {
			ts << it.key() << "=" << it.value() << '\n';
		}
	}
	f.close();
}

void tag_manager::parse(QFile& f) {
	QTextStream in(&f);

	while(!in.atEnd()) {
		QString line = in.readLine();
		if(!line.contains("=")) continue;
		
		QStringList tag = line.split("=");    
		m_fields.insert(tag.at(0), tag.at(1));
	}
}

QString tag_manager::get(QString tag) {
	QMutexLocker locker(&m_mutex);
	auto key = m_fields.find(tag);
	
	if(key == m_fields.end()) return QString("");
	return key.value();
}

void tag_manager::set(QString tag, QString value) {
	QMutexLocker locker(&m_mutex);
	m_fields.insert(tag, value);
}

