#include <conio.h>

#include "Mbr.h"
#include "NtfsOnDiskStructures.h"
#include "W32Lib.h"

#include "h264.h"
#include "DvrExt3.h"
#include "MyLib.h"

int _tmain(int argc, _TCHAR* argv[])
{
	
	//FileEx disk(_T("\\\\.\\PhysicalDrive0"));
	//if (disk.Open()) {
	//	
	//	MBR mbr = {0};
	//	size_t size = sizeof(MBR);
	//	size = sizeof(BIOS_PARAMETER_BLOCK);
	//	disk.SetPointer(0);
	//	disk.Read((void *)&mbr, 512);
	//	for (int i = 0; i < MBR_MAX_PARTITIONS_COUNT; ++i) {
	//		if (mbr.partition[i].type == MBR_NTFS_PARTITION) {
	//			NTFS_BOOT_SECTOR bs = { 0 };
	//			disk.SetPointer(mbr.partition->offset * 512);
	//			size = sizeof(NTFS_BOOT_SECTOR);
	//			disk.Read((void *)&bs, 512);
	//			int x = 0;				
	//		}		
	//	}
	//	int x = 0;
	//}

	//dvrext::Run();

	//h264dvr::main(NULL, 0, NULL);

	MyLib::WinFile file(_T("test_file.bin"));
	if (file.Create(MyLib::iFile::kReadWrite)) {
		int x = 0;
		int y = 0;
	}

	_tprintf(_T("\nPress any key for exit ..."));
	_getch();

	return 0;
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

