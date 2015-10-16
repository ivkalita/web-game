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
    email TEXT,
	name TEXT,
    password TEXT,
	token TEXT
);

--insert data

INSERT INTO GAMES (name) VALUES ('game 1');