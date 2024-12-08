CREATE DATABASE IF NOT EXISTS BookStore;
USE BookStore;

-- Address table
CREATE TABLE IF NOT EXISTS Address(
	AddressId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
	StreetNumber INT(255) NOT NULL,
	StreetName VARCHAR(50) NOT NULL,
	PostalCode VARCHAR(7) NOT NULL
);

-- Publisher table
CREATE TABLE IF NOT EXISTS Publisher(
	PublisherId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `Name` VARCHAR(75) NOT NULL UNIQUE -- added unique
);

-- Customer table
CREATE TABLE IF NOT EXISTS Customer(
	CustomerId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Email VARCHAR(100) NOT NULL UNIQUE, -- Added unique to the email so duplicates cannot be added
    FirstName VARCHAR(20) NOT NULL,
    LastName VARCHAR(30) NOT NULL,
	AddressId INT(255) NOT NULL,
    FOREIGN KEY (AddressId) REFERENCES Address(AddressId)
);

-- Book table
CREATE TABLE IF NOT EXISTS Book(
	BookId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Title VARCHAR(100) NOT NULL,
    PageCount INT(255) NOT NULL,
    `Year` VARCHAR(25) NOT NULL,
    Price FLOAT NOT NULL,
    Isbn VARCHAR(75) NOT NULL UNIQUE, -- Added unique
	PublisherId INT(255) NOT NULL,
    FOREIGN KEY (PublisherId) REFERENCES Publisher(PublisherId)
);

-- Author table
CREATE TABLE IF NOT EXISTS Author(
    AuthorId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    FirstName VARCHAR(20) NOT NULL,
    LastName VARCHAR(30) NOT NULL,
    UNIQUE (FirstName, LastName) -- Made sure the names are unique
);


-- Author Book table
CREATE TABLE IF NOT EXISTS AuthorBook(
    AuthorBookId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    AuthorId INT(255) NOT NULL,
    BookId INT(255) NOT NULL,
    FOREIGN KEY (AuthorId) REFERENCES Author(AuthorId),
    FOREIGN KEY (BookId) REFERENCES Book(BookId),
    UNIQUE (AuthorId, BookId) -- Unique constraint
);


-- Online Order table
CREATE TABLE IF NOT EXISTS OnlineOrder(
	OnlineOrderId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Quantity INT(255),
    OrderDate DateTime NOT NULL,
	BookId INT(255) NOT NULL,
	CustomerId INT(255) NOT NULL,
    FOREIGN KEY (BookId) REFERENCES Book(BookId),
    FOREIGN KEY (CustomerId) REFERENCES Customer(CustomerId)
);

-- Order Product table
CREATE TABLE IF NOT EXISTS OrderProduct(
	OrderProductId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
	OnlineOrderId INT(255) NOT NULL,
	BookId INT(255) NOT NULL,
    FOREIGN KEY (OnlineOrderId) REFERENCES OnlineOrder(OnlineOrderId),
    FOREIGN KEY (BookId) REFERENCES Book(BookId)
);

-- Store Inventory
CREATE TABLE IF NOT EXISTS StoreInventory(
	InventoryId INT(255) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Quantity INT(100) NOT NULL,
	BookId INT(255) NOT NULL,
    FOREIGN KEY (BookId) REFERENCES Book(BookId)
);