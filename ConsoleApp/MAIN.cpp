#include <conio.h>
#include "Mbr.h"
#include "NtfsOnDiskStructures.h"
#include "W32Lib.h"

#include "h264.h"
#include "DvrExt3.h"
#include "WinFile.h"
#include <map>
#include <fstream>

//#include <stdio.h>
//#include <windows.h>
//#include <TCHAR.H>

#include "Experemental.h"

#include "sqliter.h"
#include "mover.h"

void recovery_38615()
{
	using namespace sqliter;

	Record rec;
	//const FIELD *field = NULL;
	//const STRING_FIELD *str_field = NULL;
	//const INTEGER_FIELD *int_field = NULL;

	//multimap<time_t, string> messages;

	std::map<ULONGLONG, std::pair<string, string>> map;

	SQLiter db(_T("D:\\Work\\38615\\contacts2.db"));
	FileEx out_file(_T("D:\\Work\\38615\\out.txt"));
	int count = 0;
	if (db.Open() && out_file.Create()) {

		for (DWORD i = 1; i <= db.PagesCount(); i++) {
			Page *page = db.GetPage(i);

			if (page && (page->Type() == kLeafTablePage)) {
				DWORD records_count = page->RecordsCount();
				for (DWORD r = 0; r < records_count; r++) {
					if (page->GetRecord(r, &rec)) {

						//string out_line = "";
						//string chat_name = "";
						//string message_text = "";
						//string author = "";
						//string dialog_partner;
						//string time = "";
						//time_t raw_time = 0;

						ULONGLONG contact_id = 0;
						string phone_number = "";
						string name = "";

						if (rec.FieldsCount() == 5) {
							if ((rec[0]->type == kInteger) && (rec[1]->type == kInteger) && (rec[2]->type == kString) && (rec[3]->type == kString) && (rec[2]->type == kString)) {
								contact_id = ((INTEGER_FIELD *)rec[1])->val;
								phone_number = ((STRING_FIELD *)rec[2])->val;

								auto it = map.find(contact_id);
								if (it == map.end()) {
									map.insert(std::make_pair(contact_id, std::make_pair("NoName", phone_number)));
								}
								else {
									if (!it->second.second.empty()) {
										it->second.second += ",";
									}
									it->second.second += phone_number;
								}
								 
	
								//count++;
							}
						} else if (rec.FieldsCount() == 36) {

							if ((rec[7]->type == kInteger) && (rec[16]->type == kString) && (rec[17]->type == kString) && (rec[18]->type == kInteger)) {

								contact_id = ((INTEGER_FIELD *)rec[7])->val;
								name = ((STRING_FIELD *)rec[16])->val;

								auto it = map.find(contact_id);
								if (it == map.end()) {
									map.insert(std::make_pair(contact_id, std::make_pair(name, "")));
								}
								else {
									it->second.first = name;
								}

								//count++;
							}
							int x = 0;
							//count++;
						} 

						//if (rec[CHAT_NAME_FIELD]->type != kString) continue;
						//chat_name = ((STRING_FIELD *)rec[CHAT_NAME_FIELD])->val;

						//if ((chat_name.find("oleg_anshlag") == string::npos) || (chat_name.find("babay.andr") == string::npos)) continue;

						//if (((INTEGER_FIELD *)rec[MESSAGE_TYPE_FIELD])->val != kText) continue;

						//if (rec[MESSAGE_TEXT_FIELD]->type != kString) continue;
						//message_text = ((STRING_FIELD *)rec[MESSAGE_TEXT_FIELD])->val;

						//if (rec[AUTHOR_FIELD]->type != kString) continue;
						//author = ((STRING_FIELD *)rec[AUTHOR_FIELD])->val;

						//if (rec[DIALOG_PARTNER_FIELD]->type != kString) continue;
						//dialog_partner = ((STRING_FIELD *)rec[DIALOG_PARTNER_FIELD])->val;

						//if (rec[TIMESTAMP_FIELD]->type != kInteger) continue;
						//raw_time = (time_t)((INTEGER_FIELD *)rec[TIMESTAMP_FIELD])->val;
						//memset(time_string, 0x00, sizeof(time_string));
						//ctime_s(time_string, sizeof(time_string), &raw_time);
						//time = time_string;

						//time[time.size() - 1] = ' ';

						//out_line += time;
						//out_line += "- ";
						//out_line += author;

						//out_line += "(";
						//if (author.find(dialog_partner) == string::npos) {
						//	out_line += dialog_partner;
						//} else {
						//	out_line += "oleg_anshlag";
						//}
						//out_line += ")";

						//out_line += ": ";
						//out_line += message_text;
						//out_line += "\r\n";

						//messages.insert(pair<time_t, string>(raw_time, out_line));

						//out_file.Write((void *)out_line.c_str(), out_line.size());
						int x = 0;
					}
				}
			}
			if (page) {
				delete page;
				page = NULL;
			}
		}
	}
	
	std::string file_name = "D:\\Work\\38615\\out.txt";
	std::fstream file(file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	if (file.is_open()) {

		auto it = map.begin();
		while (it != map.end()) {
			if (!it->second.second.empty()) {
				file << it->second.first << "," << it->second.second << "\n";
			}
			it++;
		}
		file.flush();
	}
	file.close();

	//multimap<time_t, string>::iterator it;
	//for (it = messages.begin(); it != messages.end(); ++it) {
	//	out_file.Write((void *)((*it).second.c_str()), (*it).second.size());
	//}
	
}

#include "RarRecovery.h"
#include "ZipRec.h"
#include <list>

int mov_fix(void)
{
	W32Lib::FileEx idx_file("G:\\stco.bin");
	W32Lib::FileEx mov_file("G:\\48_2.mov");
	size_t cluster_size = 32768;

	size_t seq_len = 0;
	size_t bad_offset = 0;
	size_t bad_idx = 0;
	uint32_t correct = 0;
	bool corrected = false;

	std::list<uint32_t> result;

	if (idx_file.Open() && mov_file.Open()) {

		size_t count = 10904;
		std::vector<uint32_t> table;
		table.resize(count);
		idx_file.SetPointer(16);
		size_t readed = idx_file.Read((uint8_t *)table.data(), table.size() * 4);

		for (uint32_t idx = 0; idx < count; idx++) {

			uint32_t offset = Be2Le((DWORD *)&table[idx]) + correct;
			uint32_t sign_be = 0;

			mov_file.SetPointer(offset);
			readed = mov_file.Read(&sign_be, 4);

			uint32_t sign = Be2Le((DWORD *)&sign_be);

			if (sign > 0x000000ff) {
				int x = 0;
				seq_len++;
				if (seq_len == 1) {
					bad_offset = offset;
					bad_idx = idx;
				}
			} else {
				seq_len = 0;
			}

			if (seq_len > 5) {
				for (int j = 0; j < 5; j++) {
					bad_offset = Be2Le((DWORD *)&table[bad_idx + j]) + correct;
					for (int i = 1; i < 32; i++) {

						mov_file.SetPointer(bad_offset + i*cluster_size);
						readed = mov_file.Read(&sign_be, 4);

						sign = Be2Le((DWORD *)&sign_be);

						if (sign < 0xff) {
							std::cout << "BAD at: " << std::to_string(bad_offset) << std::string("  Cluster: ") + std::to_string(bad_offset / cluster_size) << std::endl;
							std::cout << "skip: " << std::to_string(i * cluster_size) << std::endl;
							idx = bad_idx;
							correct += i*cluster_size;
							corrected = true;
							break;
						}
					}
					if (corrected) {
						break;
					}
				}

				if (!corrected) {
					std::cout << "Mission failed" << std::endl;
					::system("pause");
					return false;
				}
				else {
					corrected = false;
				}

				int x = 0;
			}
		}
	}

	::system("pause");
	return 0;
}

#pragma pack(push,1)
struct ploop_pvd_header
{
	uint8_t  signature[16];
	uint32_t version;		// 2
	uint32_t heads;	
	uint32_t cylinders;	
	uint32_t cluster_size;
	uint32_t bat_entries;	
	union {
		struct {
			uint32_t SizeInSectors_v1;
			uint32_t Unused;
		};
		uint64_t SizeInSectors_v2;
	};
	uint32_t disk_in_use;
	uint32_t data_offset;
	uint32_t flags;
	uint8_t  reserved[8];
};
#pragma pack(pop)

#include <thread>
#include <chrono>
#include <type_traits>


void func(int value)
{
	std::cout << "Hello from thread." << std::endl;
	std::cout << "value: " << value << std::endl;

	for (int i = 0; i < 100; ++i) {
		std::cout << "Sleep for 1 second. " << "Try: " << i <<std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));	
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//std::cout << std::endl << "---=== Physical Drives ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_DISK);
	//std::cout << std::endl << "---=== CD-ROMs ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_CDROM);
	//std::cout << std::endl << "---=== COM Ports ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_COMPORT);
	
	//RarRecoveryMain(argc, argv);

	//TestZipRec();
	//ZipRec_Main(argc, argv);

	ploop_pvd_header *hdr = nullptr;
	size_t sz = sizeof(ploop_pvd_header);

	std::thread thr(func, 13);
	thr.join();

	TestZipRec();

	//ExamineFile("D:\\Soft\\#RecoverySoft#\\X-Ways\\xw_forensics170.rar");

	::system("pause");

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//												MySQL Testing
//////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "mysql_connection.h"
//#include "mysql_driver.h"
//	
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
//#include <cppconn/prepared_statement.h>

//int sql_main(void)
//{
//	try {
//		sql::Driver *driver = sql::mysql::get_driver_instance();
//
//		sql::SQLString name = driver->getName();
//		int mj_ver = driver->getMajorVersion();
//		int min_ver = driver->getMinorVersion();
//		int ptch_ver = driver->getPatchVersion();
//
//		sql::SQLString host = "ssidko4";
//		sql::SQLString user = "root";
//		sql::SQLString pwd = "19820903";
//
//		std::auto_ptr<sql::Connection> con(driver->connect(host, user, pwd));
//		std::auto_ptr<sql::Statement> stmt(con->createStatement());
//
//		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SHOW DATABASES"));
//		size_t rows = res->rowsCount();
//		std::cout << "Number of rows: " << rows << endl;
//
//		sql::ResultSetMetaData *meta = res->getMetaData();
//
//		sql::SQLString col_name = meta->getColumnLabel(1);
//
//		std::cout << col_name << endl << endl;
//		while (res->next())
//		{
//			std::cout << res->getString(col_name) << endl;
//			int x = 0;
//		}
//
//		int x = 0;
//
//	} catch (sql::SQLException &e) {
//		/*
//		The MySQL Connector/C++ throws three different exceptions:
//
//		- sql::MethodNotImplementedException (derived from sql::SQLException)
//		- sql::InvalidArgumentException (derived from sql::SQLException)
//		- sql::SQLException (derived from std::runtime_error)
//		*/
//		cout << "# ERROR: SQLException in file:" << endl << __FILE__ << endl;
//		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
//		/* Use what() (derived from std::runtime_error) to fetch the error message */
//		cout << "# ERROR: " << e.what() << endl;
//		cout << " (MySQL error code: " << e.getErrorCode();
//		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
//		return EXIT_FAILURE;
//	}
//
//	return 0;
//}

