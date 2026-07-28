
SELECT 
g.name AS genre_name,
COUNT(pg.performer_id) AS performers_count
FROM genre g
LEFT JOIN performer_genre pg ON g.id = pg.genre_id
GROUP BY g.name
ORDER BY performers_count DESC;

SELECT COUNT(t.id) AS tracks_period_2019_2020
FROM track t
WHERE t.album_id IN (
SELECT a.id FROM album a 
WHERE a."year" BETWEEN 2019 AND 2020);

SELECT a.title AS album_title,
AVG(t.length) AS aver_length
FROM album a
JOIN track t ON a.id = t.album_id
GROUP BY a.title
ORDER BY a.title;


SELECT DISTINCT p.name AS not_album_2020
FROM performer p
LEFT JOIN album_performer ap ON p.id = ap.performer_id
LEFT JOIN album a ON ap.album_id = a.id AND a."year" = 2020
WHERE a.id IS NULL 
ORDER BY p.name;


SELECT DISTINCT c.title
FROM collection c
JOIN collection_track ct ON c.id = ct.collection_id
JOIN track t ON ct.track_id = t.id
JOIN album a ON t.album_id = a.id
JOIN album_performer ap ON a.id = ap.album_id
JOIN performer p ON ap.performer_id = p.id
WHERE p.name LIKE '%Performer1%';