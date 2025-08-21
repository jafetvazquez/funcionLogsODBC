-- Table: public.logbonificaciondiaria

-- DROP TABLE IF EXISTS public.logbonificaciondiaria;

CREATE TABLE IF NOT EXISTS public.logbonificaciondiaria
(
    cliente bigint,
    factura integer,
    fechacompra date,
    fechaactual date,
    tienda integer,
    fuente character varying(1000) COLLATE pg_catalog."default"
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.logbonificaciondiaria
    OWNER to postgres;