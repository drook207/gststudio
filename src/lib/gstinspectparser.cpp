#include "gstinspectparser.h"
#include <utility>

namespace GstStudio {

GstStudio::GstInspectParser::GstInspectParser(QObject* parent) : QObject(parent), m_process(new QProcess(this)) {
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &GstInspectParser::onProcessFinished);
}

bool GstStudio::GstInspectParser::parseAllElements() {
    if (m_process->state() != QProcess::NotRunning) {
        return false;
    }

    // First get list of all elements
    m_process->start("gst-inspect-1.0", QStringList() << "--print-all");
    return m_process->waitForStarted();
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
            int endIndex = lines.size();
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
            element.name = elementName;

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
        element.longName = longNameMatch.captured(1).trimmed();
    }

    static QRegularExpression klassRegex(R"(\w+:\s+Klass\s+(.+))");
    QRegularExpressionMatch klassMatch = klassRegex.match(output);
    if (klassMatch.hasMatch()) {
        element.classification = klassMatch.captured(1).trimmed();
    }

    static QRegularExpression descRegex(R"(\w+:\s+Description\s+(.+))");
    QRegularExpressionMatch descMatch = descRegex.match(output);
    if (descMatch.hasMatch()) {
        element.description = descMatch.captured(1).trimmed();
    }

    static QRegularExpression authorRegex(R"(\w+:\s+Author\s+(.+))");
    QRegularExpressionMatch authorMatch = authorRegex.match(output);
    if (authorMatch.hasMatch()) {
        element.author = authorMatch.captured(1).trimmed();
    }

    // Parse rank
    static QRegularExpression rankRegex(R"(\w+:\s+Rank\s+(\w+)\s+\((\d+)\))");
    QRegularExpressionMatch rankMatch = rankRegex.match(output);
    if (rankMatch.hasMatch()) {
        element.rank = rankMatch.captured(1);
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
            if (!currentProperty.name.isEmpty()) {
                element.properties.append(currentProperty);
                currentProperty = GstProperty();
            }
            continue;
        }

        // Check if this is a new property line (has property name and description)
        static QRegularExpression propertyStartRegex(R"(\w+:\s+(\w+(?:-\w+)*)\s*:\s*(.+))");
        QRegularExpressionMatch propMatch = propertyStartRegex.match(line);

        if (propMatch.hasMatch()) {
            // Save previous property if exists
            if (!currentProperty.name.isEmpty()) {
                element.properties.append(currentProperty);
            }

            // Start new property
            currentProperty = GstProperty();
            currentProperty.name = propMatch.captured(1);
            currentProperty.description = propMatch.captured(2);
        } else if (!currentProperty.name.isEmpty()) {
            // This is a continuation line for the current property

            // Parse flags line
            if (trimmedLine.contains("flags:")) {
                currentProperty.readable = trimmedLine.contains("lesbar") || trimmedLine.contains("readable");
                currentProperty.writable = trimmedLine.contains("schreibbar") || trimmedLine.contains("writable");
                currentProperty.controllable = trimmedLine.contains("controllable");
            }

            // Parse type and default value
            static QRegularExpression typeRegex(
                R"(^(\w+(?:\s+\w+)*)\.\s*(?:Range:\s*([^D]+?))?\s*(?:Default:\s*(.+))?$)");
            QRegularExpressionMatch typeMatch = typeRegex.match(trimmedLine);
            if (typeMatch.hasMatch()) {
                currentProperty.type = typeMatch.captured(1);
                if (!typeMatch.captured(2).isEmpty()) {
                    currentProperty.range = typeMatch.captured(2).trimmed();
                }
                if (!typeMatch.captured(3).isEmpty()) {
                    currentProperty.defaultValue = typeMatch.captured(3).trimmed();
                }
            }

            // Parse enum values (if this line contains enum information)
            if (trimmedLine.contains("Enum") && trimmedLine.contains("Default:")) {
                static QRegularExpression enumRegex(R"(\((\d+)\):\s*(\w+))");
                QRegularExpressionMatchIterator enumIterator = enumRegex.globalMatch(trimmedLine);
                while (enumIterator.hasNext()) {
                    QRegularExpressionMatch enumMatch = enumIterator.next();
                    currentProperty.enumValues.append(enumMatch.captured(2));
                }
            }
        }
    }

    // Don't forget the last property
    if (!currentProperty.name.isEmpty()) {
        element.properties.append(currentProperty);
    }
}

