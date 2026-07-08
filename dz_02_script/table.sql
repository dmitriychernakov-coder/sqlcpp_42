CREATE TABLE  IF NOT EXISTS genre (
    id SERIAL PRIMARY KEY,
    name VARCHAR(64) NOT NULL UNIQUE
);

CREATE TABLE  IF NOT EXISTS performer (
    id SERIAL PRIMARY KEY,
    name VARCHAR(128) NOT NULL
);



CREATE TABLE  IF NOT EXISTS album (
    id SERIAL PRIMARY KEY,
    title VARCHAR(128) NOT NULL,
    year INTEGER CHECK (year > 0)
);

CREATE TABLE  IF NOT EXISTS track (
    id SERIAL PRIMARY KEY,
    title VARCHAR(64) NOT NULL,
    length INTEGER NOT NULL CHECK (length > 0),
    album_id INTEGER NOT NULL REFERENCES album(id)
);

CREATE TABLE  IF NOT EXISTS collection (
    id SERIAL PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    year INTEGER CHECK (year > 0)
);


CREATE TABLE  IF NOT EXISTS performer_genre (
    performer_id INTEGER NOT NULL REFERENCES performer(id),
    genre_id INTEGER NOT NULL REFERENCES genre(id),
    CONSTRAINT per_gen_pk PRIMARY KEY (performer_id, genre_id)
);

CREATE TABLE IF NOT EXISTS  album_performer (
    album_id INTEGER NOT NULL REFERENCES album(id),
    performer_id INTEGER NOT NULL REFERENCES performer(id),
    CONSTRAINT al_per_pk  PRIMARY KEY (album_id, performer_id)
);


CREATE TABLE IF NOT EXISTS collection_track (
    collection_id INTEGER NOT NULL REFERENCES collection(id),
    track_id INTEGER NOT NULL REFERENCES track(id),
    CONSTRAINT col_trk_pk PRIMARY KEY (collection_id, track_id)
);