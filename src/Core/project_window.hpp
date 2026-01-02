
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED

// Qt include.
#include <QMainWindow>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

class ProjectWidget;
class TabsList;
class PageView;

//
// ProjectWindow
//

class ProjectWindowPrivate;

//! Main window with project.
class ProjectWindow final : public QMainWindow
{
    Q_OBJECT

public:
    ProjectWindow(QWidget *parent = 0,
                  Qt::WindowFlags f = Qt::WindowFlags());
    ~ProjectWindow();

    //! \return Project widget.
    ProjectWidget *projectWidget() const;

    //! \return Project's file name.
    const QString &projectFileName() const;

    //! \return Show/hide grid action.
    QAction *showHideGridAction() const;
    //! \return Grid step action.
    QAction *gridStepAction() const;

    //! \return Tabs list widget.
    TabsList *tabsList() const;

    //! \return Author.
    const QString &author() const;
    //! Set author.
    void setAuthor(const QString &name);

public slots:
    //! Read project.
    void readProject(const QString &fileName);
    //! Set grid step.
    void setGridStep(int step,
                     bool forAll);
    //! Switch to polyline mode.
    void switchToPolylineMode();
    //! Switch to select mode.
    void switchToSelectMode();

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    //! Quit.
    void quit();
    //! Show/hide grid.
    void showHideGrid(bool show);
    //! Enable/disable snap to grid.
    void snapGrid(bool on);
    //! Set grid step.
    void setGridStep();
    //! Open project.
    void openProject();
    //! New project.
    void newProject();
    //! Save project.
    void saveProjectImpl(const QString &fileName = QString());
    //! Save project.
    void saveProject();
    //! Save project as.
    void saveProjectAs();
    //! Project changed.
    void projectChanged();
    //! Draw line.
    void drawLine();
    //! Draw rect.
    void drawRect();
    //! Draw polyline.
    void drawPolyline(bool on);
    //! Insert text.
    void insertText();
    //! Insert image.
    void insertImage();
    //! Group.
    void group();
    //! Ungroup.
    void ungroup();
    //! Select.
    void select(bool checked);
    //! Fill color.
    void fillColor();
    //! Stroke color.
    void strokeColor();
    //! Tab changed.
    void tabChanged(int index);
    //! Export to PDF.
    void exportToPDf();
    //! Export to HTML.
    void exportToHtml();
    //! Export to SVG images.
    void exportToSvg();
    //! Show about dialog.
    void about();
    //! Show about Qt dialog.
    void aboutQt();
    //! Draw button.
    void drawButton();
    //! Draw combobox.
    void drawComboBox();
    //! Draw radio button.
    void drawRadioButton();
    //! Draw check box.
    void drawCheckBox();
    //! Draw horizontal slider.
    void drawHSlider();
    //! Draw vertical slider.
    void drawVSlider();
    //! Draw spinbox.
    void drawSpinbox();
    //! Align vertical top.
    void alignVerticalTop();
    //! Align vertical center.
    void alignVerticalCenter();
    //! Align vertical bottom.
    void alignVerticalBottom();
    //! Align horizontal left.
    void alignHorizontalLeft();
    //! Align horizontal center.
    void alignHorizontalCenter();
    //! Align horizontal right.
    void alignHorizontalRight();
    //! Can undo changed.
    void canUndoChanged(bool canUndo);
    //! Page added.
    void pageAdded(Prototyper::Core::PageView *form);
    //! Page deleted.
    void pageDeleted(Prototyper::Core::PageView *form);
    //! Zoom in.
    void zoomIn();
    //! Zoom out.
    void zoomOut();
    //! Zoom original.
    void zoomOriginal();
    //! Zoom changed.
    void zoomChanged();
    //! Selection changed.
    void selectionChanged();
    //! Duplicate selection.
    void duplicate();
    //! Bring to top.
    void toTop();
    //! Raise.
    void raise();
    //! Lower.
    void lower();
    //! Bring to bottom.
    void toBottom();

private:
    friend class ProjectWindowPrivate;
    friend class TopGuiPrivate;
    friend class ProjectWidget;
    friend class ProjectWidgetPrivate;

    Q_DISABLE_COPY(ProjectWindow)

    std::unique_ptr<ProjectWindowPrivate> d;
}; // class ProjectWindow

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
