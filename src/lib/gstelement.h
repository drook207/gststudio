/**
 * @file gstelement.h
 * @brief Data structures for GStreamer elements, properties and pad templates
 * @author GstStudio Team
 */

#pragma once

#include <QList>
#include <QString>

namespace GstStudio {

/**
 * @struct GstProperty
 * @brief Represents a GStreamer element property
 *
 * This structure contains all the metadata for a GStreamer element property,
 * including its name, type, description, default value, and various flags.
 */
struct GstProperty {
    QString m_name;              ///< Property name
    QString m_type;              ///< Property type (e.g., "gint", "gboolean", "gchararray")
    QString m_description;       ///< Human-readable description of the property
    QString m_defaultValue;      ///< Default value as string
    QString m_range;             ///< Valid range for numeric properties
    QStringList m_enumValues;    ///< Valid enum values for enum properties
    bool m_writable = false;     ///< Whether the property can be written to
    bool m_readable = false;     ///< Whether the property can be read from
    bool m_controllable = false; ///< Whether the property can be controlled via GstController
};

/**
 * @struct GstPadTemplate
 * @brief Represents a GStreamer element pad template
 *
 * Pad templates describe the pads that an element can create,
 * including their direction, presence, and supported capabilities.
 */
struct GstPadTemplate {
    QString m_name;      ///< Pad template name
    QString m_direction; ///< Pad direction: "SRC" or "SINK"
    QString m_presence;  ///< Pad presence: "ALWAYS", "SOMETIMES", or "REQUEST"
    QString m_caps;      ///< Supported capabilities as string
};

/**
 * @struct GstElement
 * @brief Represents a complete GStreamer element
 *
 * This structure contains all metadata for a GStreamer element,
 * including its properties, pad templates, and descriptive information.
 */
struct GstElement {
    QString m_name;                       ///< Element name (e.g., "videotestsrc")
    QString m_longName;                   ///< Human-readable long name
    QString m_description;                ///< Element description
    QString m_author;                     ///< Element author information
    QString m_classification;             ///< Element classification (e.g., "Source/Video")
    QString m_rank;                       ///< Element rank for autoplugging
    QList<GstProperty> m_properties;      ///< List of element properties
    QList<GstPadTemplate> m_padTemplates; ///< List of pad templates
};

} // namespace GstStudio
