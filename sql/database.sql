CREATE DATABASE testbase
  WITH OWNER = postgres
       ENCODING = 'UTF8'
       TABLESPACE = pg_default;

\c testbase

CREATE SEQUENCE players_id_seq
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;
ALTER TABLE players_id_seq
  OWNER TO postgres;


-- Sequence: games_id_seq

-- DROP SEQUENCE games_id_seq;

CREATE SEQUENCE games_id_seq
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;
ALTER TABLE games_id_seq
  OWNER TO postgres;


-- Table: games

-- DROP TABLE games;

CREATE TABLE games
(
  id integer NOT NULL DEFAULT nextval('games_id_seq'::regclass),
  name character varying(50),
  CONSTRAINT games_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE games
  OWNER TO postgres;


-- Table: players

-- DROP TABLE players;

CREATE TABLE players
(
  id integer NOT NULL DEFAULT nextval('players_id_seq'::regclass),
  login character varying(30),
  password character varying(30),
  CONSTRAINT players_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE players
  OWNER TO postgres;


--insert data

INSERT INTO GAMES (name) VALUES ('game 1');

INSERT INTO PLAYERS (login, password) VALUES ('test', 'test');
INSERT INTO PLAYERS (login, password) VALUES ('Вова Смерть', 'Вова Смерть password');
INSERT INTO PLAYERS (login, password) VALUES ('STRELOK25RUS', 'STRELOK25RUS password');
INSERT INTO PLAYERS (login, password) VALUES ('Ярик Решето', 'Ярик Решето password');
INSERT INTO PLAYERS (login, password) VALUES ('Теньго Мороз', 'Теньго Мороз password');
INSERT INTO PLAYERS (login, password) VALUES ('Гриша', 'Гриша password');
INSERT INTO PLAYERS (login, password) VALUES ('S.T.A.L.K.E.R.', 'S.T.A.L.K.E.R. password');
INSERT INTO PLAYERS (login, password) VALUES ('S.T.A.L.K.E.R.(1)', 'S.T.A.L.K.E.R.(1) password');
INSERT INTO PLAYERS (login, password) VALUES ('S.T.A.L.K.E.R.(2)', 'S.T.A.L.K.E.R.(2) password');
INSERT INTO PLAYERS (login, password) VALUES ('Player', 'Player password');
INSERT INTO PLAYERS (login, password) VALUES ('Player(1)', 'Player(1) password');