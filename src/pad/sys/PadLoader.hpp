#ifndef __PADLOADER_HPP_1437985953__
#define __PADLOADER_HPP_1437985953__
#include <memory>

#include <QMap>
#include <QLibrary>

#include "../../panels/ui/SPad.hpp"

typedef QWidget*(*PadBuilder)(const QString &);

class PadLoader {
public:
	std::unique_ptr<SPad> load(const QString & collection, const QString & pad);
private:
	QMap<QString, PadBuilder> mRepo;

	PadBuilder loadCollection(const QString & collection);
};
#endif
