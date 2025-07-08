#include "gstinspectparser.h"
#include <utility>

namespace GstStudio {

GstStudio::GstInspectParser::GstInspectParser(QObject* parent) : QObject(parent), m_process(new QProcess(this)) {
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &GstInspectParser::onProcessFinished);
}

bool GstStudio::GstInspectParser::parseAllElements() {
    QProcess process;
    process.start("gst-inspect-1.0", QStringList() << "--print-all");
    if (!process.waitForStarted()) {
        return false;
    }

    if (!process.waitForFinished()) {
        return false;
    }

    return true;
}

GstElement GstStudio::GstInspectParser::parseElement(const QString& elementName) {
    QProcess process;
    process.start("gst-inspect-1.0", QStringList() << elementName);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    return parseElementDetails(output);
}

void GstInspectParser::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitCode == 0) {
        QString output = m_process->readAllStandardOutput();
        parseElementList(output);
        emit parsingFinished();
    }
}

void GstStudio::GstInspectParser::parseElementList(const QString& output) {
    // For --print-all, we need to look for element sections
    // Each element starts with "elementname: Factory Details:"
    static QRegularExpression elementStartRegex(R"(^(\w+):\s+Factory Details:)");
    QStringList lines = output.split('\n');

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch match = elementStartRegex.match(lines[i]);
        if (match.hasMatch()) {
            QString elementName = match.captured(1);

            // Find the end of this element's section (next element or end of file)
            int endIndex = static_cast<int>(lines.size());
            for (int j = i + 1; j < lines.size(); ++j) {
                if (elementStartRegex.match(lines[j]).hasMatch()) {
                    endIndex = j;
                    break;
                }
            }

            // Extract this element's section
            QStringList elementLines = lines.mid(i, endIndex - i);
            QString elementOutput = elementLines.join('\n');

            // Parse the element details
            GstElement element = parseElementDetails(elementOutput);
            element.m_name = elementName;

            m_elements[elementName] = element;
            emit elementParsed(elementName);

            // Skip to the next element
            i = endIndex - 1;
        }
    }
}

GstElement GstStudio::GstInspectParser::parseElementDetails(const QString& output) {
    GstElement element;

    // Parse factory details - looking for lines like "dv1394src:   Long-name                Firewire (1394) DV video
    // source"
    static QRegularExpression longNameRegex(R"(\w+:\s+Long-name\s+(.+))");
    QRegularExpressionMatch longNameMatch = longNameRegex.match(output);
    if (longNameMatch.hasMatch()) {
        element.m_longName = longNameMatch.captured(1).trimmed();
    }

    static QRegularExpression klassRegex(R"(\w+:\s+Klass\s+(.+))");
    QRegularExpressionMatch klassMatch = klassRegex.match(output);
    if (klassMatch.hasMatch()) {
        element.m_classification = klassMatch.captured(1).trimmed();
    }

    static QRegularExpression descRegex(R"(\w+:\s+Description\s+(.+))");
    QRegularExpressionMatch descMatch = descRegex.match(output);
    if (descMatch.hasMatch()) {
        element.m_description = descMatch.captured(1).trimmed();
    }

    static QRegularExpression authorRegex(R"(\w+:\s+Author\s+(.+))");
    QRegularExpressionMatch authorMatch = authorRegex.match(output);
    if (authorMatch.hasMatch()) {
        element.m_author = authorMatch.captured(1).trimmed();
    }

    // Parse rank
    static QRegularExpression rankRegex(R"(\w+:\s+Rank\s+(\w+)\s+\((\d+)\))");
    QRegularExpressionMatch rankMatch = rankRegex.match(output);
    if (rankMatch.hasMatch()) {
        element.m_rank = rankMatch.captured(1);
    }

    // Extract and parse properties section
    QString propertiesSection = extractSection(output, "Element Properties:");
    if (!propertiesSection.isEmpty()) {
        parseProperties(propertiesSection, element);
    }

    // Extract and parse pad templates section
    QString padSection = extractSection(output, "Pad Templates:");
    if (!padSection.isEmpty()) {
        parsePadTemplates(padSection, element);
    }

    return element;
}

