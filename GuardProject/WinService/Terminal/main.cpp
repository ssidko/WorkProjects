#include "terminal.h"
#include <QtWidgets/QApplication>

#include "ComPort.h"
#include "MessageReciver.h"

#include "MasterModule.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Terminal w;

	//Message msg;
	//MasterModule master;
	//if (master.Open("\\\\.\\COM6")) {
	//	if (master.Identify()) {
	//		while (master.WaitForMessage(msg)) {
	//			int x = 0;
	//		}	
	//		int x = 0;
	//	}
	//	else {
	//		int y = 0;
	//	}
 //	}

	w.show();
	return a.exec();
}
