#ifndef TERMINAL_H
#define TERMINAL_H

#include <QtWidgets/QMainWindow>
#include "ui_terminal.h"
#include "MessageReciver.h"

class Terminal : public QMainWindow
{
	Q_OBJECT
private:
	Ui::TerminalClass ui;
	MessageReciver *msg_reciver;
public:
	Terminal(QWidget *parent = 0);
	~Terminal();
	bool Initialize(void);
	void UpdateComPortsCombobox();
public slots:
	void OnOpenButton(void);
};

#endif // TERMINAL_H
