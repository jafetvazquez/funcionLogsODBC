-- FUNCTION: public.fun_guardarlogbonificaciondiaria(bigint, integer, date, date, integer, character varying)

-- DROP FUNCTION IF EXISTS public.fun_guardarlogbonificaciondiaria(bigint, integer, date, date, integer, character varying);

CREATE OR REPLACE FUNCTION public.fun_guardarlogbonificaciondiaria(
	p_cliente bigint,
	p_factura integer,
	p_fecha_compra date,
	p_fecha_actual date,
	p_tienda integer,
	p_fuente character varying)
    RETURNS TABLE(estado integer, mensaje text) 
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE PARALLEL UNSAFE
    ROWS 1000

AS $BODY$
BEGIN
    -- Verificar si la factura ya existe
    IF EXISTS (
        SELECT 1 FROM logBonificacionDiaria WHERE factura = p_factura
    ) THEN
        RETURN QUERY SELECT -1 AS estado, 'La factura ya existe.' AS mensaje;
        RETURN;
    END IF;

    -- Insertar si no hay duplicado
    INSERT INTO logBonificacionDiaria (
        cliente,
        factura,
        fechaCompra,
        fechaActual,
        tienda,
        fuente
    )
    VALUES (
        p_cliente,
        p_factura,
        p_fecha_compra,
        p_fecha_actual,
        p_tienda,
        p_fuente
    );

    RETURN QUERY SELECT 0 AS estado, 'Exito' AS mensaje;
    RETURN;

EXCEPTION
    WHEN OTHERS THEN
        RETURN QUERY SELECT -1 AS estado, 'Detalles: ' || SQLERRM AS mensaje;
        RETURN;
END;
$BODY$;

ALTER FUNCTION public.fun_guardarlogbonificaciondiaria(bigint, integer, date, date, integer, character varying)
    OWNER TO postgres;
