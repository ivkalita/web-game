-- web-game\sql>psql --username=postgres --file=database.sql
-- Password: web-game
DROP DATABASE "web-game";
DROP USER "web-game";
CREATE USER "web-game" WITH password 'web-game';
CREATE DATABASE "web-game" WITH ENCODING = 'UTF8';
GRANT ALL privileges ON DATABASE "web-game" TO "web-game";

\c web-game web-game

CREATE TABLE games
(
    id SERIAL PRIMARY KEY,
    name character varying(50)
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

INSERT INTO GAMES (name) VALUES ('game 1');