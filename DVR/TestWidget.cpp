#include "TestWidget.h"
#include <QPainter>
#include <QScrollBar>

TestWidget::TestWidget(QObject *parent /*= NULL*/) : file(nullptr), rows(0), columns(0)
{
	Initialize();
}

void TestWidget::Initialize(void)
{
	resize(570, 481);
	file = new QFile(QString::fromLocal8Bit("D:\\Work\\37601\\37601.pst"));
	if (file) {
		qint64 size = file->size();
		verticalScrollBar()->setRange(0, (int)file->size());		
	}

	verticalScrollBar()->setSingleStep(16);
	verticalScrollBar()->setPageStep(3*16);
}

void TestWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(viewport());
	QRect rect = viewport()->geometry();

	int margin = 10;
	int margin_2 = 5;

	QString info_string;
	info_string.sprintf("Viewport size: width = %d, height = %d. Scorll position: %d.", rect.width(), rect.height(), verticalScrollBar()->sliderPosition());

	QRect info_rect(margin, margin, (rect.width() - (2*margin)), 20);
	painter.fillRect(info_rect, QColor(220, 220, 100));
	painter.drawRect(info_rect);
	painter.drawText(info_rect, Qt::AlignCenter, info_string);

	QRect h_offsets_rect(margin, info_rect.y() + info_rect.height() + margin_2, (rect.width() - (2 * margin)), 20);
	painter.fillRect(h_offsets_rect, QColor(220, 220, 100));
	painter.drawRect(h_offsets_rect);

	QRect v_offsets_rect(margin, h_offsets_rect.y() + h_offsets_rect.height() + margin_2, 120, rect.height() - h_offsets_rect.bottom() - 2*margin);
	painter.fillRect(v_offsets_rect, QColor(220, 220, 100));
	painter.drawRect(v_offsets_rect);

	QRect hex_rect(v_offsets_rect.x() + v_offsets_rect.width() + margin_2, v_offsets_rect.top(),
		rect.width() - (v_offsets_rect.right() + 1 + margin + margin_2),
		rect.height() - h_offsets_rect.bottom() - 2 * margin);
	//painter.fillRect(hex_rect, QColor(220, 220, 100));
	painter.drawRect(hex_rect);


	//painter.drawText(10, 10, "Machine " + QString::number(100));

	int counter = verticalScrollBar()->sliderPosition();
	int width = 20;
	int height = 15;
	int interval = 5;
	QRect r(0, 0, width, height);

	painter.setPen(Qt::black);
	painter.setFont(QFont("Courier", 12));
	//painter.setFont(QFont("Courier", 12, QFont::Bold));

	int column_counter = 0;
	r.moveTo(hex_rect.x() + margin_2, h_offsets_rect.y()+2);
	while ((r.width() + r.x() + margin_2) <= h_offsets_rect.right()) {
		QString s;
		s.sprintf("%02X", (column_counter % 256));
		//painter.fillRect(r, Qt::white);
		painter.drawText(r, Qt::AlignCenter, s);
		//painter.drawRect(r);
		r.moveTo(r.x() + r.width() + interval, r.y());
		++column_counter;
	}

	//painter.setFont(QFont("Courier", 12, QFont::Bold));
	r.moveTo(hex_rect.x() + margin_2, hex_rect.y() + margin_2);
	rows = 0;
	while ((r.height() + r.y() /*+ margin_2*/) <= hex_rect.bottom()) {
		columns = 0;
		while ((r.width() + r.x() + margin_2) <= hex_rect.right()) {
			//if (counter % 2) {
			//	painter.fillRect(r, Qt::green);
			//} else {
			//	painter.fillRect(r, Qt::red);
			//}

			QString s;
			s.sprintf("%02X", (counter % 256));
			//painter.fillRect(r, Qt::white);
			/*painter.drawText(r, Qt::AlignCenter, QString::number(counter % 256, 16).toUpper());*/
			painter.drawText(r, Qt::AlignCenter, s);
			//painter.drawRect(r);

			r.moveTo(r.x() + r.width() + interval, r.y());
			++counter;
			++columns;
		}
		r.moveTo(hex_rect.x() + margin_2, r.y() + r.height() + interval);
		++rows;
	}

	int x = 0;

}

void TestWidget::resizeEvent(QResizeEvent *event)
{

}
