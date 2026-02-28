
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED

// C++ include.
#include <memory>

// Prototyper include.
#include "../types.hpp"
#include "project_cfg.hpp"

QT_BEGIN_NAMESPACE
class QSvgGenerator;
class QPainter;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

class ExporterPrivate;

//
// Exporter
//

//! Base class for exporters to the doc.
class Exporter
{
public:
    explicit Exporter(const Cfg::Project &project);
    virtual ~Exporter() = default;

    //! Export documentation.
    virtual void exportToDoc(const QString &fileName) = 0;

    //! \return Hash of images.
    ImagesHash images() const;

protected:
    explicit Exporter(std::unique_ptr<ExporterPrivate> &&dd);

protected:
    std::unique_ptr<ExporterPrivate> d;

private:
    Q_DISABLE_COPY(Exporter)
}; // class Exporter

namespace impl
{

void drawGroup(const Cfg::Group &group,
               QPainter &p,
               qreal dpi,
               QSvgGenerator &svg,
               const ImagesHash &images);

} /* namespace impl */

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED
