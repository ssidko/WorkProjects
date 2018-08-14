import pyodbc

print (pyodbc.drivers())

try:
    conn = pyodbc.connect(
        r'DRIVER={ODBC Driver 13 for SQL Server};'
        r'SERVER=localhost\MSSQLLocalDB'
        r'UID=epos_main\ssidko;'
        r'PWD=ssidko2834'
        )
except BaseException as exc:
    print(exc)

print("Finish")
