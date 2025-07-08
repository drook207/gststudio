/**
 * @file gstinspectparser.h
 * @brief Parser for GStreamer element inspection data
 * @author GstStudio Team
 */

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

/**
 * @class GstInspectParser
 * @brief Parser for GStreamer element inspection data
 *
 * This class provides functionality to parse GStreamer element information
 * using the gst-inspect-1.0 command line tool. It can parse all available
 * elements or specific elements and extract their properties, pad templates,
 * and other metadata.
 */
class GstInspectParser : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Constructs a new GstInspectParser
     * @param parent Parent QObject
     */
    explicit GstInspectParser(QObject* parent = nullptr);

    /**
     * @brief Parse all available GStreamer elements
     * @return true if parsing was successful, false otherwise
     */
    static bool parseAllElements();

    /**
     * @brief Parse a specific GStreamer element
     * @param elementName Name of the element to parse
     * @return GstElement structure containing element information
     */
    static GstElement parseElement(const QString& elementName);

    /**
     * @brief Get list of all available element names
     * @return QStringList containing all element names
     */
    [[nodiscard]] QStringList getAllElementNames() const;

    /**
     * @brief Get parsed element data by name
     * @param name Element name
     * @return GstElement structure for the requested element
     */
    [[nodiscard]] GstElement getElement(const QString& name) const;

    /**
     * @brief Get elements filtered by classification
     * @param classification Classification string (e.g., "Source/Video")
     * @return QStringList containing matching element names
     */
    [[nodiscard]] QStringList getElementsByClassification(const QString& classification) const;

  signals:
    /**
     * @brief Emitted during parsing to indicate progress
     * @param current Current number of elements parsed
     * @param total Total number of elements to parse
     */
    void parsingProgress(int current, int total);

    /**
     * @brief Emitted when parsing is complete
     */
    void parsingFinished();

    /**
     * @brief Emitted when an individual element has been parsed
     * @param elementName Name of the element that was parsed
     */
    void elementParsed(const QString& elementName);

  private slots:
    /**
     * @brief Called when the gst-inspect process finishes
     * @param exitCode Process exit code
     * @param exitStatus Process exit status
     */
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

  private:
    QMap<QString, GstElement> m_elements; ///< Cache of parsed elements
    QProcess* m_process;                  ///< Process for running gst-inspect
    QString m_currentOutput;              ///< Current output from gst-inspect

    /**
     * @brief Parse the list of all available elements
     * @param output Raw output from gst-inspect-1.0
     */
    void parseElementList(const QString& output);

    /**
     * @brief Parse detailed information for a specific element
     * @param output Raw output from gst-inspect-1.0 for a specific element
     * @return GstElement structure with parsed information
     */
    static GstElement parseElementDetails(const QString& output);

    /**
     * @brief Parse properties section of element output
     * @param section Properties section text
     * @param element Element to populate with properties
     */
    static void parseProperties(const QString& section, GstElement& element);

    /**
     * @brief Parse pad templates section of element output
     * @param section Pad templates section text
     * @param element Element to populate with pad templates
     */
    static void parsePadTemplates(const QString& section, GstElement& element);

    /**
     * @brief Parse a single property from text
     * @param propertyText Raw property text
     * @return GstProperty structure
     */
    static GstProperty parseProperty(const QString& propertyText);

    /**
     * @brief Parse a single pad template from text
     * @param padText Raw pad template text
     * @return GstPadTemplate structure
     */
    static GstPadTemplate parsePadTemplate(const QString& padText);

    /**
     * @brief Process pad template header line
     * @param line Input line to process
     * @param currentPad Current pad template being built
     * @param element Element to add completed pad template to
     * @param inCapabilities Reference to capabilities parsing state
     * @return true if line was processed as header, false otherwise
     */
    static bool processPadTemplateHeader(const QString& line, GstPadTemplate& currentPad, GstElement& element,
                                         bool& inCapabilities);

    /**
     * @brief Process availability information line
     * @param trimmedLine Trimmed input line
     * @param pad Pad template to update
     * @return true if line was processed, false otherwise
     */
    static bool processAvailabilityLine(const QString& trimmedLine, GstPadTemplate& pad);

    /**
     * @brief Process capabilities section
     * @param trimmedLine Trimmed input line
     * @param currentPad Current pad template being built
     * @param inCapabilities Reference to capabilities parsing state
     * @return true if line was processed, false otherwise
     */
    static bool processCapabilitiesSection(const QString& trimmedLine, GstPadTemplate& currentPad,
                                           bool& inCapabilities);

    /**
     * @brief Check if line contains capability information
     * @param line Line to check
     * @return true if line contains capability data
     */
    static bool isCapabilityLine(const QString& line);

    /**
     * @brief Check if line indicates end of current section
     * @param line Line to check
     * @return true if line indicates section end
     */
    static bool isSectionEnd(const QString& line);

    /**
     * @brief Finalize and add pad template to element
     * @param pad Pad template to finalize
     * @param element Element to add pad template to
     */
    static void finalizePadTemplate(GstPadTemplate& pad, GstElement& element);

    /**
     * @brief Parse pad name and direction from line
     * @param line Line to parse
     * @param pad Pad template to update
     */
    static void parsePadNameAndDirection(const QString& line, GstPadTemplate& pad);

    /**
     * @brief Process capability line for pad template
     * @param line Line to process
     * @param pad Pad template to update
     */
    static void processCapabilityLine(const QString& line, GstPadTemplate& pad);

    /**
     * @brief Extract a section from gst-inspect output
     * @param text Full text to search
     * @param sectionName Name of section to extract
     * @return Extracted section text
     */
    static QString extractSection(const QString& text, const QString& sectionName);

    /**
     * @brief Split text into sections using delimiter
     * @param text Text to split
     * @param delimiter Delimiter to use
     * @return QStringList of sections
     */
    static QStringList splitIntoSections(const QString& text, const QString& delimiter);

    /**
     * @brief Clean up text by removing extra whitespace
     * @param text Text to clean
     * @return Cleaned text
     */
    static QString cleanupText(const QString& text);

    /**
     * @brief Utility function for section extraction
     * @return Empty string
     */
    static QString extracted();
};

} // namespace GstStudio
