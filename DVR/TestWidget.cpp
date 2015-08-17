#include "TestWidget.h"
#include <QPainter>
#include <QScrollBar>

TestWidget::TestWidget(QObject *parent /*= NULL*/)
{
	Initialize();
}

void TestWidget::Initialize(void)
{
	this->verticalScrollBar()->setRange(0, 1000);
}

void TestWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(viewport());
	QRect rect = viewport()->geometry();

	painter.fillRect(0, 0, rect.width(), rect.height(), QColor(220, 220, 100));

	painter.drawText(10, 10, "Machine " + QString::number(100));

	int counter = 0;
	int width = 16;
	int height = 15;
	int interval = 2;
	QRect r(0, 0, width, height);

	painter.setPen(Qt::black);
	r.moveTo(10, 30);
	//for (int j = 0; j < 50; j++) {
	while ((r.height() + r.y() + 5) <= rect.height()) {
		//for (int i = 0; i < 100; i++) {
		while ((r.width() + r.x() + 5) <= rect.width()) {
			//if (counter % 2) {
			//	painter.fillRect(r, Qt::green);
			//} else {
			//	painter.fillRect(r, Qt::red);
			//}

			//painter.fillRect(r, Qt::white);

			painter.drawText(r, Qt::AlignCenter, QString::number(counter % 256, 16).toUpper());

			painter.drawRect(r);
			r.moveTo(r.x() + r.width() + interval, r.y());
			++counter;
		}
		r.moveTo(10, r.y() + r.height() + interval);
	}


}
