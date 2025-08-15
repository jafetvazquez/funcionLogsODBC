#define ODBCVER 0x0351
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>

// data de db 
//const std::string CONNECTION_STRING = "dbname=postgres user=postgres password=123 host=localhost port=5432";


// chequeo de errores ODBC
void checkError(SQLRETURN ret, SQLHANDLE handle, SQLSMALLINT type, const std::wstring& msg) {
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR sqlState[1024];
        SQLWCHAR message[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;

        if (SQLGetDiagRecW(type, handle, 1, sqlState, &nativeError, message,
            sizeof(message) / sizeof(SQLWCHAR), &textLength) == SQL_SUCCESS) {
            std::wcerr << msg << L"\nSQLSTATE: " << sqlState
                << L"\nMessage: " << message << std::endl;
        }
        exit(1);
    }
}

// función para conectar a PostgreSQL usando ODBC
/*void conectaODBC() {
    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLRETURN ret;

    // Crear entorno ODBC
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error creando entorno ODBC");

    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error configurando versión ODBC");

    // Crear conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error creando conexión ODBC");

    //OPCIÓN 1: Usar DSN creado (cambia "MiPostgres" por tu DSN)
    //SQLWCHAR connStr[] = L"DSN=PostgreSQL35W;UID=postgres;PWD=123;";

    //OPCIÓN 2: Conexión directa sin DSN
    SQLWCHAR connStr[] = L"DRIVER={PostgreSQL Unicode};Server=localhost;Port=5432;Database=postgres;UID=postgres;PWD=123;";

    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error conectando a PostgreSQL via ODBC");

    std::wcout << L"Conexión ODBC a PostgreSQL exitosa" << std::endl;

    // Desconectar y liberar recursos
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}*/

/*SQLHDBC conexionODBC(const std::wstring& server, const std::wstring& db,
    const std::wstring& user, const std::wstring& password, int port) {
    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLRETURN ret;

    // Crear entorno
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error creando entorno ODBC");

    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error configurando versión ODBC");

    // Crear conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error creando conexión ODBC");

    // Cadena de conexión DSN-less
    std::wstring connStr =
        L"DRIVER={PostgreSQL Unicode(x64)};"
        L"SERVER=" + server +
        L";PORT=" + std::to_wstring(port) +
        L";DATABASE=" + db +
        L";UID=" + user +
        L";PWD=" + password + L";";

    ret = SQLDriverConnectW(hDbc, NULL,
        (SQLWCHAR*)connStr.c_str(), SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error conectando a PostgreSQL via ODBC");

    return hDbc; // Retorna conexión lista
}*/

// funcion para insertar logs con odbc sin checkerror
void enviarLogNoError(const std::string& mensaje, int dia, int mes, int anio, int tienda) { //recibe 5 params
    SQLHENV hEnv = SQL_NULL_HENV; // entorno odbc
    SQLHDBC hDbc = SQL_NULL_HDBC; // conexión a db
    SQLHSTMT hStmt = SQL_NULL_HSTMT; // statement SQL
    SQLRETURN ret; // retorno odbc

    // inicializar entorno ODBC
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    // version de odbc
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

    // conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    // SQLWCHAR con data db
    SQLWCHAR connStr[] = L"DRIVER={PostgreSQL Unicode};Server=localhost;Port=5432;Database=postgres;UID=postgres;PWD=123;";
    // establecer conexion
    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    // statement sql
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Preparar consulta parametrizada
    SQLWCHAR query[] = L"INSERT INTO logmsgs (mensaje, dia, mes, anio, tienda) VALUES (?, ?, ?, ?, ?)";
    ret = SQLPrepareW(hStmt, query, SQL_NTS);

    // Enlazar parámetros
	SQLWCHAR mensajeW[512]; // arreglo para mensaje en formato wide
    mbstowcs(mensajeW, mensaje.c_str(), mensaje.size() + 1); // convertir string a wstring

    // enlazar params por posicion
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, wcslen(mensajeW), 0, mensajeW, 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dia, 0, NULL);
    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &mes, 0, NULL);
    ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &anio, 0, NULL);
    ret = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &tienda, 0, NULL);

    // ejecutar consulta
    ret = SQLExecute(hStmt);

    // mensaje de guardado
    std::wcout << L"Log guardado: Mensaje='" << mensajeW
        << L"', Tienda=" << tienda
        << L", Fecha=" << dia << L"/" << mes << L"/" << anio << std::endl;

    // Liberar recursos odbc creados
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

// Insertar log en la base con ODBC
/*void enviarLog(const std::string& mensaje, int dia, int mes, int anio, int tienda) {
    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    // entorno ODBC
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    //checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error creando entorno ODBC");

    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    //checkError(ret, hEnv, SQL_HANDLE_ENV, L"Error configurando versión ODBC");

    // conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    //checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error creando conexión ODBC");

    // SQLWCHAR con data db
    SQLWCHAR connStr[] = L"DRIVER={PostgreSQL Unicode};Server=localhost;Port=5432;Database=postgres;UID=postgres;PWD=123;";

    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    //checkError(ret, hDbc, SQL_HANDLE_DBC, L"Error conectando a PostgreSQL via ODBC");

    // statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error creando statement");

    // Preparar consulta parametrizada
    SQLWCHAR query[] = L"INSERT INTO logmsgs (mensaje, dia, mes, anio, tienda) VALUES (?, ?, ?, ?, ?)";
    ret = SQLPrepareW(hStmt, query, SQL_NTS);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error preparando consulta");

    // Enlazar parámetros
    SQLWCHAR mensajeW[512];
    mbstowcs(mensajeW, mensaje.c_str(), mensaje.size() + 1); // convertir string a wstring

    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        wcslen(mensajeW), 0, mensajeW, 0, NULL);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error vinculando mensaje");

    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dia, 0, NULL);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error vinculando dia");

    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &mes, 0, NULL);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error vinculando mes");

    ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &anio, 0, NULL);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error vinculando anio");

    ret = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &tienda, 0, NULL);
    //checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error vinculando tienda");

    // Ejecutar
    ret = SQLExecute(hStmt);
    checkError(ret, hStmt, SQL_HANDLE_STMT, L"Error ejecutando consulta");

    std::wcout << L"Log guardado en PostgreSQL (ODBC): Mensaje='" << mensajeW
        << L"', Tienda=" << tienda
        << L", Fecha=" << dia << L"/" << mes << L"/" << anio << std::endl;

    // Liberar recursos
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}*/

int main() {
    // Ejecucion de funciones
    std::cout << "Ejecutando main() " << std::endl;


    //funcion con odbc
    //SQLHDBC hDbc = conexionODBC(L"localhost", L"postgres", L"postgres", L"123", 5432);
    //std::cout << hDbc << std::endl;
    //conectaODBC();

    enviarLogNoError("Factura 9432456", 14, 8, 2025, 101);

    return 0; // Devuelve un código de éxito
}