# funcionLogsODBC
## Pasos a seguir
usar dsn-less si no se creo un DSN para la conexión (datos de db)

SQLCHAR connStr[] =
"Driver={PostgreSQL Unicode};"
"Server=localhost;"
"Port=5432;"
"Database=tienda0902;"
"Uid=postgres;"
"Pwd=123;";

FUNCIONES PRINCIPALES:
   - enviarLogParams(conexion, cliente, factura, mesaje_log, fechacompra, tienda, fuente_log);
   - conectarPostgreSQL (solo de manera local)
