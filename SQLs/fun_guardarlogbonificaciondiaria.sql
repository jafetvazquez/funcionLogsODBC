-- FUNCTION: public.fun_guardarlogbonificaciondiaria(integer, integer, text, date, integer, text)

-- DROP FUNCTION IF EXISTS public.fun_guardarlogbonificaciondiaria(integer, integer, text, date, integer, text);

CREATE OR REPLACE FUNCTION public.fun_guardarlogbonificaciondiaria(
	icliente integer,
	ifactura integer,
	tmensaje text,
	dfechacompra date,
	itienda integer,
	tfuente text)
    RETURNS TABLE(estado integer, mensaje text) 
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE PARALLEL UNSAFE
    ROWS 1000

AS $BODY$

/****************************************************************************************************************
* FECHA: 21-08-2025												*
* PROGRAMADOR: JAFET VAZQUEZ OLVERA									*
* DESCRIPCION: Graba logs sobre certificacion de bonificacion en la tabla logbonificaciondiari				*
****************************************************************************************************************/
DECLARE
	-- declarar fecha actual como date
	dfechaactual date;
BEGIN
	-- obtener fecha de gndominio
	SELECT fecha INTO dfechaactual FROM gndominio LIMIT 1;
	
    -- Insertar data
    INSERT INTO logBonificacionDiaria (
        cliente,
        factura,
        mensaje,
        fechacompra,
        fechaactual,
        tienda,
        fuente
    ) VALUES (
        icliente,
        ifactura,
        tmensaje,
        dfechacompra,
        dfechaactual,
        itienda,
        tfuente
    );

    -- retorna el valor 0 si la insercion es correcta
    RETURN QUERY SELECT 0 AS estado, 'Exito' AS mensaje;
    RETURN;

EXCEPTION
    WHEN OTHERS THEN
		-- retorna el valor -1 si la insercion es incorrecta
        RETURN QUERY SELECT -1 AS estado, 'Detalles: ' || SQLERRM AS mensaje;
        RETURN;
END;
$BODY$;

ALTER FUNCTION public.fun_guardarlogbonificaciondiaria(integer, integer, text, date, integer, text)
    OWNER TO postgres;
