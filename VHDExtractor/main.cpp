#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "DiskImageExtractor.h"
#include "BitMap.h"

#include <QDir>
#include <QFileInfo>

#include <time.h> 

namespace dvr
{
	typedef struct _DATE_TIME {
		DWORD sec:6;
		DWORD min:6;
		DWORD hour:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;
	} DATE_TIME;

#define FRAME_HEADER_SIGNATURE				0x534D4248 //'HBMS'

	typedef struct _FRAME_HEADER {
		unsigned int signature;
		unsigned int unk1;
		unsigned int unk2;
		unsigned int ctime;
	} FRAME_HEADER;

	QStringList GetFileList(QString &path)
	{
		QDir dir(path);
		QStringList file_list;
		QStringList filters;
		filters << "*.dat";
		QFileInfoList file_info_list = dir.entryInfoList(filters, QDir::Files);
		foreach(QFileInfo file_info, file_info_list) {
			file_list << file_info.absoluteFilePath ();
		}
		return file_list;
	}

	int FindFrame(const QByteArray &buff, const unsigned int offset)
	{
		int *dw = (int *)(&buff.data()[offset]);
		while ((int)(dw + 4) <= (int)(buff.data() + buff.size())) {
			if (*dw == FRAME_HEADER_SIGNATURE) {
				return (int)((char *)dw - buff.data());
			}
			++dw;
		}
		return -1;
	}

	QString ComposeFileName(const QString &dir, const time_t &time, const ULONGLONG &counter)
	{
		QString file_name;
		char time_str[128] = {0};
		tm *timeinfo = localtime(&time);
		strftime(time_str, 128, "%Y-%m-%d %H:%M:%S", timeinfo);
		file_name.sprintf("%s/%s [%06lld].h264", dir.toLocal8Bit().data(), time_str, counter);
		return file_name;
	}

	int NextFrameSequence(const QByteArray &buff, const unsigned int offset, unsigned int &size)
	{
		const int max_time_delta = 5;
		unsigned int frame_offset = FindFrame(buff, 0);
		FRAME_HEADER *header = (FRAME_HEADER *)&buff.data()[frame_offset];
		time_t start_time = (time_t)header->ctime;
		while (frame_offset != -1)
		{
			
		}
	
		return -1;
	}

	void CutVideo(QString &src_file_path, QString &out_dir)
	{
		static ULONGLONG file_counter = 0;

		QFile src_file(src_file_path);
		QFile *dst_file = NULL;

		if (src_file.open(QIODevice::ReadOnly)) {
			QByteArray buff = src_file.readAll();
			unsigned int frame_offset = FindFrame(buff, 0);

			while((frame_offset = FindFrame(buff, frame_offset)) != -1) {
				FRAME_HEADER *header = (FRAME_HEADER *)&buff.data()[frame_offset];
				time_t time = (time_t)header->ctime;
				// YYYY-MM-DD HH-MM-SS [counter]
				char *str = ctime(&time);

				QString time_str = ComposeFileName(out_dir, time, file_counter);

				int x = 0;
			}
		}
	}

	void Run(QString &video_files_dir, QString &out_dir)
	{
		QStringList file_list = GetFileList(video_files_dir);
		foreach(QString file_path, file_list) {
			CutVideo(file_path, out_dir);
		}		
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	dvr::Run(QString::fromLocal8Bit("G:/"), QString::fromLocal8Bit("E:/36183-36184"));

	return a.exec();
}
