DROP SEQUENCE IF EXISTS games_id_seq CASCADE;
DROP TABLE IF EXISTS games CASCADE;
DROP TABLE IF EXISTS connections CASCADE;
DROP SEQUENCE IF EXISTS connections_id_seq;

CREATE SEQUENCE games_id_seq
    INCREMENT 1
    MINVALUE 1
    MAXVALUE 9223372036854775807
    START 1
    CACHE 1;

CREATE SEQUENCE connections_id_seq
    INCREMENT 1
    MINVALUE 1
    MAXVALUE 9223372036854775807
    START 1
    CACHE 1;


CREATE TABLE games
(
    id integer NOT NULL DEFAULT nextval('games_id_seq'::regclass),
    name character varying(50),
    maxNumPlayers integer NOT NULL,
    curNumPlayers integer NOT NULL,
    owner_id integer references users(id) UNIQUE,
    CHECK (curNumPlayers <= maxNumPlayers),
    CONSTRAINT games_pkey PRIMARY KEY (id)
)
WITH (
    OIDS=FALSE
);

CREATE TABLE connections
(
    id integer NOT NULL DEFAULT nextval('connections_id_seq'::regclass),
    game_id integer references games(id) ON DELETE CASCADE,
    player_id integer references users(id) UNIQUE,
    CONSTRAINT connections_pkey PRIMARY KEY (id)
)
WITH (
    OIDS=FALSE
);
