
SELECT title, length
FROM track
WHERE length = (SELECT MAX(length) FROM track);

SELECT title, length
FROM track
WHERE length >= 210;

SELECT title, year
FROM collection
WHERE year BETWEEN 2018 AND 2020;

SELECT name
FROM performer
WHERE LENGTH(name) = LENGTH(REGEXP_REPLACE(name, '\s', '', 'g'));

SELECT title
FROM track
WHERE title LIKE '%мой%' OR title LIKE '%my%';
