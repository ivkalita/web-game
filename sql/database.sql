DROP DATABASE "web-game";
DROP USER "web-game";
CREATE USER "web-game" WITH password 'web-game';
CREATE DATABASE "web-game" WITH ENCODING = 'UTF8';
GRANT ALL privileges ON DATABASE "web-game" TO "web-game";

\c web-game web-game

CREATE SEQUENCE players_id_seq
    INCREMENT 1
    MINVALUE 1
    MAXVALUE 9223372036854775807
    START 1
    CACHE 1;

CREATE SEQUENCE games_id_seq
    INCREMENT 1
    MINVALUE 1
    MAXVALUE 9223372036854775807
    START 1
    CACHE 1;

-- web-game\sql>psql --username=postgres --file=database.sql
-- Password: web-game

CREATE SEQUENCE connections_id_seq
    INCREMENT 1
    MINVALUE 1
    MAXVALUE 9223372036854775807
    START 1
    CACHE 1;

CREATE TABLE players
(
    id integer NOT NULL DEFAULT nextval('players_id_seq'::regclass),
    login character varying(30) UNIQUE,
    password character varying(30),
    CONSTRAINT players_pkey PRIMARY KEY (id)
)
WITH (
    OIDS=FALSE
);


CREATE TABLE games
(
    id integer NOT NULL DEFAULT nextval('games_id_seq'::regclass),
    name character varying(50),
    maxNumPlayers integer NOT NULL,
    curNumPlayers integer NOT NULL,
    owner_id integer references players(id) UNIQUE,
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
    player_id integer references players(id) UNIQUE,
    CONSTRAINT connections_pkey PRIMARY KEY (id)
)
WITH (
    OIDS=FALSE
);


CREATE TABLE users
(
    id SERIAL PRIMARY KEY,
    login TEXT,
    name TEXT,
    password TEXT,
    token TEXT
);

--insert data

INSERT INTO users(login, name, password, token) VALUES ('qwe','Qwe','k3kUonq2GNvP5VCmxQvkDgps5Xhqm12e:f9ac53b501caf001c78b4f560638c1e20d7bd703','');
INSERT INTO users(login, name, password, token) VALUES ('asd','Asd','5gYtKlwTuSL8vd8i4yr77c9sygNHbz2L:c81f3db17a169c1ebcc31bbe74919de2fe22784b','');
