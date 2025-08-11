#ifndef __TERMINAL_WINDOW_HPP__
#define __TERMINAL_WINDOW_HPP__

// Qt Libraries
#include <QFontDatabase>
#include <QHeaderView>
#include <QMainWindow>
#include <QTableView>

// Personal Libraries
#include "QTerminalModel.hpp"

class QTerminalWindow : public QMainWindow
{
	Q_OBJECT
	
	private:
	
		QTableView* const	terminal = new QTableView(this);
		QTerminalModel		model;
	
	private slots:
	
		inline void		updateTable(HexTerminalFile*);
	
	public:
	
		inline			QTerminalWindow(QWidget*);
		inline void		setUp(quint32, quint32, qreal, qreal);
};

QTerminalWindow::QTerminalWindow(QWidget* parent) : QMainWindow(parent)
{
	QMainWindow::setCentralWidget(QTerminalWindow::terminal);
	QMainWindow::setWindowTitle("Terminal Window");
	
	const auto id = QFontDatabase::addApplicationFont("./Font.ttf");
	const auto family = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont monospace(family, 13);
	
	QTerminalWindow::terminal->setFont(monospace);
	QTerminalWindow::terminal->setObjectName("Black");
	
	QTerminalWindow::terminal->setModel(nullptr);
	QTerminalWindow::terminal->reset();
	QTerminalWindow::terminal->setModel(&(QTerminalWindow::model));
	
	const auto header1 = QTerminalWindow::terminal->horizontalHeader();
	header1->setMinimumSectionSize(5);
	header1->resizeSection(0, 80);
	header1->resizeSection(1, 50);
	header1->resizeSection(2, 50);
	header1->resizeSection(3, 19);
	header1->resizeSection(4, 60);
	header1->resizeSection(5, 35);
	header1->resizeSection(6, 19);
	header1->resizeSection(7, 140);
	header1->resizeSection(8, 140);
	header1->resizeSection(9, 140);
	header1->resizeSection(10, 35);
	header1->resizeSection(11, 140);
	header1->resizeSection(12, 140);
	header1->resizeSection(13, 140);
	header1->hide();
	
	const auto header2 = QTerminalWindow::terminal->verticalHeader();
	header2->setMinimumSectionSize(5);
	header2->setDefaultSectionSize(20);
	header2->hide();
}

void QTerminalWindow::setUp(quint32 p, quint32 nc, qreal th, qreal tp)
{
	QTerminalWindow::model.setUp(p, nc, th, tp);
	QTerminalWindow::terminal->setSpan(0, 0, 1, 14);
}

void QTerminalWindow::updateTable(HexTerminalFile* foo)
{
	const auto size = QTerminalWindow::model.process(foo);
	const auto index = QTerminalWindow::model.index(size, 0);
	
	if (foo->ratio == 0u)
	{
		QTerminalWindow::terminal->setSpan(size, 7, 1, 3);
		QTerminalWindow::terminal->setSpan(size, 11, 1, 3);
	}
	
	QTerminalWindow::terminal->scrollTo(index, QAbstractItemView::PositionAtBottom);
	delete foo;
}

#endif
