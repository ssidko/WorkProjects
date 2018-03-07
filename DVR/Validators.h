#pragma once

#ifndef _VALIDATORS_H
#define _VALIDATORS_H

#include <qvalidator.h>

class QUInt64Validator : public QValidator
{
private:
	int64_t value;
public:
	QUInt64Validator(QObject * parent = 0) : QValidator(parent) {}
	~QUInt64Validator() {}
	void fixup(QString &input) const override {}
	State validate(QString &input, int &pos) const override
	{
		bool ok = false;
		uint64_t value = static_cast<uint64_t>(input.toULongLong(&ok, 10));
		if (ok) {
			return QValidator::Acceptable;
		}
		else {
			return QValidator::Invalid;
		}
	}
};

#endif // _VALIDATORS_H
