#ifndef TAG_MANAGER_HPP
#define	TAG_MANAGER_HPP
#include <QString>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <iostream>
#include <fstream>
class tag_manager {
public:
	tag_manager();
	
	void load(QString hash);
	void save();
	
	void set(QString tag, QString value);
	QString get(QString tag);
	
	virtual ~tag_manager();
private:
	QMutex m_mutex;
	QString m_hash;
	QMap<QString, QString> m_fields;
	
	void parse(QFile& f);
};

#endif	/* TAG_MANAGER_HPP */

