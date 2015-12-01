#include "terminal.h"

Terminal::Terminal(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	Initialize();
}

Terminal::~Terminal()
{
	msg_reciver.terminate();
}

bool Terminal::Initialize(void)
{
	UpdateComPortsCombobox();
	connect(ui.open_button, SIGNAL(clicked()), this, SLOT(OnOpenButton()));
	connect(&msg_reciver, SIGNAL(MessageRecived(Message)), this, SLOT(OnMessageRecived(Message)), Qt::QueuedConnection/*Qt::DirectConnection*/);
	return true;
}

void Terminal::UpdateComPortsCombobox()
{
	std::list<std::string> com_list;
	ComPort::AvailableComPorts(com_list);
	auto it = com_list.begin();
	while (it != com_list.end()) {		
		ui.com_ports_combobox->addItem(QString::fromLocal8Bit((*it).c_str()));
		it++;
	}
}

void Terminal::OnOpenButton(void)
{
	ComPort com_port;
	QString com_name = ui.com_ports_combobox->currentText();
	ui.text_edit->insertPlainText("Com port opened.\n");

	msg_reciver.WaitForMessage(com_name.toLocal8Bit());

	//Message msg;
	//OnMessageRecived(msg);

}

void Terminal::OnMessageRecived(Message msg)
{
	QColor def_color = ui.text_edit->textColor();
	ui.text_edit->insertPlainText(QString::fromLocal8Bit("------"));
	ui.text_edit->setTextColor(QColor(Qt::darkGreen));
	ui.text_edit->insertPlainText(QString::fromLocal8Bit(" Message "));
	ui.text_edit->setTextColor(def_color);
	ui.text_edit->insertPlainText(QString::fromLocal8Bit("------") + "\n");

	ui.text_edit->insertPlainText(QString("  Header:\t%1").arg(QString::number(msg.header, 16)) + "\n");
	ui.text_edit->insertPlainText(QString("  Type:\t%1").arg(QString::number(msg.type)) + "\n");
	ui.text_edit->insertPlainText(QString("  Code:\t%1").arg(QString::number(msg.code)) + "\n");
	ui.text_edit->insertPlainText(QString("  Footer:\t%1").arg(QString::number(msg.footer, 16)) + "\n");
	ui.text_edit->insertPlainText(QString::fromLocal8Bit("------------------------") + "\n\n");
	//ui.text_edit->insertPlainText(QString::fromLocal8Bit("\n"));
}
