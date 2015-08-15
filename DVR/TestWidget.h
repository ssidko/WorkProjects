#pragma once
#ifndef _TEST_WIDGET_H
#define _TEST_WIDGET_H

#include <QAbstractScrollArea>

class TestWidget : public QAbstractScrollArea
{
public:
	TestWidget(QObject *parent = NULL) {}
	~TestWidget() {}
protected:
	void paintEvent(QPaintEvent * event);
};

#endif // _TEST_WIDGET_H

