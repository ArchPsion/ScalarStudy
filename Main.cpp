// Qt Libraries
#include <QApplication>

// Personal Libraries
#include "QScalarWindow.hpp"

const std::vector<qreal> QFeedingThread::Gaps = { 0.25, 0.5, 1., 5., 10., 50., 100., 500. };
std::vector<qreal> HexNormalisedInfo::Coefficients = { };

int main(int argc, char* argv[])
{
	const QString stylesheet = "QLineEdit { min-width: 0px }"
				"QLabel#Blue { color: rgb(51, 153, 255) }"
				"QLabel#Red { color: rgb(255, 68, 68) }"
				"QPushButton#Blue { background-color: rgb(153, 204, 255) }"
				"QPushButton#Green { background-color: rgb(153, 255, 153) }"
				"QPushButton#Red { background-color: rgb(255, 178, 178) }"
				"QPushButton#Yellow { background-color: rgb(229, 229, 153) }"
				"QWidget#Black { background-color: black }";
	
	auto app = QApplication(argc, argv);
	app.setStyleSheet(stylesheet);
	
	auto p = app.palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	p.setColor(QPalette::WindowText, QColor(51, 51, 51));
	
	p.setColor(QPalette::Base, QColor(255, 255, 255));
	p.setColor(QPalette::AlternateBase, QColor(255, 255, 255));
	
	p.setColor(QPalette::PlaceholderText, QColor(51, 51, 51));
	p.setColor(QPalette::Text, QColor(51, 51, 51));
	
	p.setColor(QPalette::Disabled, QPalette::Button, QColor(170, 170, 170));
	p.setColor(QPalette::Active, QPalette::Button, QColor(204, 204, 204));
	p.setColor(QPalette::Inactive, QPalette::Button, QColor(204, 204, 204));
	p.setColor(QPalette::ButtonText, QColor(51, 51, 51));
	
	p.setColor(QPalette::Highlight, QColor(142, 45, 197));
	p.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
	app.setPalette(p);
	
	auto window = QScalarWindow();
	window.show();
	
	return app.exec();
}
