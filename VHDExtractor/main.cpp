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
		//filters << "*.dat";
		QFileInfoList file_info_list = dir.entryInfoList(filters, QDir::Files);
		foreach(QFileInfo file_info, file_info_list) {
			file_list << QDir::toNativeSeparators(file_info.absoluteFilePath());
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
		tm *timeinfo = gmtime(&time);
		strftime(time_str, 128, "%Y-%m-%d %H-%M-%S", timeinfo);
		file_name.sprintf("%s\\%s [%06lld].h264", dir.toLocal8Bit().data(), time_str, counter);
		return file_name;
	}

	int NextFrameSequence(const QByteArray &buff, const unsigned int offset, unsigned int &size)
	{
		const int max_time_delta = 60*60;
		long long frame_counter = 0;
		long long min_frame_counter = 2;

		unsigned int finder_offset = 0;
		unsigned int start_frame = 0;
		unsigned int prev_frame = 0;
		unsigned int curr_frame = 0;
		FRAME_HEADER *header = NULL;
		unsigned int prev_time = 0;
		unsigned int curr_time = 0;
		
		size = 0;
		frame_counter = 0;
		finder_offset = offset;

		while ((curr_frame = FindFrame(buff, finder_offset)) != -1) {
			frame_counter++;
			header = (FRAME_HEADER *)&buff.data()[curr_frame];
			curr_time = header->ctime;
			if (frame_counter > 1) {
				if ((curr_time < prev_time) || ((curr_time - prev_time) > max_time_delta)) {
					// Последовательность фреймов нарушена.
					// Соответствующая обработка.
					if (frame_counter > min_frame_counter) {
						size = curr_frame - start_frame;
						return start_frame;
					} else {
						start_frame = curr_frame;
						frame_counter = 0;
					}
				}
			} else if (frame_counter == 1) {
				start_frame = curr_frame;
			} else if (frame_counter == 0) {
				// Защита от переполнения счётчика.
				frame_counter = 1;
			}
			prev_frame = curr_frame;
			prev_time = curr_time;
			finder_offset = curr_frame + 4;
		}
		if (frame_counter > min_frame_counter) {
			size = buff.size() - start_frame;
			return start_frame;
		}
		return -1;
	}

	void ConvertToMkv(QString &file_absolute_path, QString &out_dir_absolute_path)
	{
		QString app_path = QString::fromLocal8Bit("C:\\Program Files\\MKVToolNix\\mkvmerge.exe");
		QString command_line;
		QFileInfo file_info(file_absolute_path);
		command_line.sprintf("\"\"%s\" -o \"%s\\%s.mkv\" \"%s\"\"",
			app_path.toLocal8Bit().data(),
			out_dir_absolute_path.toLocal8Bit().data(),
			file_info.fileName().toLocal8Bit().data(),
			file_absolute_path.toLocal8Bit().data());
		QByteArray arr = command_line.toLocal8Bit();
		char *cmd_str = arr.data();
		system(cmd_str);			
	}

	void CutVideo(QString &src_file_path, QString &raw_out_dir, QString &converted_out_dir)
	{
		static ULONGLONG file_counter = 0;

		QFile src_file(src_file_path);
		QFile *dst_file = NULL;

		if (src_file.open(QIODevice::ReadOnly)) {

			QByteArray buff = src_file.readAll();
			unsigned int offset = 0;
			unsigned int sequence_size = 0;

			while((offset = NextFrameSequence(buff, offset + sequence_size, sequence_size)) != -1) {
				FRAME_HEADER *header = (FRAME_HEADER *)&buff.data()[offset];
				time_t time = (time_t)header->ctime;
				QString file_name = ComposeFileName(raw_out_dir, time, file_counter);
				QFile file(file_name);
				if (file.open(QIODevice::ReadWrite)){
					file.write(&buff.data()[offset], sequence_size);
					++file_counter;
					ConvertToMkv(file_name, converted_out_dir);
				}
				int x = 0;
			}
			int x = 0;
		}
	}

	void Run(QString &video_files_dir, QString &raw_out_dir, QString &converted_out_dir)
	{
		QStringList file_list = GetFileList(video_files_dir);
		foreach(QString file_path, file_list) {
			CutVideo(file_path, raw_out_dir, converted_out_dir);
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//dvr::Run(QString::fromLocal8Bit("H:\\"), QString::fromLocal8Bit("E:\\2-raw"), QString::fromLocal8Bit("E:\\2-mkv"));
	dvr::Run(QString::fromLocal8Bit("G:\\"), QString::fromLocal8Bit("E:\\tst"), QString::fromLocal8Bit("E:\\tst-mkv"));

	MainWindow w;
	w.show();

	return a.exec();
}
