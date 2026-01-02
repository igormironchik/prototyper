
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "html_exporter.hpp"
#include "../constants.hpp"
#include "../form/utils.hpp"
#include "exporter_private.hpp"

// Qt include.
#include <QBuffer>
#include <QByteArray>
#include <QFile>
#include <QSvgGenerator>
#include <QTextStream>

namespace Prototyper
{

namespace Core
{

//
// HtmlExporterPrivate
//

class HtmlExporterPrivate : public ExporterPrivate
{
public:
    HtmlExporterPrivate(const Cfg::Project &cfg,
                        HtmlExporter *parent)
        : ExporterPrivate(cfg,
                          parent)
    {
    }

    //! Print document.
    void printDocument(QTextStream &stream);
}; // class HtmlExporterPrivate

static inline QString printStyle(const Cfg::TextStyle &style)
{
    return QStringLiteral("style=\"font-size: ") + QString::number(qRound(style.fontSize())) + QStringLiteral("pt;\"");
}

static inline void printText(QTextStream &stream,
                             const std::vector<Cfg::TextStyle> &text)
{
    for (const Cfg::TextStyle &t : text) {
        stream
            << QStringLiteral("<span ")
            << printStyle(t)
            << QStringLiteral(">")
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_boldStyle) != t.style().cend()
                    ? QStringLiteral("<b>")
                    : QString())
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_italicStyle) != t.style().cend()
                    ? QStringLiteral("<i>")
                    : QString())
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_underlineStyle) != t.style().cend()
                    ? QStringLiteral("<ins>")
                    : QString());

        if (!t.link().isEmpty()) {
            stream << QStringLiteral("<a href=\"") << t.link() << QStringLiteral("\">") << Qt::endl;
        }

        QStringList strings = t.text().split(QLatin1Char('\n'));

        foreach (const QString &str, strings) {
            stream << str << QStringLiteral("<br>") << Qt::endl;
        }

        if (!t.link().isEmpty()) {
            stream << QStringLiteral("</a>") << Qt::endl;
        }

        stream
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_underlineStyle) != t.style().cend()
                    ? QStringLiteral("</ins>")
                    : QString())
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_italicStyle) != t.style().cend()
                    ? QStringLiteral("</i>")
                    : QString())
            << (std::find(t.style().cbegin(), t.style().cend(), Cfg::c_boldStyle) != t.style().cend()
                    ? QStringLiteral("</b>")
                    : QString())
            << QStringLiteral("</span>")
            << Qt::endl;
    }
}

void HtmlExporterPrivate::printDocument(QTextStream &stream)
{
    stream
        << QStringLiteral("<!DOCTYPE html><head></head>")
        << Qt::endl
        << QStringLiteral("<body>")
        << Qt::endl
        << QStringLiteral("<div style=\"width: 800px; margin: auto;\">");

    printText(stream, m_cfg.description().text());

    stream << QStringLiteral("<br><br>") << Qt::endl;

    for (const Cfg::Page &form : m_cfg.page()) {
        std::vector<Cfg::TextStyle> headList;
        Cfg::TextStyle head;
        head.style().push_back(Cfg::c_boldStyle);
        head.fontSize() = c_headerFontSize;
        head.text() = form.tabName();
        headList.push_back(head);

        stream << QStringLiteral("<a name=\"") << form.tabName() << QStringLiteral("\">");

        printText(stream, headList);

        stream << QStringLiteral("</a>") << QStringLiteral("<br><br>");

        QByteArray data;
        QBuffer buff(&data);
        buff.open(QIODevice::WriteOnly);
        QSvgGenerator svg;
        svg.setResolution(c_resolution);
        svg.setOutputDevice(&buff);

        drawForm(svg, form, c_resolution);

        const int i = data.indexOf(QStringLiteral("\n").toLatin1());

        if (i != -1) {
            data.remove(0, i);
        }

        stream << QStringLiteral("<div>") << data << QStringLiteral("</div><br>");
    }

    stream << QStringLiteral("</div></body>") << Qt::endl;
}

//
// HtmlExporter
//

HtmlExporter::HtmlExporter(const Cfg::Project &project)
    : Exporter(std::make_unique<HtmlExporterPrivate>(project,
                                                     this))
{
}

HtmlExporter::~HtmlExporter() = default;

void HtmlExporter::exportToDoc(const QString &fileName)
{
    QFile file(fileName);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    d_ptr()->printDocument(stream);

    file.close();
}

} /* namespace Core */

} /* namespace Prototyper */
