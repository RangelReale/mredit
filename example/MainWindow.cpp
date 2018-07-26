#include "MainWindow.h"

#include <mredit/margin/MarginStacker.h>

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QVBoxLayout>

MainWindow::MainWindow() : QMainWindow()
{
	_editor = new mredit::Editor(this);
	setCentralWidget(_editor);
	
	_editor->marginStacker()->setVisible(mredit::Global::Margin::BookmarkMargin, true);
	_editor->marginStacker()->setVisible(mredit::Global::Margin::NumberMargin, true);
	_editor->marginStacker()->setVisible(mredit::Global::Margin::RevisionMargin, true);
	//_editor->marginStacker()->setVisible(mredit::Global::Margin::FoldMargin, true);
	_editor->marginStacker()->setVisible(mredit::Global::Margin::SpaceMargin, true);

	_bm = _editor->bookmarkGroups().addGroup("Bookmark", QIcon(":/bookmarks.png"));
	_bm2 = _editor->bookmarkGroups().addGroup("Call stack", QIcon(":/arrow-right.png"));

	QTextBlockFormat tmformat;
	tmformat.setBackground(QColor(Qt::yellow));
	_editor->bookmarkGroups().getGroup(_bm2)->setTextBlockFormat(tmformat);

	_editor->setText("filter_impl<Predicate, type-parameter-0-1, type-parameter-0-2...>");

	_editor->setBookmark(_bm, 10, true);
	_editor->setBookmark(_bm2, 12, true);

	QMenu *fileMenu = menuBar()->addMenu("&File");

	QAction *testMenu = new QAction("&Test", this);
	connect(testMenu, &QAction::triggered, this, &MainWindow::menuClicked);
	fileMenu->addAction(testMenu);

	resize(1024, 768);
}

void MainWindow::menuClicked()
{
	_editor->clearBookmarks(_bm2);
}