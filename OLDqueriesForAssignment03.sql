USE sget_customer_balanceakila;


INSERT INTO rental (rental_date, inventory_id, customer_id, return_date, staff_id)
VALUES (NOW(), 2, 3, NULL, 1);

SELECT * FROM rental
WHERE customer_id = 3
ORDER BY rental_date DESC;

-- Check if staff exists
SELECT * FROM staff
WHERE staff_id = 3;
-- BE VERY CAREFUL WITH THE STUFF YOU HAVE HERE, YOU USED IT TO MAKE DUMMY DATA!!!!

-- insert a NEW movie to play with FILM_ID is 1001
INSERT INTO film (title, description, release_year, language_id, rental_duration, rental_rate, length, replacement_cost, rating, special_features)
VALUES ('Non-Rentable Movie', 'A movie that cannot be rented.', 2024, 1, 7, 2.99, 120, 19.99, 'PG', 'Trailers');
-- Verify it exists
SELECT * FROM film
WHERE title = 'Non-Rentable Movie';

-- Insert a new customer to play with
-- Insert a new address first
INSERT INTO address (address, address2, district, city_id, postal_code, phone, location)
VALUES ('123 Random St', NULL, 'Sample District', 1, '12345', '555-1234', ST_GeomFromText('POINT(0 0)', 0));
-- insert the customer now
-- Insert a new customer
INSERT INTO customer (store_id, first_name, last_name, email, address_id, active, create_date)
VALUES (1, 'Some', 'Guy', 'someguy@example.com', LAST_INSERT_ID(), 1, NOW());

-- His ID is 600
SELECT * FROM customer
WHERE first_name LIKE "Som%";

-- BE CAREFUL WITH THE STUFF UP THERE, IT'S FOR DUMMY DATA CREATION!!!

SELECT * FROM inventory;
SELECT * from rental;

SELECT * FROM customer
WHERE customer_id = 1000;

SELECT inventory_id, film_id FROM inventory WHERE film_id = 1001;
SELECT film_id, title from film WHERE film_id = 1001;

-- GET AVAILABLE INVENTORY FOR MOVIE
-- Returns: the TITLE of the movie and the NUMBER of available inventory for it (more than 0 means it can be rented)
-- SO IF THIS RETURNS A ROW or MORE it's rentable! Check it with value 1001 since you made that one and it's NOT rentable
  SELECT f.title, COUNT(i.inventory_id) AS available_inventory
FROM film f
JOIN inventory i ON f.film_id = i.film_id
LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL
WHERE f.film_id = 1000  -- Where the FILM ID goes USING 1001 from the ABOVE inset to simulate NOT RENTALBLE
GROUP BY f.film_id, f.title
HAVING COUNT(i.inventory_id) > COUNT(r.inventory_id);
  
-- rental history no date range
SELECT c.first_name, c.last_name, f.title, r.rental_date, r.return_date
FROM rental r
JOIN customer c ON r.customer_id = c.customer_id
JOIN inventory i ON r.inventory_id = i.inventory_id
JOIN film f ON i.film_id = f.film_id
WHERE c.customer_id = 1  -- Replace with the customer's ID
ORDER BY r.rental_date DESC;

-- Rental history with date range
-- this is such a waste of time to program getting the date like this.
SELECT c.first_name, c.last_name, f.title, r.rental_date, r.return_date
FROM rental r
JOIN customer c ON r.customer_id = c.customer_id
JOIN inventory i ON r.inventory_id = i.inventory_id
JOIN film f ON i.film_id = f.film_id
WHERE c.customer_id = 1  -- Customer ID: 1 (replace with actual customer_id)
  AND r.rental_date BETWEEN '2005-05-01' AND '2005-05-31'  -- Date range (pretty dumb since the transactions are from early 2000s)
ORDER BY r.rental_date DESC;



SELECT * FROM actor;

-- Create outstanding rental for customer_id 33
-- INSERT INTO rental (rental_date, inventory_id, customer_id, staff_id)
-- SELECT CURRENT_TIMESTAMP, i.inventory_id, 33, 1  -- Assuming staff_id = 1
-- FROM inventory i
-- JOIN film f ON i.film_id = f.film_id
-- WHERE f.title = 'ACADEMY DINOSAUR'  -- Specify the movie title
-- LIMIT 1;


-- Check if customer has outstanding rentals
SELECT r.rental_id, r.rental_date, r.return_date, f.title, c.first_name, c.last_name
FROM rental r
JOIN customer c ON r.customer_id = c.customer_id
JOIN inventory i ON r.inventory_id = i.inventory_id
JOIN inventory i ON r.inventory_id = i.inventory_id
JOIN film f ON i.film_id = f.film_id
WHERE r.customer_id = 1  -- Use 33 for testing, as above you added them to the database with an outstanding rental, two in fact.
AND r.return_date IS NULL;

SELECT * FROM customer
WHERE customer_id = 33;

UPDATE customer
SET first_name = TRIM(' ' FROM 'John')
WHERE customer_id = 33;

UPDATE customer
SET last_name = 'Smith'
WHERE customer_id = 33;

SELECT * FROM address
WHERE customer_id = 33;

UPDATE customer
SET address_id = 1  -- Replace 123 with the new address_id
WHERE customer_id = 33;  -- Replace 1 with the actual customer_id

SELECT cu.first_name, cu.last_name, a.address, a.address2, a.district, 
       c.city, a.postal_code, a.phone
FROM customer cu
JOIN address a ON cu.address_id = a.address_id
JOIN city c ON a.city_id = c.city_id
WHERE cu.customer_id = 33;  -- Replace 1 with the actual customer_id

UPDATE address
SET address = '123 Poop Street'  -- Replace with the new address
WHERE address_id = (
    SELECT address_id
    FROM customer
    WHERE customer_id = 33  -- Replace 1 with the actual customer_id
);

UPDATE customer
SET email = 'imalittleteapot@me.com'  -- Replace with the new email
WHERE customer_id = 33;  -- Replace 1 with the actual customer_id