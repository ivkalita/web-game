DROP TABLE IF EXISTS games CASCADE;
DROP TABLE IF EXISTS connections CASCADE;


CREATE TABLE games
(
    id serial primary key,
    name character varying(50),
    maxNumPlayers integer NOT NULL,
    curNumPlayers integer NOT NULL,
    owner_id integer references users(id) UNIQUE,
    CHECK (curNumPlayers <= maxNumPlayers)
);

CREATE TABLE connections
(
    game_id integer references games(id) ON DELETE CASCADE,
    player_id integer references users(id) UNIQUE,
    CONSTRAINT connections_id PRIMARY KEY (game_id, player_id)
);
