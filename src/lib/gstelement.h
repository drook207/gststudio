#pragma once

#include <QList>
#include <QString>

namespace GstStudio {
struct GstProperty {
	QString name;
	QString type;
	QString description;
	QString defaultValue;
	QString range;
	QStringList enumValues;
	bool writable = false;
	bool readable = false;
	bool controllable = false;
};

struct GstPadTemplate {
	QString name;
	QString direction; // "SRC" or "SINK"
	QString presence; // "ALWAYS", "SOMETIMES", "REQUEST"
	QString caps;
};

struct GstElement {
	QString name;
	QString longName;
	QString description;
	QString author;
	QString classification;
	QString rank;
	QList<GstProperty> properties;
	QList<GstPadTemplate> padTemplates;
};
}
