#pragma once
#ifndef _TEST_WIDGET_H
#define _TEST_WIDGET_H

#include <QFile>
#include <QAbstractScrollArea>

class TestWidget : public QAbstractScrollArea
{
private:
	QFile *file;
	int rows;
	int columns;
public:
	TestWidget(QObject *parent = NULL);
	~TestWidget() {}
	void Initialize(void);
	void resizeEvent(QResizeEvent *event);
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // _TEST_WIDGET_H

