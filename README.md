# funcionLogsODBC
## Pasos a seguir
1. instalar el driver psqlODBC - https://odbc.postgresql.org/
2. ejecutar msi
3. crear DSN en psqlODBC (no es necesario)
<img width="536" height="295" alt="image" src="https://github.com/user-attachments/assets/6a4a6082-50a7-46e3-8380-7ad3cfc7bcd2" />



4. enlazar proyecto c++
   - #include <sql.h>
   - #include <sqlext.h>

5. agregar odbc32.lib a:
   - Linker → Input → Additional Dependencies. en solución c++
6. usar dsn-less si no se creo un DSN para la conexión
   - SQLWCHAR connStr[] = L"DRIVER={PostgreSQL Unicode};Server=localhost;Port=5432;Database=postgres;UID=postgres;PWD=123;";
