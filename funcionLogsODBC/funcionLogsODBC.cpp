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


bool conectarPostgreSQL(SQLHENV& hEnv, SQLHDBC& hDbc) {
    SQLRETURN ret;

    // 1. Allocate environment
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

    // 2. Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

    // 3. Build DSN-less connection string
    SQLCHAR connStr[] =
        "Driver={PostgreSQL Unicode};"
        "Server=localhost;"
        "Port=5432;"
        "Database=tienda0902;"
        "Uid=postgres;"
        "Pwd=123;";

    SQLCHAR outStr[1024];
    SQLSMALLINT outLen;

    ret = SQLDriverConnectA(hDbc, NULL, connStr, SQL_NTS, outStr, sizeof(outStr), &outLen, SQL_DRIVER_NOPROMPT);
    return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

void mostrarError(const std::string& funcion, SQLHANDLE handle, SQLSMALLINT tipoHandle) {
    SQLWCHAR estado[6], mensaje[256];
    SQLINTEGER codigo;
    SQLSMALLINT longitud;
    int i = 1;

    while (SQLGetDiagRec(tipoHandle, handle, i, estado, &codigo, mensaje, sizeof(mensaje), &longitud) == SQL_SUCCESS) {
        std::wstring estadoStr(estado); // Convertir SQLWCHAR a std::wstring
        std::wstring mensajeStr(mensaje); // Convertir SQLWCHAR a std::wstring

        std::wcerr << L"[ODBC Error] () "
            << L"SQLSTATE: " << estadoStr << L", Código: " << codigo
            << L", Mensaje: " << mensajeStr << std::endl;
        i++;
    }
}

// funcion para insertar logs con odbc sin checkerror
bool enviarLogParams(SQLHDBC hDbc, int cliente, int factura, const std::string& mensaje, const std::string& fechacompra, int tienda, const std::string& fuente) { //recibe 5 params
    SQLHSTMT hStmt; // statement sql
    SQLRETURN ret; // retorno de odbc

    // Crear statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    // validacion si se creó statement
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

    // Consulta parametrizada: SELECT * FROM fun_guardarlogbonificaciondiaria(?, ?, ?, ?, ?, ?)
    SQLCHAR query[] = "SELECT * FROM fun_guardarlogbonificaciondiaria(?, ?, ?, ?, ?, ?)";
    // preparar consulta
    ret = SQLPrepareA(hStmt, query, SQL_NTS);
    // validacion de preparacion de consulta
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "[Error] SQLPrepareA" << std::endl;
        return false;
    }

    // Bind de parámetros ordenados
    // param: cliente
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&cliente, 0, NULL);
    // param: factura
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&factura, 0, NULL);
    // param: mensaje
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, mensaje.size(), 0, (SQLPOINTER)mensaje.c_str(), 0, NULL);
    // param: fechacompra
    SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, fechacompra.size(), 0, (SQLPOINTER)fechacompra.c_str(), 0, NULL);
    // param: tienda
    SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&tienda, 0, NULL);
    // param: fuente
    SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, fuente.size(), 0, (SQLPOINTER)fuente.c_str(), 0, NULL);

    // Ejecutar
    ret = SQLExecute(hStmt);
    // validacion dfe ejecucion
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "[Error] SQLExecute" << std::endl;
        return false;
    }

    // Leer resultados si retorna algo
    char buffer[256];
    // indicador de longitud
    SQLLEN ind;

    // leer data de resultados
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        // ontener data de primer columna
        SQLGetData(hStmt, 1, SQL_C_CHAR, buffer, sizeof(buffer), &ind);
		// mostrar resultado
        std::cout << "Resultado: " << buffer << std::endl;
    }
    // liberar recursos
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    // retun existoso
    return true;
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
    SQLHENV hEnv;
    SQLHDBC hDbc;
    // Ejecucion de funciones
    std::cout << "Ejecutando main() " << std::endl;

    if (!conectarPostgreSQL(hEnv, hDbc)) {
        std::cerr << "Error al conectar a PostgreSQL." << std::endl;
        return 1;
    }

    //funcion con odbc
    //SQLHDBC hDbc = conexionODBC(L"localhost", L"postgres", L"postgres", L"123", 5432);
    //std::cout << hDbc << std::endl;
    //conectaODBC();

    //enviarLogNoError("Factura 9432456", 14, 8, 2025, 101);
    //enviarLogParams(conexion, cliente, factura, mesaje_log, fechacompra, tienda, fuente_log);
    enviarLogParams(hDbc, 10203, 240340, "este es el log....", "2025-08-20", 803, "este log viene de.....");
    system("pause");
    return 0; // Devuelve un código de éxito
}