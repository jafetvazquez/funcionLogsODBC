-- Table: public.logbonificaciondiaria

-- DROP TABLE IF EXISTS public.logbonificaciondiaria;

/****************************************************************************************************************
* FECHA: 21-08-2025												*
* PROGRAMADOR: JAFET VAZQUEZ OLVERA									*
* DESCRIPCION: Tabla para grabar logs sobre certificacion de bonificacion				*
****************************************************************************************************************/


CREATE TABLE IF NOT EXISTS public.logbonificaciondiaria
(
    cliente integer,
    factura integer,
    mensaje text COLLATE pg_catalog."default",
    fechacompra date,
    fechaactual date,
    tienda integer,
    fuente text COLLATE pg_catalog."default"
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.logbonificaciondiaria
    OWNER to postgres;