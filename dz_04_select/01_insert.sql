
INSERT INTO public.album (id, title, "year")
VALUES(nextval('album_id_seq'::regclass), 'album1', 2000);

INSERT INTO public.album (id, title, "year")
VALUES(nextval('album_id_seq'::regclass), 'album2', 2018),
(nextval('album_id_seq'::regclass), 'album3', 2019),
(nextval('album_id_seq'::regclass), 'album4', 2020),
(nextval('album_id_seq'::regclass), 'album5', 2021);

INSERT INTO performer (name) 
VALUES 
('Performer1 Performer'), 
('Performer2'), 
('Performer3 Performer'), 
('Performer4');

INSERT INTO genre (id, name) 
VALUES 
(nextval('genre_id_seq'::regclass), 'Rock'),
(nextval('genre_id_seq'::regclass), 'Pop'),
(nextval('genre_id_seq'::regclass), 'Jazz');

INSERT INTO track (title, length, album_id) 
VALUES 
('my track1', 245, 1),
('track2', 180, 1),
('track3', 210, 2),
('мой track4', 300, 2),
('track5', 260, 3),
('track6', 195, 4);


INSERT INTO collection (title, year) 
VALUES 
('Collection2000', 2000),
('Collection2018', 2018),
('Collection2019', 2019),
('Collection2020', 2020),
('Collection2021', 2021);

INSERT INTO performer_genre (performer_id, genre_id) 
values (1, 1),(2, 2),(3, 3),(4, 1);

INSERT INTO album_performer (album_id, performer_id) 
values (1, 1), 
(1, 4),
(2, 2),
(3, 3);


INSERT INTO collection_track (collection_id, track_id) 
VALUES
(1, 1), (1, 3), (1, 5),
(2, 1), (2, 2),
(3, 3), (3, 4),
(4, 5), (4, 6);
