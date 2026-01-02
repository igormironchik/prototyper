
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__SVG_EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__SVG_EXPORTER_HPP__INCLUDED

// Prototyper include.
#include "exporter.hpp"

namespace Prototyper
{

namespace Core
{

//
// SvgExporter
//

class SvgExporterPrivate;

//! Exporter to SVG.
class SvgExporter final : public Exporter
{
public:
    explicit SvgExporter(const Cfg::Project &project);
    ~SvgExporter();

    //! Export documentation.
    void exportToDoc(const QString &fileName) override;

private:
    inline const SvgExporterPrivate *d_ptr() const
    {
        return reinterpret_cast<const SvgExporterPrivate *>(d.get());
    }
    inline SvgExporterPrivate *d_ptr()
    {
        return reinterpret_cast<SvgExporterPrivate *>(d.get());
    }

private:
    Q_DISABLE_COPY(SvgExporter)
}; // class SvgExporter

//
// SvgExporterException
//

class SvgExporterException final
{
public:
    explicit SvgExporterException(const QString &w);

    const QString &what() const noexcept;

private:
    QString m_what;
}; // class SvgExporterException

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PDF_EXPORTER_HPP__INCLUDED
