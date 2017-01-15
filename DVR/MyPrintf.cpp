#include <cstdarg>
#include <cctype>
#include "MyPrintf.h"

struct FormatSpecifier {
	int zero_pad;
	int width;
	int precision;
	char specifier;
};

int StrToInt(char *&str)
{
	int value = 0;
	size_t len = 0;

	while(*str && isdigit(*str)) {
		++len;
		++str;
	}

	str -= len;

	int tenth = 1;
	for (int i = len - 1; i >= 0; i--) {
		value += (str[i] - '0') * tenth;
		tenth *= 10;
	}

	str += len;

	return value;
}

//
// A format specifier follows this prototype:
// %[zero_pad][width][.precision]specifier
//
bool ReadFormatSpecifier(char *&format, FormatSpecifier &specifier)
{

	if (*format && *format == '0') {
		specifier.zero_pad = 1;
		format++;
	}

	if (*format && std::isdigit(*format)) {
		specifier.width = StrToInt(format);		
	}	

	if (*format && (*format == '.')) {
		format++;
		specifier.precision = StrToInt(format);
	}

	if (*format && *format == '%') {
		format++;
		specifier.specifier = '%';
		return true;
	} 
	
	if (*format && std::isalpha(*format)) {
		specifier.specifier = *format;
		format++;
		return true;
	}

	return false;
}

int MyPrintf(char *buffer, size_t size, char *format, ...)
{
	int out_idx = 0;

	va_list args;
	va_start(args, format);

	while (*format) {
		
		if (*format != '%') {
			//buffer[out_idx++] = *format++;
			*buffer++ = *format++;
			continue;
		}

		char *substr;
		FormatSpecifier format_specifier = {0};
		if (ReadFormatSpecifier(++format, format_specifier)) {
		
			switch (format_specifier.specifier) {
			case 'd':
				break;			
			case 's':
				substr = va_arg(args, char *);

				while (*substr) {					
					//buffer[out_idx++] = *substr++;
					*buffer++ = *substr++;
				}				


				break;
			case '%':
				buffer[out_idx++] = '%';
				break;
			case 'f':
				break;
			default:
				break;			
			}		
		}
	
	}

	va_end(args);
	return 0;
}