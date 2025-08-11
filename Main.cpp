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
	
	const auto window = new QScalarWindow();
	window->show();
	return app.exec();
}
