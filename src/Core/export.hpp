
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Qt include.
#include <QtCore/QtGlobal>

#if defined(PROTOTYPER_CORE)
#define PROTOTYPER_CORE_EXPORT Q_DECL_EXPORT
#else
#define PROTOTYPER_CORE_EXPORT Q_DECL_IMPORT
#endif
