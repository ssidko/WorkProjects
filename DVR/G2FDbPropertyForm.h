#pragma once
#ifndef G2FDBPROPERYFORM_H
#define G2FDBPROPERYFORM_H
#include <QWidget>
#include "ui_G2FDbPropertyForm.h"

class G2FDbPropertyForm : public QWidget
{
	Q_OBJECT
private:
	Ui::G2FDbPropertyForm ui;

public:
	G2FDbPropertyForm(QWidget * parent = Q_NULLPTR);
	~G2FDbPropertyForm();

private:
	
};

#endif // G2FDBPROPERYFORM_HPP