import struct
import pymssql


SQL_HEADER = "use zp2010_fromBackup\n" \
             "go\n\n" \

#QUERY  = "DROP TABLE db2010_fromBackup.dbo.{0}\n"\
#         "SELECT * INTO db2010_fromBackup.dbo.{0} FROM db2010.dbo.{0}"

QUERY = "IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = '{0}') DROP TABLE dbo.{0} SELECT * INTO zp2010_fromBackup.dbo.{0} FROM zp2010.dbo.{0}\n"


def test_sql_server():
    server = r"10.0.0.106\MSSQLSERVER"
    user = "NT Service\MSSQLSERVER"
    password = "123"

    conn = pymssql.connect(server, user, password, "testDB")
    cursor = conn.cursor()
    cursor.execute("""
    IF OBJECT_ID('persons', 'U') IS NOT NULL
        DROP TABLE persons
    CREATE TABLE persons (
        id INT NOT NULL,
        name VARCHAR(100),
        salesrep VARCHAR(100),
        PRIMARY KEY(id)
    )
    """)



    pass

if __name__ == "__main__":

    tst = {24:23}

    tst["str"] = 56
    tst[67] = "string"

    test_sql_server()

    table_list_file = r"\\10.0.0.108\incoming\44570\zp2010.txt"
    sql_file = r"\\10.0.0.108\incoming\44570\zp2010_copy_all_tables.sql"

    with open(table_list_file, "r") as tables_file:
        with open(sql_file, "w") as out_file:
            out_file.write(SQL_HEADER)

            for line in tables_file:
                line = line.rstrip()
                out_file.write(QUERY.format(line))

    print("--== Finish ==--")