void GstStudio::GstInspectParser::parseProperties(const QString& section, GstElement& element) {

    // Properties are formatted like:
    // dv1394src:   automatic-eos       : Automatically EOS when the segment is done
    // dv1394src:                         flags: lesbar, schreibbar
    // dv1394src:                         Boolean. Default: true

    QStringList lines = section.split('\n');
    GstProperty currentProperty;

    for (const QString& line : std::as_const(lines)) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) {
            // Empty line might indicate end of current property
            if (!currentProperty.m_name.isEmpty()) {
                element.m_properties.append(currentProperty);
                currentProperty = GstProperty();
            }
            continue;
        }

        // Check if this is a new property line (has property name and description)
        static QRegularExpression propertyStartRegex(R"(\w+:\s+(\w+(?:-\w+)*)\s*:\s*(.+))");
        QRegularExpressionMatch propMatch = propertyStartRegex.match(line);

        if (propMatch.hasMatch()) {
            // Save previous property if exists
            if (!currentProperty.m_name.isEmpty()) {
                element.m_properties.append(currentProperty);
            }

            // Start new property
            currentProperty = GstProperty();
            currentProperty.m_name = propMatch.captured(1);
            currentProperty.m_description = propMatch.captured(2);
        } else if (!currentProperty.m_name.isEmpty()) {
            // This is a continuation line for the current property

            // Parse flags line
            if (trimmedLine.contains("flags:")) {
                currentProperty.m_readable = trimmedLine.contains("lesbar") || trimmedLine.contains("readable");
                currentProperty.m_writable = trimmedLine.contains("schreibbar") || trimmedLine.contains("writable");
                currentProperty.m_controllable = trimmedLine.contains("controllable");
            }

            // Parse type and default value
            static QRegularExpression typeRegex(
                R"(^(\w+(?:\s+\w+)*)\.\s*(?:Range:\s*([^D]+?))?\s*(?:Default:\s*(.+))?$)");
            QRegularExpressionMatch typeMatch = typeRegex.match(trimmedLine);
            if (typeMatch.hasMatch()) {
                currentProperty.m_type = typeMatch.captured(1);
                if (!typeMatch.captured(2).isEmpty()) {
                    currentProperty.m_range = typeMatch.captured(2).trimmed();
                }
                if (!typeMatch.captured(3).isEmpty()) {
                    currentProperty.m_defaultValue = typeMatch.captured(3).trimmed();
                }
            }

            // Parse enum values (if this line contains enum information)
            if (trimmedLine.contains("Enum") && trimmedLine.contains("Default:")) {
                static QRegularExpression enumRegex(R"(\((\d+)\):\s*(\w+))");
                QRegularExpressionMatchIterator enumIterator = enumRegex.globalMatch(trimmedLine);
                while (enumIterator.hasNext()) {
                    QRegularExpressionMatch enumMatch = enumIterator.next();
                    currentProperty.m_enumValues.append(enumMatch.captured(2));
                }
            }
        }
    }

    // Don't forget the last property
    if (!currentProperty.m_name.isEmpty()) {
        element.m_properties.append(currentProperty);
    }
}

GstProperty GstStudio::GstInspectParser::parseProperty(const QString& propertyText) {
    GstProperty prop;
    QStringList lines = propertyText.split('\n');

    if (lines.isEmpty())
        return prop;

    // First line contains name and description
    const QString& firstLine = lines.first();
    static QRegularExpression nameRegex(R"(^\s*(\w+)\s*:\s*(.+)$)");
    QRegularExpressionMatch nameMatch = nameRegex.match(firstLine);

    if (nameMatch.hasMatch()) {
        prop.m_name = nameMatch.captured(1);
        prop.m_description = nameMatch.captured(2);
    }

    // Parse remaining lines for flags, type, default value, etc.
    for (int i = 1; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();

        // Parse flags
        if (line.startsWith("flags:")) {
            prop.m_readable = line.contains("readable");
            prop.m_writable = line.contains("writable");
            prop.m_controllable = line.contains("controllable");
        }

        // Parse type and default value
        static QRegularExpression typeRegex(R"(^(\w+)\.\s*(?:Range:\s*([^D]+))?\s*(?:Default:\s*(.+))?$)");
        QRegularExpressionMatch typeMatch = typeRegex.match(line);
        if (typeMatch.hasMatch()) {
            prop.m_type = typeMatch.captured(1);
            prop.m_range = typeMatch.captured(2).trimmed();
            prop.m_defaultValue = typeMatch.captured(3).trimmed();
        }

        // Parse enum values
        if (line.contains("Enum ") && line.contains("Default:")) {
            static QRegularExpression enumRegex(R"(\((\d+)\):\s*(\w+))");
            QRegularExpressionMatchIterator enumIterator = enumRegex.globalMatch(line);
            while (enumIterator.hasNext()) {
                QRegularExpressionMatch enumMatch = enumIterator.next();
                prop.m_enumValues.append(enumMatch.captured(2));
            }
        }
    }

    return prop;
}

void GstStudio::GstInspectParser::parsePadTemplates(const QString& section, GstElement& element) {
    QStringList lines = section.split('\n');
    GstPadTemplate currentPad;
    bool inCapabilities = false;

    for (const QString& line : std::as_const(lines)) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty())
            continue;

        if (processPadTemplateHeader(line, currentPad, element, inCapabilities)) {
            continue;
        }

        if (processAvailabilityLine(trimmedLine, currentPad)) {
            continue;
        }

        if (processCapabilitiesSection(trimmedLine, currentPad, inCapabilities)) {
            continue;
        }
    }

    finalizePadTemplate(currentPad, element);
}

GstPadTemplate GstStudio::GstInspectParser::parsePadTemplate(const QString& padText) {
    GstPadTemplate pad;
    QStringList lines = padText.split('\n');

    for (const QString& line : std::as_const(lines)) {
        QString trimmedLine = line.trimmed();

        parsePadNameAndDirection(trimmedLine, pad);
        processAvailabilityLine(trimmedLine, pad);
        processCapabilityLine(trimmedLine, pad);
    }

    return pad;
}

