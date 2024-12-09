USE bookstore;

SELECT * FROM customer;

UPDATE customer
SET FirstName = 'Jonathan'
WHERE CustomerId = 10; 

UPDATE customer
SET LastName = 'Paventi'
WHERE CustomerId = 10;

UPDATE customer
SET email = 'jon.paventi@gmail.com'
WHERE customerid = 10;

-- INSERT ADDRESSES TO BE USED INTO ADDRESS TABLE
-- These queries will 
INSERT INTO Address (StreetNumber, StreetName, PostalCode)
SELECT 123, 'Main Street', 'A1B2C3'
WHERE NOT EXISTS (
    SELECT 1 FROM Address 
    WHERE StreetNumber = 123 AND StreetName = 'Main Street' AND PostalCode = 'A1B2C3'
);

INSERT INTO Address (StreetNumber, StreetName, PostalCode)
SELECT 456, 'Oak Avenue', 'B2C3D4'
WHERE NOT EXISTS (
    SELECT 1 FROM Address 
    WHERE StreetNumber = 456 AND StreetName = 'Oak Avenue' AND PostalCode = 'B2C3D4'
);

INSERT INTO Address (StreetNumber, StreetName, PostalCode)
SELECT 789, 'Pine Road', 'C3D4E5'
WHERE NOT EXISTS (
    SELECT 1 FROM Address 
    WHERE StreetNumber = 789 AND StreetName = 'Pine Road' AND PostalCode = 'C3D4E5'
);

INSERT INTO Address (StreetNumber, StreetName, PostalCode)
SELECT 101, 'Maple Street', 'D4E5F6'
WHERE NOT EXISTS (
    SELECT 1 FROM Address 
    WHERE StreetNumber = 101 AND StreetName = 'Maple Street' AND PostalCode = 'D4E5F6'
);

-- Show just address stuff
SELECT * FROM address;


-- INSERT NEW CUSTOMERS INTO CUSTOMER TABLE
-- Relies on UNIQUE for email in customer table to prevent duplicates
INSERT INTO Customer (Email, FirstName, LastName, AddressId)
VALUES 
    ('asmith@domain.com', 'Alice', 'Smith', 1),
    ('bobbyJ@comnet.net', 'Bob', 'Jones', 2),
    ('brownC@hootmail.ca', 'Charlie', 'Brown', 3);
    
-- Show the information from the populated ADDRESS and CUSTOMER tables
SELECT DISTINCT 
    c.CustomerId, 
    c.Email, 
    c.FirstName, 
    c.LastName, 
    a.AddressId, 
    a.StreetNumber, 
    a.StreetName, 
    a.PostalCode
FROM 
    Customer c
INNER JOIN 
    Address a ON c.AddressId = a.AddressId;

-- Publisher table populating
INSERT INTO Publisher (`Name`)
VALUES 
    ('Penguin Books'),
    ('HarperCollins'),
    ('Simon & Schuster'),
    ('Random House');
    
SELECT * FROM publisher;

-- POPULATE book table
INSERT INTO Book (Title, PageCount, `Year`, Price, Isbn, PublisherId)
VALUES
    ('The Catcher in the Rye', 277, '1951', 9.99, '9780316769488', 1),
    ('To Kill a Mockingbird', 281, '1960', 7.99, '9780061120084', 2),
    -- The following two books have the SAME publisher, so there is an unused publisher in the publisher table for testing
    ('1984', 328, '1949', 8.99, '9780451524935', 3),
    ('The Great Gatsby', 180, '1925', 10.99, '9780743273565', 3),
    -- Adding Book 4 and Book 5
    ('Pride and Prejudice', 279, '1813', 12.99, '9781503290563', 3),  -- Book 4: New book with a new publisher (PublisherId = 4)
    ('Moby-Dick', 635, '1851', 11.99, '9781503280786', 1);  -- Book 5: New book with another new publisher (PublisherId = 5)


SELECT * FROM book;


-- Populate AUTHOR table
INSERT INTO Author (FirstName, LastName)
VALUES 
    ('J.K.', 'Rowling'),
    ('George', 'Orwell'),
    ('Harper', 'Lee');
Select * FROM author;

-- Populate authorbook table
INSERT INTO AuthorBook (AuthorId, BookId)
VALUES
    (1, 1),  -- Author 1 (J.K. Rowling) wrote Book 1 (The Catcher in the Rye)
    (2, 2),  -- Author 2 (George Orwell) wrote Book 2 (To Kill a Mockingbird)
    (3, 3);  -- Author 3 (Harper Lee) wrote Book 3 (1984)
SELECT * FROM authorbook;

-- Populate some fake orders
INSERT INTO OnlineOrder (Quantity, OrderDate, BookId, CustomerId)
VALUES
    (2, '2024-12-08 10:30:00', 1, 1),  -- Order 1: Customer 1 ordered 2 copies of Book 1 on 2024-12-08
    (1, '2024-12-08 11:00:00', 2, 2),  -- Order 2: Customer 2 ordered 1 copy of Book 2 on 2024-12-08
    (3, '2024-12-08 12:00:00', 3, 3);  -- Order 3: Customer 3 ordered 3 copies of Book 3 on 2024-12-08



-- POPULATE ORDERPRODUCT Table with multiple entries!
-- OrderProduct for the first order (OnlineOrderId = 1)
INSERT INTO OrderProduct (OnlineOrderId, BookId)
VALUES
    (1, 1),  -- Order 1: Book 1 (The Catcher in the Rye)
    (1, 2);  -- Order 1: Book 2 (To Kill a Mockingbird)

-- OrderProduct for the second order (OnlineOrderId = 2)
INSERT INTO OrderProduct (OnlineOrderId, BookId)
VALUES
    (2, 3);  -- Order 2: Book 3 (1984)

-- OrderProduct for the third order (OnlineOrderId = 3)
INSERT INTO OrderProduct (OnlineOrderId, BookId)
VALUES
    (3, 1),  -- Order 3: Book 1 (The Catcher in the Rye)
    (3, 2),  -- Order 3: Book 2 (To Kill a Mockingbird)
    (3, 3);  -- Order 3: Book 3 (1984)



-- POPULATE THE INVENTORY
-- Book 1 (The Catcher in the Rye) has been ordered 3 times in total
-- Book 2 (To Kill a Mockingbird) has been ordered 3 times in total
-- Book 3 (1984) has been ordered 2 times in total

INSERT INTO StoreInventory (Quantity, BookId)
VALUES
    (5, 1),  -- Book 1: Stock 5, considering 3 orders already
    (6, 2),  -- Book 2: Stock 6, considering 3 orders already
    (3, 3),  -- Book 3: Stock 3, considering 2 orders already
    (10, 4), -- Book 4: Stock 10, not ordered yet (arbitrary)
    (8, 5);  -- Book 5: Stock 8, not ordered yet (arbitrary)
