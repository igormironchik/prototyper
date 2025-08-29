
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "svg_exporter.hpp"
#include "../constants.hpp"
#include "../form/utils.hpp"
#include "exporter_private.hpp"

// Qt include.
#include <QFile>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>

namespace Prototyper
{

namespace Core
{

//
// SvgExporterPrivate
//

class SvgExporterPrivate : public ExporterPrivate
{
public:
    SvgExporterPrivate(const Cfg::Project &cfg,
                       SvgExporter *parent)
        : ExporterPrivate(cfg,
                          parent)
    {
    }

    //! Create images.
    void createImages(const QString &dir);
}; // class SvgExporterPrivate

void SvgExporterPrivate::createImages(const QString &dir)
{
    int i = 1;

    for (const Cfg::Page &form : m_cfg.page()) {
        const QString fileName = dir + QStringLiteral("/") + QString::number(i) + QStringLiteral(".svg");

        QFile file(fileName);

        bool canModify = true;

        if (!file.open(QIODevice::WriteOnly)) {
            canModify = false;
        }

        file.close();

        if (canModify) {
            QSvgGenerator svg;
            svg.setFileName(fileName);
            svg.setResolution(c_resolution);

            drawForm(svg, form, c_resolution);

            ++i;
        } else {
            throw SvgExporterException(QObject::tr("Unable to export SVG into %1.\n"
                                                   "File is not writable.")
                                           .arg(fileName));
        }
    }
}

//
// SvgExporter
//

SvgExporter::SvgExporter(const Cfg::Project &project)
    : Exporter(std::make_unique<SvgExporterPrivate>(project,
                                                    this))
{
}

SvgExporter::~SvgExporter() = default;

void SvgExporter::exportToDoc(const QString &fileName)
{
    SvgExporterPrivate *d = d_ptr();

    d->createImages(fileName);
}

//
// SvgExporterException
//

SvgExporterException::SvgExporterException(const QString &w)
    : m_what(w)
{
}

const QString &SvgExporterException::what() const noexcept
{
    return m_what;
}

} /* namespace Core */

} /* namespace Prototyper */
