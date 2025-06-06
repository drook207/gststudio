#pragma once

#include "gstelement.h"
#include <QList>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

namespace GstStudio {

class GstInspectParser : public QObject {
    Q_OBJECT

  public:
    explicit GstInspectParser(QObject* parent = nullptr);

    // Parse all available elements
    bool parseAllElements();

    // Parse specific element
    GstElement parseElement(const QString& elementName);

    // Get list of all element names
    QStringList getAllElementNames() const;

    // Get parsed element data
    GstElement getElement(const QString& name) const;

    // Get elements by category/classification
    QStringList getElementsByClassification(const QString& classification) const;

  signals:
    void parsingProgress(int current, int total);
    void parsingFinished();
    void elementParsed(const QString& elementName);

  private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

  private:
    QMap<QString, GstElement> m_elements;
    QProcess* m_process;
    QString m_currentOutput;

    // Parsing helper functions
    void parseElementList(const QString& output);
    GstElement parseElementDetails(const QString& output);
    void parseProperties(const QString& section, GstElement& element);
    void parsePadTemplates(const QString& section, GstElement& element);
    GstProperty parseProperty(const QString& propertyText);
    GstPadTemplate parsePadTemplate(const QString& padText);

    // Utility functions
    QString extractSection(const QString& text, const QString& sectionName);
    QStringList splitIntoSections(const QString& text, const QString& delimiter);
    QString cleanupText(const QString& text);
};

} // namespace GstStudio
