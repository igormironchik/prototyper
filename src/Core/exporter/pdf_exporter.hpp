
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PDF_EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__PDF_EXPORTER_HPP__INCLUDED

// Prototyper include.
#include "exporter.hpp"

namespace Prototyper
{

namespace Core
{

//
// PdfExporter
//

class PdfExporterPrivate;

//! Exporter to PDF.
class PdfExporter final : public Exporter
{
public:
    explicit PdfExporter(const Cfg::Project &project);
    ~PdfExporter();

    //! Export documentation.
    void exportToDoc(const QString &fileName) override;

private:
    inline const PdfExporterPrivate *d_ptr() const
    {
        return reinterpret_cast<const PdfExporterPrivate *>(d.get());
    }
    inline PdfExporterPrivate *d_ptr()
    {
        return reinterpret_cast<PdfExporterPrivate *>(d.get());
    }

private:
    Q_DISABLE_COPY(PdfExporter)
}; // class PdfExporter

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PDF_EXPORTER_HPP__INCLUDED
