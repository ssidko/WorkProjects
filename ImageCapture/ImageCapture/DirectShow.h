#pragma once
#include <QString>
#include <QList>

class DirectShow
{
public:
	DirectShow(void);
	~DirectShow(void);

	void Test();
	void AvailableDevices(QList<QString> &list);
};

