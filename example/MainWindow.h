#pragma once

#include <mredit/Editor.h>

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
	MainWindow();
public slots:
	void menuClicked();
private:
	mredit::Editor *_editor;
	int _bm, _bm2;
};