bool GstStudio::GstInspectParser::processPadTemplateHeader(const QString& line, GstPadTemplate& currentPad,
                                                           GstElement& element, bool& inCapabilities) {
    static QRegularExpression padRegex(R"(\w+:\s+(SRC|SINK)\s+template:\s*'([^']+)')");
    QRegularExpressionMatch padMatch = padRegex.match(line);
    if (padMatch.hasMatch()) {
        finalizePadTemplate(currentPad, element);

        currentPad = GstPadTemplate();
        currentPad.m_direction = padMatch.captured(1);
        currentPad.m_name = padMatch.captured(2);
        inCapabilities = false;
        return true;
    }
    return false;
}

bool GstStudio::GstInspectParser::processAvailabilityLine(const QString& trimmedLine, GstPadTemplate& pad) {
    if (trimmedLine.contains("Availability:")) {
        if (trimmedLine.contains("Always"))
            pad.m_presence = "ALWAYS";
        else if (trimmedLine.contains("Sometimes"))
            pad.m_presence = "SOMETIMES";
        else if (trimmedLine.contains("On request"))
            pad.m_presence = "REQUEST";
        return true;
    }
    return false;
}

bool GstStudio::GstInspectParser::processCapabilitiesSection(const QString& trimmedLine, GstPadTemplate& currentPad,
                                                             bool& inCapabilities) {
    if (trimmedLine.contains("Capabilities:")) {
        inCapabilities = true;
        return true;
    }

    if (inCapabilities && isCapabilityLine(trimmedLine)) {
        if (!currentPad.m_caps.isEmpty()) {
            currentPad.m_caps += "\n";
        }
        currentPad.m_caps += trimmedLine;
        return true;
    }

    if (inCapabilities && isSectionEnd(trimmedLine)) {
        inCapabilities = false;
        return true;
    }

    return false;
}

bool GstStudio::GstInspectParser::isCapabilityLine(const QString& line) {
    return line.startsWith("video/") || line.startsWith("audio/") || line.startsWith("application/") ||
           line.startsWith("text/") || line.startsWith("image/") || line.contains("format:") ||
           line.contains("systemstream:");
}

bool GstStudio::GstInspectParser::isSectionEnd(const QString& line) {
    return line.contains("Element has") || line.contains("URI handling") || line.contains("Pads:");
}

void GstStudio::GstInspectParser::finalizePadTemplate(GstPadTemplate& pad, GstElement& element) {
    if (!pad.m_name.isEmpty()) {
        element.m_padTemplates.append(pad);
    }
}

void GstStudio::GstInspectParser::parsePadNameAndDirection(const QString& line, GstPadTemplate& pad) {
    static QRegularExpression padRegex(R"(^(SRC|SINK)\s+template:\s*'([^']+)')");
    QRegularExpressionMatch padMatch = padRegex.match(line);
    if (padMatch.hasMatch()) {
        pad.m_direction = padMatch.captured(1);
        pad.m_name = padMatch.captured(2);
    }
}

void GstStudio::GstInspectParser::processCapabilityLine(const QString& line, GstPadTemplate& pad) {
    if (line.startsWith("Capabilities:")) {
        return;
    }

    if (line.contains("/") && !line.startsWith("Availability:")) {
        if (!pad.m_caps.isEmpty())
            pad.m_caps += "\n";
        pad.m_caps += line;
    }
}

QString GstStudio::GstInspectParser::extracted() {
    return {};
}
QString GstStudio::GstInspectParser::extractSection(const QString& text, const QString& sectionName) {
    auto startPos = text.indexOf(sectionName);
    if (startPos == -1)
        return extracted();

    startPos += sectionName.length();

    // Find the next major section or end of text
    QStringList nextSections = {
        "Factory Details:", "Pad Templates:", "Element Properties:", "Element Signals:", "Element Actions:"};
    auto endPos = text.length();

    for (const QString& nextSection : nextSections) {
        if (nextSection == sectionName)
            continue;
        auto pos = text.indexOf(nextSection, startPos);
        if (pos != -1 && pos < endPos) {
            endPos = pos;
        }
    }

    return text.mid(startPos, endPos - startPos);
}

QStringList GstStudio::GstInspectParser::splitIntoSections(const QString& text, const QString& delimiter) {
    return text.split(delimiter, Qt::SkipEmptyParts);
}

QString GstStudio::GstInspectParser::cleanupText(const QString& text) {
    static QRegularExpression replaceRegex("\\s+");
    return text.trimmed().replace(replaceRegex, " ");
}

QStringList GstStudio::GstInspectParser::getAllElementNames() const {
    return m_elements.keys();
}

GstElement GstStudio::GstInspectParser::getElement(const QString& name) const {
    return m_elements.value(name);
}

QStringList GstStudio::GstInspectParser::getElementsByClassification(const QString& classification) const {
    QStringList result;
    for (auto it = m_elements.begin(); it != m_elements.end(); ++it) {
        if (it.value().m_classification.contains(classification, Qt::CaseInsensitive)) {
            result.append(it.key());
        }
    }
    return result;
}

} // namespace GstStudio
