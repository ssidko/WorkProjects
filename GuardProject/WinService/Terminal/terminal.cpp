#include "terminal.h"

Terminal::Terminal(QWidget *parent) : QMainWindow(parent), msg_reciver(nullptr)
{
	ui.setupUi(this);
	Initialize();
}

Terminal::~Terminal()
{

}

bool Terminal::Initialize(void)
{
	UpdateComPortsCombobox();
	connect(ui.open_button, SIGNAL(clicked()), this, SLOT(OnOpenButton()));
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
	QString com_name = ui.com_ports_combobox->currentText();
	ui.text_edit->insertPlainText(com_name + "\n");
}