GstProperty GstStudio::GstInspectParser::parseProperty(const QString& propertyText) {
    GstProperty prop;
    QStringList lines = propertyText.split('\n');

    if (lines.isEmpty())
        return prop;

    // First line contains name and description
    QString firstLine = lines.first();
    static QRegularExpression nameRegex(R"(^\s*(\w+)\s*:\s*(.+)$)");
    QRegularExpressionMatch nameMatch = nameRegex.match(firstLine);

    if (nameMatch.hasMatch()) {
        prop.name = nameMatch.captured(1);
        prop.description = nameMatch.captured(2);
    }

    // Parse remaining lines for flags, type, default value, etc.
    for (int i = 1; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();

        // Parse flags
        if (line.startsWith("flags:")) {
            prop.readable = line.contains("readable");
            prop.writable = line.contains("writable");
            prop.controllable = line.contains("controllable");
        }

        // Parse type and default value
        static QRegularExpression typeRegex(R"(^(\w+)\.\s*(?:Range:\s*([^D]+))?\s*(?:Default:\s*(.+))?$)");
        QRegularExpressionMatch typeMatch = typeRegex.match(line);
        if (typeMatch.hasMatch()) {
            prop.type = typeMatch.captured(1);
            prop.range = typeMatch.captured(2).trimmed();
            prop.defaultValue = typeMatch.captured(3).trimmed();
        }

        // Parse enum values
        if (line.contains("Enum ") && line.contains("Default:")) {
            static QRegularExpression enumRegex(R"(\((\d+)\):\s*(\w+))");
            QRegularExpressionMatchIterator enumIterator = enumRegex.globalMatch(line);
            while (enumIterator.hasNext()) {
                QRegularExpressionMatch enumMatch = enumIterator.next();
                prop.enumValues.append(enumMatch.captured(2));
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

        // Parse pad template header like: "dv1394src:   SRC template: 'src'"
        static QRegularExpression padRegex(R"(\w+:\s+(SRC|SINK)\s+template:\s*'([^']+)')");
        QRegularExpressionMatch padMatch = padRegex.match(line);
        if (padMatch.hasMatch()) {
            // Save previous pad if exists
            if (!currentPad.name.isEmpty()) {
                element.padTemplates.append(currentPad);
            }

            // Start new pad
            currentPad = GstPadTemplate();
            currentPad.direction = padMatch.captured(1);
            currentPad.name = padMatch.captured(2);
            inCapabilities = false;
        }

        // Parse availability
        if (trimmedLine.contains("Availability:")) {
            if (trimmedLine.contains("Always"))
                currentPad.presence = "ALWAYS";
            else if (trimmedLine.contains("Sometimes"))
                currentPad.presence = "SOMETIMES";
            else if (trimmedLine.contains("On request"))
                currentPad.presence = "REQUEST";
        }

        // Start of capabilities section
        if (trimmedLine.contains("Capabilities:")) {
            inCapabilities = true;
            continue;
        }

        // Capabilities content (indented lines after "Capabilities:")
        if (inCapabilities && (trimmedLine.startsWith("video/") || trimmedLine.startsWith("audio/") ||
                               trimmedLine.startsWith("application/") || trimmedLine.startsWith("text/") ||
                               trimmedLine.startsWith("image/") || trimmedLine.contains("format:") ||
                               trimmedLine.contains("systemstream:"))) {
            if (!currentPad.caps.isEmpty()) {
                currentPad.caps += "\n";
            }
            currentPad.caps += trimmedLine;
        }

        // Stop capabilities parsing when we hit another section
        if (inCapabilities && (trimmedLine.contains("Element has") || trimmedLine.contains("URI handling") ||
                               trimmedLine.contains("Pads:"))) {
            inCapabilities = false;
        }
    }

    // Don't forget the last pad
    if (!currentPad.name.isEmpty()) {
        element.padTemplates.append(currentPad);
    }
}

GstPadTemplate GstStudio::GstInspectParser::parsePadTemplate(const QString& padText) {
    GstPadTemplate pad;
    QStringList lines = padText.split('\n');

    for (const QString& line : std::as_const(lines)) {
        QString trimmedLine = line.trimmed();

        // Parse pad name and direction like: "SRC template: 'src'"
        static QRegularExpression padRegex(R"(^(SRC|SINK)\s+template:\s*'([^']+)')");
        QRegularExpressionMatch padMatch = padRegex.match(trimmedLine);
        if (padMatch.hasMatch()) {
            pad.direction = padMatch.captured(1);
            pad.name = padMatch.captured(2);
        }

        // Parse presence
        if (trimmedLine.startsWith("Availability:")) {
            if (trimmedLine.contains("Always"))
                pad.presence = "ALWAYS";
            else if (trimmedLine.contains("Sometimes"))
                pad.presence = "SOMETIMES";
            else if (trimmedLine.contains("On request"))
                pad.presence = "REQUEST";
        }

        // Capabilities span multiple lines, collect them
        if (trimmedLine.startsWith("Capabilities:")) {
            // Caps follow on subsequent lines, typically indented
            continue;
        }

        // If this looks like a capability line (starts with media type)
        if (trimmedLine.contains("/") && !trimmedLine.startsWith("Availability:")) {
            if (!pad.caps.isEmpty())
                pad.caps += "\n";
            pad.caps += trimmedLine;
        }
    }

    return pad;
}

QString GstStudio::GstInspectParser::extractSection(const QString& text, const QString& sectionName) {
    int startPos = text.indexOf(sectionName);
    if (startPos == -1)
        return QString();

    startPos += sectionName.length();

    // Find the next major section or end of text
    QStringList nextSections = {
        "Factory Details:", "Pad Templates:", "Element Properties:", "Element Signals:", "Element Actions:"};
    int endPos = text.length();

    for (const QString& nextSection : nextSections) {
        if (nextSection == sectionName)
            continue;
        int pos = text.indexOf(nextSection, startPos);
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
        if (it.value().classification.contains(classification, Qt::CaseInsensitive)) {
            result.append(it.key());
        }
    }
    return result;
}

} // namespace GstStudio
