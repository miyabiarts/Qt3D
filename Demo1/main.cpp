#include <QApplication>
#include <QMainWindow>
#include "Viewport.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Viewport view;
	view.show();

	return a.exec();
}
