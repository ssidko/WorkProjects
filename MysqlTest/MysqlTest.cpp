// MysqlTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////
//												MySQL Testing
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mysql_connection.h"
#include "mysql_driver.h"
	
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


using namespace std;

int sql_main(void)
{
	try {
		sql::Driver *driver = sql::mysql::get_driver_instance();

		sql::SQLString name = driver->getName();
		int mj_ver = driver->getMajorVersion();
		int min_ver = driver->getMinorVersion();
		int ptch_ver = driver->getPatchVersion();

		sql::SQLString host = "ssidko4";
		sql::SQLString user = "root";
		sql::SQLString pwd = "19820903";

		std::auto_ptr<sql::Connection> con(driver->connect(host, user, pwd));
		std::auto_ptr<sql::Statement> stmt(con->createStatement());

		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SHOW DATABASES"));
		size_t rows = res->rowsCount();
		std::cout << "Number of rows: " << rows << endl;

		sql::ResultSetMetaData *meta = res->getMetaData();

		sql::SQLString col_name = meta->getColumnLabel(1);

		std::cout << col_name << endl << endl;
		while (res->next())
		{
			std::cout << res->getString(col_name) << endl;
			int x = 0;
		}

		int x = 0;

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << "# ERROR: SQLException in file:" << endl << __FILE__ << endl;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		/* Use what() (derived from std::runtime_error) to fetch the error message */
		cout << "# ERROR: " << e.what() << endl;
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return EXIT_FAILURE;
	}

	_tprintf(_T("\nPress any key for exit ...")); _getch();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	return sql_main();
}

