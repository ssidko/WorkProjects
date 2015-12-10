#include <conio.h>
#include "Mbr.h"
#include "NtfsOnDiskStructures.h"
#include "W32Lib.h"

#include "h264.h"
#include "DvrExt3.h"
#include "WinFile.h"

//#include <stdio.h>
//#include <windows.h>
//#include <TCHAR.H>

#include "Experemental.h"

#include "sqliter.h"

void recovery_38615()
{
	using namespace sqliter;

	Record rec;
	//const FIELD *field = NULL;
	//const STRING_FIELD *str_field = NULL;
	//const INTEGER_FIELD *int_field = NULL;

	//multimap<time_t, string> messages;

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

						unsigned long long contact_id = 0;
						string phone_number = "";

						if (rec.FieldsCount() == 5) {
							if ((rec[0]->type == kInteger) && (rec[1]->type == kInteger) && (rec[2]->type == kString) && (rec[3]->type == kString) && (rec[2]->type == kString)) {
								contact_id = ((INTEGER_FIELD *)rec[1])->val;;
								phone_number = ((STRING_FIELD *)rec[2])->val;
								int x = 0;
								count++;
							}
						} else if (rec.FieldsCount() == 36) {
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


	int z = 0;

	//multimap<time_t, string>::iterator it;
	//for (it = messages.begin(); it != messages.end(); ++it) {
	//	out_file.Write((void *)((*it).second.c_str()), (*it).second.size());
	//}
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	//std::cout << std::endl << "---=== Physical Drives ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_DISK);
	//std::cout << std::endl << "---=== CD-ROMs ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_CDROM);
	//std::cout << std::endl << "---=== COM Ports ===---" << std::endl;
	//EnumerateDevicesInterfaces(&GUID_DEVINTERFACE_COMPORT);

	recovery_38615();

	std::cout << "Press any key..." << std::endl;
	_getch();
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

