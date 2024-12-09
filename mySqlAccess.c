/*
*
*
*
* NOTE TO EVERYONE!!!!
*
* IF YOU ARE USING OLD FUNCTIONS FROM THE LAST PROGRAM, MAKE SURE YOU CHECK THE COLUMN IDENTIFIER
*
* For example, when searching for a customer in the customer table
* the old way was called customer_id, the new way is simply customerid, so if you try to access customer_id it will error out
*
*
*
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <conio.h>	// Let's us use getch() for menu system

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 500

// Connection details
#define DEFAULT_DATABASE_SERVER_ADDRESS "localhost"
#define DEFAULT_DATABASE_USERNAME "root"
#define DEFAULT_DATABASE_PASSWORD "6741"
#define DEFAULT_DATABASE_NAME "bookstore"


// Date ranges
#define MONTHMIN 1
#define MONTHMAX 12
#define DAYMIN 1
#define DAYMAX 31

// Do not remove these, used for final project
#define YEARMIN 1900
// YEARMAX can be changed to allow books to be added before their release date
// This will accommodate pre-orders for example
#define YEARMAX 2024
#define MAX_DATABASE_TABLE_ROWS 999

// Function prototypes
// Tool Function Prototypes
int GetIntegerFromUser();
float GetFloatFromUser();
long long int GetIsbnFromUser();
int GetBookYearFromUser();
bool NoWhitespaceCheck(char* name);
bool ValidateEmailAddress(char* address);
char PromptForYesOrNo();
void ClearCarriageReturn(char buffer[]);
void GetDateFromUser(char dateString[]);
void GetString(char* buffer);
bool ValidatePostalCode(char* postalCode);

// Database specific prototypes
bool DatabaseLoginWithUserInput(MYSQL* databaseObject);
bool DatabaseLoginWithProgramDefaults(MYSQL* databaseObject);
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase);
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString);
bool CheckRowResult(MYSQL_RES* resultToCheck);

// Old trash, keep incase its useful DELETE LATER
bool StaffIdExistsQuery(MYSQL* databaseObject, int staffIdNumber);
bool IsFilmAvailableQuery(MYSQL* databaseObject, int movieIdToCheck);
bool OutstandingRentalsQuery(MYSQL* databaseObject, int customer_id);
//bool CheckRentalHistory(MYSQL* databaseObject);
//bool AddNewRental(MYSQL* databaseObject);

/*
* SPECIFIC TABLE CRUD FUNCTION PROTOTYPES
*/

// CUSTOMER Table CRUD
// Create
bool CreateCustomer(MYSQL* databaseObject);

// Read
bool ReadCustomerTable(MYSQL* databaseObject);
bool CheckCustomerIdExistsQuery(MYSQL* databaseObject, int customerIdNumber);
bool CheckCustomerEmailExistsQuery(MYSQL* databaseObject, char* customerEmail);
bool SearchCustomerTableForEmail(MYSQL* databaseObject, char* emailToCheck);

// CUSTOMER table function prototypes
// UPDATE
bool UpdateCustomerInformation(MYSQL* databaseObject);
bool UpdateCustomerFirstName(MYSQL* databaseObject, int customer_id, char* customer_name);
bool UpdateCustomerLastName(MYSQL* databseObject, int customer_id, char* customer_lastName);
bool UpdateCustomerEmail(MYSQL* databaseObject, int customer_id, char* customer_email);
bool UpdateCustomerAddressId(MYSQL* databaseObject, int customer_id, int address_id);

// Delete
bool DeleteCustomerRecord(MYSQL* databaseObject);
bool SearchCustomerTableById(MYSQL* databaseObject, int customerToUpdate);


// BOOK table CRUD
// CREATE
bool CreateBookEntry(MYSQL* databaseObject);

// READ
bool ReadBookTable(MYSQL* databaseObject);
bool CheckBookTableIsbnExists(MYSQL* databaseObject, long long isbnToCheck);


// Order table CRUD
// CREATE
bool CreateOrder(MYSQL* databaseObject);
bool CreateNewAddress(MYSQL* databaseObject, int streetNumber, char* streetName, char* postalCode);

// READ
bool ReadOrderTable(MYSQL* databaseObject);
bool CheckAddressExistsQuery(MYSQL* databaseObject, int streetNumber, char* streetName);
bool CheckBookIsbnExistsQuery(MYSQL* databaseObject, long long isbnNumber);
bool SearchAddressTable(MYSQL* databaseObject, int streetNumber, char* streetName, char* addressId);
bool ReadAndGetPublisherTable(MYSQL* databaseObject, int* publisherIds, int* size);
bool ReadAndGetBookTable(MYSQL* databaseObject, int* bookIds, int* size);
bool ReadAndGetCustomerTable(MYSQL* databaseObject, int* customerIds, int* size);

// UPDATE

// DELETE



/*
* TOOL FUNCTIONS
*/

// Get a string from the user
void GetString(char* buffer)
{
	fgets(buffer, MAX_STRING_SIZE, stdin);
	ClearCarriageReturn(buffer);
	while (buffer[0] == '\n' || buffer[0] == '\0' || buffer[0] == ' ')
	{
		printf("Invalid input...try again.\n");
		fgets(buffer, MAX_STRING_SIZE, stdin);
	}
}

// Get an integer from the user
int GetIntegerFromUser()
{
	char userInput[MAX_STRING_SIZE] = { "\0" };
	int inputAsInt = 0;
	fgets(userInput, MAX_STRING_SIZE, stdin);

	while (sscanf(userInput, "%i", &inputAsInt) != 1)
	{
		printf("Invalid entry, try again: ");
		fgets(userInput, MAX_STRING_SIZE, stdin);
	}

	ClearCarriageReturn(userInput);
	return inputAsInt;
}

// Function to get a float input
float GetFloatFromUser()
{
	char userInput[MAX_STRING_SIZE] = { "\0" };
	float price = 0.0f;

	// Repeat until a valid float is entered
	while (1)
	{
		printf("Please enter a positive value: ");
		fgets(userInput, MAX_STRING_SIZE, stdin);

		// Remove the newline character that fgets may have added
		ClearCarriageReturn(userInput);

		// Try to convert the string to a float
		if (sscanf(userInput, "%f", &price) == 1 && price >= 0)
		{
			return price;  // Valid float
		}
		else
		{
			printf("Invalid input. Please enter a positive value: \n");
		}
	}
}

// Function to get ISBN (13 digit unique book ID)
long long int GetIsbnFromUser()
{
	char userInput[MAX_STRING_SIZE] = { "\0" };
	long long int isbnNumber = 0;

	// Repeat until a valid ISBN is entered
	while (1)
	{
		printf("Please enter a 13-digit ISBN number: ");
		fgets(userInput, MAX_STRING_SIZE, stdin);

		// Remove the newline character that fgets may have added
		ClearCarriageReturn(userInput);

		// Try to convert the string to a long long int
		if (sscanf(userInput, "%lld", &isbnNumber) == 1)
		{
			// Check if the number is 13 digits
			if (isbnNumber >= 1000000000000LL && isbnNumber <= 9999999999999LL)
			{
				return isbnNumber;  // Valid ISBN
			}
			else
			{
				printf("Invalid input. ISBN must be a 13-digit number.\n");
			}
		}
		else
		{
			printf("Invalid input. Please enter a valid 13-digit ISBN number.\n");
		}
	}
}

// Get a 4 digit year from the user
int GetBookYearFromUser()
{
	int year;
	while (scanf("%d", &year) != 1 || year < YEARMIN || year > YEARMAX)
	{
		printf("Invalid year. Please enter a year between %d and %d: ", YEARMIN, YEARMAX);
		//while (getchar() != '\n'); // Clear input buffer
	}
	return year;
}

// Check for white space in a string
bool NoWhitespaceCheck(char* name)
{
	int asciiValue = ' ';								// character we are searching for
	char* pointer = NULL;

	pointer = strchr(name, asciiValue);					// searches the user input email address for the ' '
	if (pointer != NULL)								// if the pointer find a ' ' character return false
	{
		return false;
	}

	return true;
}

// Prompt for Y/N user choices
char PromptForYesOrNo()
{
	char input;

	// Keep prompting until a valid input is entered
	while (1)
	{
		printf("Type Y or N: ");
		input = getchar();

		// Clear any extra characters from the input buffer
		ClearCarriageReturn(input);

		// Check if the input is a valid Y/y or N/n
		if (input == 'Y' || input == 'y' || input == 'N' || input == 'n')
		{
			//printf("You entered: %c\n", input); // Display the output for debugging
			return input;
		}
		else
		{
			printf("Invalid input. Please enter Y, y, N, or n.\n");
		}
	}
}

// Remove carriage return for when the user is entering a string
void ClearCarriageReturn(char buffer[])
{
	char* whereCR = strchr(buffer, '\n');
	if (whereCR != NULL)
	{
		*whereCR = '\0';
	}
}

// Basic email validation
bool ValidateEmailAddress(char* address)
{
	//const char email[MAX_STRING_SIZE] = address;			// string to hold the user input value
	const char emailEnding[MAX_STRING_SIZE] = ".com";		// used to search user string for the first occurrence of '.com'
	char* dotCom = NULL;							// pointer used for strstr function
	int ch = '@';									// the character, in ASCII value, that we are searching for '@'
	char* pointer = NULL;							// pointer used for strchr function

	pointer = strchr(address, ch);					// searches the user input email address for the '@'
	dotCom = strstr(address, emailEnding);			// searches for the first occurrence of '.com' -- this could possible be tricked?

	// if either pointer is null, aka it did not find the occurrence of those requirements
	if (pointer == NULL || dotCom == NULL)
	{
		return false;
	}

	return true;
}

// Basic postal code validation
bool ValidatePostalCode(char* postalCode)
{
	// Check all the stuff
	if (strlen(postalCode) != 6 ||
		!isalpha(postalCode[0]) ||
		!isdigit(postalCode[1]) ||
		!isalpha(postalCode[2]) ||
		!isdigit(postalCode[3]) ||
		!isalpha(postalCode[4]) ||
		!isdigit(postalCode[5]))
	{
		return false;  // Invalid postal code
	}

	return true;  // Valid postal code
}

/*
*
* I DONT THINK WE NEED THIS? I dont think we have any date stuff?
* We need to use date when adding an order, but it will use datetime now() stuff in SQL for that
*
*/
void GetDateFromUser(char dateString[])
{
	// Get year
	printf("Enter the year: ");
	int userYear = 0;
	while (userYear < YEARMIN || userYear > YEARMAX)
	{
		printf("Please enter a year between %d and %d: ", YEARMIN, YEARMAX);
		userYear = GetIntegerFromUser();
	}

	// Get month
	printf("Enter the month: ");
	int userMonth = 0;
	while (userMonth < MONTHMIN || userMonth > MONTHMAX)
	{
		printf("Please enter a month between %d and %d: ", MONTHMIN, MONTHMAX);
		userMonth = GetIntegerFromUser();
	}

	// Get day
	printf("Enter the day: ");
	int userDay = 0;
	while (userDay < DAYMIN || userDay > DAYMAX)
	{
		printf("Please enter a day between %d and %d: ", DAYMIN, DAYMAX);
		userDay = GetIntegerFromUser();
	}

	// Format the date into the provided buffer
	sprintf(dateString, "%d-%02d-%02d", userYear, userMonth, userDay);
}



/*
* DATABASE FUNCTIONS START
*/

// Login to a database using user input
bool DatabaseLoginWithUserInput(MYSQL* databaseObject)
{
	char serverAddress[MAX_STRING_SIZE] = { "\0" };
	char userName[MAX_STRING_SIZE] = { "\0" };
	char password[MAX_STRING_SIZE] = { "\0" };
	char databaseName[MAX_STRING_SIZE] = { "\0" };

	// Get the server
	printf("Please enter the server address (Ex: 192.168.0.1, or localHost): ");
	GetString(serverAddress);

	// Get the username
	printf("Please enter your user name: ");
	GetString(userName);

	// Get the password
	printf("Please enter your password: ");
	GetString(password);

	// Get the database name
	printf("Please enter the database name (Ex: bookstore): ");
	GetString(databaseName);

	if (!ConnectToDatabase(databaseObject, serverAddress, userName, password, databaseName))
	{
		// Did not connect
		return false;
	}

	// Connection successful
	return true;

}

// Login to the database using hard coded #define values
bool DatabaseLoginWithProgramDefaults(MYSQL* databaseObject)
{
	if (!ConnectToDatabase(databaseObject, 
		DEFAULT_DATABASE_SERVER_ADDRESS, 
		DEFAULT_DATABASE_USERNAME, 
		DEFAULT_DATABASE_PASSWORD, 
		DEFAULT_DATABASE_NAME))
	{
		// Did not connect
		return false;
	}

	// Connection successful
	return true;

}

// Function to connect to the database
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase)
{
	if (!mysql_real_connect(databaseObject, server, userName, password, defaultDatabase, 0, NULL, 0))
	{
		printf("Failed to connect to the DB: Error %s", mysql_error(databaseObject));
		// Close connection
		mysql_close(databaseObject);

		return false;
	}

	// Connected to database
	return true;
}

// Attempts to execute a query, and lets you know if it failed for some reason
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString)
{
	if (mysql_query(databaseObject, queryString) != 0)
	{
		printf("Failed on query!\n");
		//mysql_close(databaseObject); // I commented this out, to prevent any issues when accessing the database.
		return false;
	}
	// The query was successful!
	return true;
}

// Checks to see if any rows were returned from a MYSQL_RESULT
// returns true if there was at least 1  row returned
bool CheckRowResult(MYSQL_RES* resultToCheck)
{
	if (mysql_num_rows(resultToCheck) == 0)
	{
		// No rows were found in the result
		return false;
	}
	// 1 or more was found
	return true;
}

/*
* DATABASE FUNCTIONS END
*/


// Check if the staff exists
/*
*
* CAN BE DELETED
*
*/
bool StaffIdExistsQuery(MYSQL* databaseObject, int staffIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM staff\n"
		"WHERE staff_id = %d; \n"
		, staffIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}


// RETURNS true/false based on return of SENDQUERYTODB
// PART ONE AFTER getting CUSTOMER_ID
/*
*
* CAN BE DELETED
*
*/
bool IsFilmAvailableQuery(MYSQL* databaseObject, int movieIdToCheck)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT f.title, COUNT(i.inventory_id) AS available_inventory\n"
		"FROM film f\n"
		"JOIN inventory i ON f.film_id = i.film_id\n"
		"LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL\n"
		"WHERE f.film_id = %d  -- Where the FILM ID GOES\n"
		"GROUP BY f.film_id, f.title\n"
		"HAVING COUNT(i.inventory_id) > COUNT(r.inventory_id);",
		movieIdToCheck); // Pass the values in to use them in sprintf

	// Send the query
	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

/*
*
* CAN BE DELETED
*
*/
bool OutstandingRentalsQuery(MYSQL* databaseObject, int customer_id)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Format the SQL query string and store it in the 'newQuery' array
	sprintf(newQuery,
		"SELECT r.rental_id, r.rental_date, r.return_date, f.title, c.first_name, c.last_name\n"
		"FROM rental r\n"
		"JOIN customer c ON r.customer_id = c.customer_id\n"
		"JOIN inventory i ON r.inventory_id = i.inventory_id\n"
		"JOIN film f ON i.film_id = f.film_id\n"
		"WHERE r.customer_id = %d  -- Use 33 for testing, as above you added them to the database with an outstanding rental, two in fact.\n"
		"AND r.return_date IS NULL;",
		customer_id); // Pass the customer ID (33) for testing


	// Send the query
	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}



/*
* ----------------------------------------------------
* CUSTOMER TABLE CRUD FUNCTIONS START HERE
*/

// COMPLETED
bool CreateCustomer(MYSQL* databaseObject)
{
	char email[MAX_STRING_SIZE] = { "\0" }; // Storage for email
	char firstName[MAX_STRING_SIZE] = { "\0" }; // Storage for firstName
	char lastName[MAX_STRING_SIZE] = { "\0" }; // Storage for lastName
	char addressId[MAX_STRING_SIZE] = { "\0" }; // Storage for addressId (If the user wishes to use the ID found from the address search
	int streetNumber = NULL; // Storage for addressId
	char streetName[MAX_STRING_SIZE] = { "\0" }; // Storage for street name string
	char postalCode[MAX_STRING_SIZE] = { "\0" }; // Storage for postal code string
	bool getPostalCode = true; // Use case: when the address does NOT exist and a new postal code must be entered for a new address

	// Ask for the email address first (to see if the customer exists)
	printf("Please enter the customers EMAIL address: ");
	GetString(email);
	while (!ValidateEmailAddress(email))
	{
		printf("Invalid email, please enter a valid email (me@home.com for example): ");
		GetString(email);
	}

	// Check if the customer email does exist and return to main menu
	if (SearchCustomerTableForEmail(databaseObject, email))
	{
		printf("CUSTOMER FOUND!");
		printf("Cannot add customer, a customer with the email %s already exists!\nPress any key to return to main menu...", email);
		char returnNothing = getchar();
		return false;
	}

	// Get first name
	printf("Please enter the customers FIRST NAME: ");
	GetString(firstName);

	// Get last name
	printf("Please enter the customers LAST NAME: ");
	GetString(lastName);;

	// Ask for the customers street number
	printf("Please enter the customers house number: ");
	streetNumber = GetIntegerFromUser();

	// Get street name
	printf("Please enter the customers STREET NAME: ");
	GetString(streetName);

	// Check if the address already exists in the address table
	if (SearchAddressTable(databaseObject, streetNumber, streetName, addressId))
	{
		char yesOrNo;
		printf("\nThe address you provided already exists in the database!\n");
		printf("Would you like to use the existing address ID of %s?\n", addressId);
		yesOrNo = PromptForYesOrNo();
		if (yesOrNo == 'N' || yesOrNo == 'n')
		{
			printf("You did not want to use the existing address, please try again, returning to main menu!\n");
			return false;
		}

		else
		{
			// DONT Reuse the address, so skip getting the postal code
			getPostalCode = false;
		}

	}

	// If they entered a brand new address
	// This will let the user:
	// Enter a new postal code, create the new address in the address table
	// Search the address table once added to get the newly added addresses addressId
	// Use the NEW addressId and customer information to create the customer entry.
	if (getPostalCode)
	{
		// Get the postal code from the user
		printf("Please enter the customers POSTAL CODE (NO SPACES PLEASE EX: N6C3X7: ");
		GetString(postalCode);
		while (!ValidatePostalCode(postalCode))
		{
			printf("ERROR: Please enter the customers POSTAL CODE (NO SPACES PLEASE EX: N6C3X7: ");
			GetString(postalCode);
		}

		// If the NEW address could not be added successfully
		if (!CreateNewAddress(databaseObject, streetNumber, streetName, postalCode))
		{
			// AddNewAddress was unsuccessful
			printf("Error adding new address from customer creation, please contact support!\n");
			return false;
		}

		// If the NEW address could not be found (This is to grab the NEW addressId to assign it to the NEW customer!)
		// This SHOULD NOT happen unless there was an error specific to the database
		if (!SearchAddressTable(databaseObject, streetNumber, streetName, addressId))
		{
			printf("Error adding new address during customer creation!\n");
			return false;
		}
	}

	// Create the query to add the customer
	char createCustomerQuery[MAX_STRING_SIZE]; // Storage for the query string
	sprintf(createCustomerQuery, "INSERT INTO Customer (Email, FirstName, LastName, AddressId) VALUES ('%s', '%s', '%s', '%s');",
		email, firstName, lastName, addressId);

	// If the query could not be sent to the database
	if (!SendQueryToDatabase(databaseObject, createCustomerQuery))
	{
		printf("Failed to add new CUSTOMER table entry!\n");
		return false;
	}

	// The customer was successfully added to the database
	return true;
}

// Read function COMPLETED
bool ReadCustomerTable(MYSQL* databaseObject)
{
	// This is the BASIS for reading the entire customer table
	char readCustomerTableQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(readCustomerTableQuery, "SELECT * FROM customer\n");

	if (!SendQueryToDatabase(databaseObject, readCustomerTableQuery))
	{
		// Query was NOT successful
		printf("Customer read table unsuccessful!");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* customerTableReadResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (customerTableReadResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the customer didnt exist
	if (!CheckRowResult(customerTableReadResult))
	{
		printf("Nothing to read from customer table!!\n");
		return false;
	}
	// The result has at LEAST ONE row, the customer DOES exist!
	else
	{
		MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		while ((customerRow = mysql_fetch_row(customerTableReadResult)) != NULL)
		{
			printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
		}
	}

	// Free the result for the customer so memory isnt still consumed by it
	mysql_free_result(customerTableReadResult);
	return true;

}

// Update the customer information
//bool UpdateCustomerInformation(MYSQL* databaseObject)
//{
//	char yesOrNo[MAX_STRING_SIZE];						// temporarily changed to yes to make my life easier
//	char newEntry[MAX_STRING_SIZE];			// hold the input for the update
//	int menu = 0;							// menu control for choosing what to update
//	int newAddressId = 0;					// if updating the address this holds the id for new address
//	int customerToUpdate = NULL;			// holds the customer id for the person we are updating info for
//	bool update = false;					// controls whether or not we are updating something - defaulted to false
//	bool updateReturn = false;				// this determines whether or not the update was successful for the user to know
//	bool validEmail = false;				// control for if an email is valid or not
//	bool noWhitespace = false;					// control for if a name is valid or not
//
//	printf("Would you like to update a customer's information? Y/N\n");
//	//fgets(yesOrNo, sizeof(yesOrNo), stdin);
//	GetString(yesOrNo);
//
//	switch (yesOrNo[0])		// we only care about the first index which should have Y or N
//	{
//		// Yes case
//	case 'Y':
//	case 'y': // added for lower case
//		printf("Choose the ID of the customer to update.\n");
//		customerToUpdate = GetIntegerFromUser();
//		if (!CheckCustomerIdExistsQuery(databaseObject, customerToUpdate))
//		{
//			printf("Failed to find customer.\n");
//			update = false;						// change this to true to access menu for options to update
//			break;
//		}
//		update = true;
//		break;
//
//		// No case
//	case 'N':
//	case 'n': // Added for lower case
//		printf("\nReturning to menu.\n");		// let the user know they are returning to our default menu
//
//		// This could potentially be replaced with a RETURN false
//		break;
//
//		// Default case
//	default:
//		printf("\nInvalid input.\n");		// if someone is stupid and can't read, this probably won't help
//
//
//		// This could also be replaced with a RETURN false
//		updateReturn = false;				// let's the main() know nothing was updated
//	}
//
//
//	// if the above switches use RETURN, there is not need to check update here, since it will
//	// never get to this point if the user selects NO< or the default is used at any point
//	if (update)								// if update is true we go here
//	{
//		while (menu == 0)
//		{
//			// menu presented to user - populates through every loop
//			printf("\nPlease choose what you would like to update:\n");
//			printf("1)\tFirst Name\n");
//			printf("2)\tLast Name\n");
//			printf("3)\tEmail\n");
//			printf("4)\tAddress\n");
//			printf("5)\tReturn to main menu.\n\n");
//			menu = GetIntegerFromUser();					// get their input for which menu item - appears until a success or user enters 5
//
//			switch (menu)
//			{
//			case 1:
//				printf("\nPlease enter a new first name. Do not use any whitespace.\n");
//
//				// get the new name from the user
//				GetString(newEntry);
//
//				//validate the name by making sure there are no spaces in it
//				noWhitespace = NoWhitespaceCheck(newEntry);
//				if (noWhitespace == false)
//				{
//					printf("\nPlease do not include any whitespace in name entry.\n");
//					menu = 0;
//					break;
//				}
//
//				// if the update of the first name is false then notify the user that it didn't update and reload the menu
//				updateReturn = UpdateCustomerFirstName(databaseObject, customerToUpdate, newEntry);
//				if (updateReturn == false)
//				{
//					printf("\nThere was an error. Did not update.\n");
//					menu = 0;
//				}
//
//				// leave the loop and return to the main interface if menu != 0
//				break;
//			case 2:
//				printf("\nPlease enter a new last name. Do not use any whitespace.\n");
//
//				// get the new last name from the user
//				fgets(newEntry, sizeof(newEntry), stdin);
//				// clear the \n from the entry so it doesn't interfere with the database query
//				ClearCarriageReturn(newEntry);
//
//				//validate the name by making sure there are no spaces in it
//				noWhitespace = NoWhitespaceCheck(newEntry);
//				if (noWhitespace == false)
//				{
//					printf("\nPlease do not include any whitespace in name entry.\n");
//					menu = 0;
//					break;
//				}
//
//				// if the update of the last name is false then notify the user that it didn't update and reload the menu
//				updateReturn = UpdateCustomerLastName(databaseObject, customerToUpdate, newEntry);
//				if (updateReturn == false)
//				{
//					printf("\nThere was an error. Did not update.\n");
//					menu = 0;
//				}
//
//				// leave the loop and return to the main interface if menu != 0
//				break;
//			case 3:
//				printf("\nPlease enter a new email address. Do not user any whitespace. It must include an '@' symbol and end in '.com'.\n");
//
//				// get the new email from the user
//				fgets(newEntry, sizeof(newEntry), stdin);
//
//				// validate the email by checking for the '@' and for the ending of '.com'
//				validEmail = ValidateEmailAddress(newEntry);
//				noWhitespace = NoWhitespaceCheck(newEntry);
//
//				// if the email didn't meet the validation requirements, notify the user and reload the menu
//				if (validEmail == false || noWhitespace == false)
//				{
//					printf("\nThe email address is not valid.\n");
//					menu = 0;
//					break;
//				}
//
//				// if email is valid remove the \n so it doesn't interfere with the database query
//				ClearCarriageReturn(newEntry);
//
//				// if the update of the email is false then notify the user that it didn't update and reload the menu
//				updateReturn = UpdateCustomerEmail(databaseObject, customerToUpdate, newEntry);
//				if (updateReturn == false)
//				{
//					printf("\nThere was an error. Did not update.\n");
//					menu = 0;
//				}
//
//				// leave the loop and return to the main interface if menu != 0
//				break;
//			case 4:
//				printf("\nPlease enter a new address id for the customer. It cannot be less than 1 or greater than 605.\n");
//
//				// get an integer from the user
//				newAddressId = GetIntegerFromUser();
//				// if the integer is not between 1 and 605 inclusive, notify the user it wasn't valid and reload the menu
//
//
//				// Why only up to 605?
//				if (newAddressId < 1 || newAddressId > 605)
//				{
//					printf("\nThe address ID was not valid.\n");
//					menu = 0;
//					break;
//				}
//
//				// if the update of the address ID is false then notify the user that it didn't update and reload the menu
//				updateReturn = UpdateCustomerAddressId(databaseObject, customerToUpdate, newAddressId);
//				if (updateReturn == false)
//				{
//					printf("\nThere was an error. Did not update.\n");
//					menu = 0;
//				}
//
//				// leave the loop and return to the main interface if menu != 0
//				break;
//			case 5:
//				printf("\nReturning to main menu.\n");		// if they don't want to update they can exit using this option
//				// set the return value for the function to false because no changes were made
//				updateReturn = false;
//
//				// leave the loop and return to the main interface if menu != 0
//				break;
//			default:
//				printf("\nPlease chose one of the listed options.\n");		// if they didn't chose a valid option there
//				menu = 0;													// this makes sure we go back to the menu and re-prompt them
//			}
//		}
//	}
//
//	return updateReturn;
//}

// Delete a customer entry, requires checks in onlineOrder table for customerid
bool DeleteCustomer(MYSQL* databaseObject)
{
	// Use the CustomerExistQuery to see if they exist

	// Cannot delete customer if onlineOrder has that customerId in one of its entries
	// Need to create a function to check the onlineOrder for the specific customerid
}

/*
*
* MISC CUSTOMER table functions
*
*/
// Read customer data based on customer ID
// Return FALSE when the customer does NOT exist
// Return TRUE when the customer DOES exists, AFTER printing out the customer details!
bool SearchCustomerTableForId(MYSQL* databaseObject)
{
	printf("Please enter a customer ID to check: ");
	int customerIdToCheck = GetIntegerFromUser();
	// CHECK IF THE CUSTOMER EXISTS FIRST
	if (!CheckCustomerIdExistsQuery(databaseObject, customerIdToCheck))
	{
		// The query was NOT valid!
		printf("Failed to find customer in the database!\n");
		return false;
	}
	// The query was valid
	else
	{
		// Store the result from the query
		MYSQL_RES* customerResult = mysql_store_result(databaseObject);

		// If the result is null, there was no result
		if (customerResult == NULL)
		{
			// Print the SQL error
			printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
			return false;
		}

		// If the result was NO rows, the customer didnt exist
		if (!CheckRowResult(customerResult))
		{
			printf("Customer with ID %d does not exist.\n", customerIdToCheck);
			return false;
		}
		// The result has at LEAST ONE row, the customer DOES exist!
		else
		{
			MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
			// Iterate over the row data until it is reading null, and print each entry (probably only 1)
			while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
			{
				printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
			}
		}

		// Free the result for the customer so memory isnt still consumed by it
		mysql_free_result(customerResult);

		// The customer read was successful, put a message saying something like 
		// "Customer read successful, all results have been displayed!" where this was called
		return true;

	}
}

bool SearchCustomerTableForEmail(MYSQL* databaseObject, char* emailToCheck)
{
	// CHECK IF THE CUSTOMER EXISTS FIRST
	if (!CheckCustomerEmailExistsQuery(databaseObject, emailToCheck))
	{
		// The query was NOT valid!
		printf("Failed to find customer email in the database!\n");
		return false;
	}
	// The query was valid
	else
	{
		// Store the result from the query
		MYSQL_RES* customerResult = mysql_store_result(databaseObject);

		// If the result is null, there was no result
		if (customerResult == NULL)
		{
			// Print the SQL error
			printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
			return false;
		}

		// If the result was NO rows, the customer didnt exist
		if (!CheckRowResult(customerResult))
		{
			printf("Customer with email %s does not exist.\n", emailToCheck);
			return false;
		}

		// The result has at LEAST ONE row, the customer DOES exist!
		else
		{
			MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
			// Iterate over the row data until it is reading null, and print each entry (probably only 1)
			while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
			{
				printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
			}
		}

		// Free the result for the customer so memory isnt still consumed by it
		mysql_free_result(customerResult);

		// The customer read was successful, put a message saying something like 
		// "Customer read successful, all results have been displayed!" where this was called
		return true;

	}
}

bool UpdateCustomerInformation(MYSQL* databaseObject)
{
	// variables needed to make everything run smoothly
	char userResponse[MAX_STRING_SIZE];
	char newEntry[MAX_STRING_SIZE];
	char streetName[MAX_STRING_SIZE];
	char postalCode[MAX_STRING_SIZE];
	char addressId[MAX_STRING_SIZE] = { '\0' };
	bool didItWork = false;
	bool validEmail = false;
	bool noWhiteSpace = false;
	int customerToUpdate = 0;
	int streetNumber = 0;
	int menu = 0;

	printf("Would you like to update? Y/N\n");
	GetString(userResponse);

	// first switch case that determines if user is updating info or not
	switch (userResponse[0])
	{
	case 'y':
	case 'Y': // yes case
		printf("Please enter a customer ID to update.\n");
		customerToUpdate = GetIntegerFromUser();
		
		// does that customer exist
		if (!SearchCustomerTableById(databaseObject, customerToUpdate))
		{
			didItWork = false;		// they do not exist! exit and go back to main menu
			break;
		}

		// creates the menu with options for updating customer info
		while (menu == 0)
		{
			printf("\nPlease choose what you would like to update:\n");
			printf("1)\tFirst Name\n");
			printf("2)\tLast Name\n");
			printf("3)\tEmail\n");
			printf("4)\tAddress\n");
			printf("5)\tReturn to main menu.\n\n");
			menu = GetIntegerFromUser();

			// menu control switch
			switch (menu)
			{
			case 1:			// first name update
				printf("Please enter a new first name.\n");
				GetString(newEntry);

				// validate whether it worked or not and break out of switch
				didItWork = UpdateCustomerFirstName(databaseObject, customerToUpdate, newEntry);
				break;

			case 2:			// last name update
				printf("Please enter a new last name.\n");
				GetString(newEntry);

				// validate whether it worked or not and break out of switch
				didItWork = UpdateCustomerLastName(databaseObject, customerToUpdate, newEntry);
				break;

			case 3:			// email update
				printf("Please enter a new email address. Do not use any whitespace. It must include '@' and end in '.com'.\n");
				GetString(newEntry);

				// must have no whitespace and meet the email validation criteria
				if ((validEmail = ValidateEmailAddress(newEntry)) == true && (noWhiteSpace = NoWhitespaceCheck(newEntry) == true))
				{
					// validate whether it worked or not
					didItWork = UpdateCustomerEmail(databaseObject, customerToUpdate, newEntry);
				}
				else
				{
					// inform of invalid email
					printf("Invalid email address.\n");
				}
				break;

			case 4:			// address update
				printf("Please enter a new address for the customer.\n");

				// get new street number
				printf("Street Number:\n");
				streetNumber = GetIntegerFromUser();

				// get new street name
				printf("Please enter a street name:\n");
				GetString(streetName); 

				// do the street name/number combo already exist?
				if (SearchAddressTable(databaseObject, streetNumber, streetName, addressId))
				{
					// if it does already exist
					printf("\nThe address you provided already exists in the database!\n");
					printf("Would you like to use the existing address ID of %s? Y/N\n", addressId);
					GetString(userResponse);
					if (userResponse[0] == 'Y' || userResponse[0] == 'y')
					{
						int addrId = addressId[0] - '0';		// changes into an ASCII readable number
						// updates customer address id to the one returned by the search
						didItWork = UpdateCustomerAddressId(databaseObject, customerToUpdate, addrId); 
					}
					else
					{
						// inform them they are exiting the update and break out of switch
						printf("Exiting address update.\n");
						didItWork = false;
					}
				}
				else
				{
					// if it does not exist get a postal code
					printf("Please enter a postal code.\n");
					GetString(postalCode);
					
					// loop to force valid postal code
					while (!ValidatePostalCode(postalCode))
					{
						printf("ERROR: Please enter the customers POSTAL CODE (NO SPACES PLEASE EX: N6C3X7: ");
						GetString(postalCode);
					}
					
					// add the new address
					didItWork = AddNewAddress(databaseObject, streetNumber, streetName, postalCode);
					// get the id for the new address
					didItWork = SearchAddressTable(databaseObject, streetNumber, streetName, addressId);
					// make it ASCII from string
					int addrId = addressId[0] - '0';
					// update the customer's address ID to new address ID
					didItWork = UpdateCustomerAddressId(databaseObject, customerToUpdate, addrId);
				}

				break;

			case 5:			// exit the update menu
				printf("Returning to main menu...\n");
				didItWork = false;

				break;

			default:		// refuse any invalid menu numbers
				printf("Please select one of the listed options.\n");
				menu = 0;		// loop back to menu for another prompt
			}
		}
		break;

	case 'n':
	case 'N':		// the no case
		printf("Returning to main menu...\n");
		didItWork = false;		// give a return value to calling function
		break;

	default:		// refuse anything other than y,Y,n,N
		printf("Invalid input.\n");
	}

	// return a value to calling function
	return didItWork;
}

bool SearchCustomerTableById(MYSQL* databaseObject, int customerIdToCheck)
{
	if (!CheckCustomerIdExistsQuery(databaseObject, customerIdToCheck))
	{
		// The query was NOT valid!
		printf("Failed to find customer in the database!\n");
		return false;
	}
	// The query was valid
	else
	{
		// Store the result from the query
		MYSQL_RES* customerResult = mysql_store_result(databaseObject);

		// If the result is null, there was no result
		if (customerResult == NULL)
		{
			// Print the SQL error
			printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
			return false;
		}

		// If the result was NO rows, the customer didnt exist
		if (!CheckRowResult(customerResult))
		{
			printf("Customer with ID %d does not exist.\n", customerIdToCheck);
			return false;
		}
		
		// The result has at LEAST ONE row, the customer DOES exist!
		// Free the result for the customer so memory isnt still consumed by it
		mysql_free_result(customerResult);

		// The customer read was successful, put a message saying something like 
		// "Customer read successful, all results have been displayed!" where this was called
		return true;

	}
}

bool UpdateCustomerFirstName(MYSQL* databaseObject, int customerId, char* firstName)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\nSET FirstName = '%s'\nWHERE CustomerId = '%d'", firstName, customerId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

bool UpdateCustomerLastName(MYSQL* databaseObject, int customerId, char* lastName)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET lastname = TRIM(' ' FROM '%s')\n"
		"WHERE customerid = %d;", lastName, customerId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

bool UpdateCustomerEmail(MYSQL* databaseObject, int customerId, char* customerEmail)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET email = '%s'\n"
		"WHERE customerid = %d", customerEmail, customerId);


	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

bool UpdateCustomerAddressId(MYSQL* databaseObject, int customerId, int addressId)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET addressid = %d\n"
		"WHERE customerid = %d", addressId, customerId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Function to see if customer exists based on ID
bool CheckCustomerIdExistsQuery(MYSQL* databaseObject, int customerIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM customer\n"
		//"WHERE customer_id = %d; \n" // THis is the original line from the last program, NO underscore in the new customer ID
		"WHERE customerid = %d; \n"
		, customerIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Function to see if customer exists based on EMAIL
bool CheckCustomerEmailExistsQuery(MYSQL* databaseObject, char* customerEmail)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM customer\n"
		//"WHERE customer_id = %d; \n" // THis is the original line from the last program, NO underscore in the new customer ID
		"WHERE email = \"%s\"; \n"
		, customerEmail);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
/*
* END OF
* CUSTOMER TABLE CRUD FUNCTIONS
* -----------------------------------------------------
*/


/*
* ----------------------------------------------------
* BOOK TABLE CRUD FUNCTIONS START HERE
*/

// Create function
bool CreateBookEntry(MYSQL* databaseObject)
{
	int publisherIds[MAX_DATABASE_TABLE_ROWS];
	int size = 0;

	char bookTitle[MAX_STRING_SIZE] = { "\0" };
	int pageCount = 0;
	int bookYear = 0;
	float bookPrice = 0.00;
	long long isbnNumber = 0;
	int requestedPublisherId = 0;
	bool goodPublisherId = false;

	// Get the ISBN number
	printf("Please enter the ISBN of the book: ");
	isbnNumber = GetIsbnFromUser();
	printf("ISBN: %lld\n", isbnNumber);

	// CHECK the isbn number
	if (CheckBookTableIsbnExists(databaseObject, isbnNumber))
	{
		printf("ISBN EXISTS!\n");
		printf("Please check the ISBN and try again, press any key to return to main menu...\n");
		char returnNothing = getch();
		return false;
	}

	// Get title
	printf("Please enter the book TITLE: ");
	GetString(bookTitle);
	printf("BOOK TITLE: %s\n", bookTitle);

	// Get page count
	printf("Please enter the number of pages in the book: ");
	pageCount = GetIntegerFromUser();
	printf("Page count: %d\n", pageCount);

	// Get the release year
	// Proof of concept, expanded in the future to an exact date
	printf("Please enter the release year for the book: ");
	bookYear = GetBookYearFromUser();
	printf("Year: %d\n", bookYear);

	// Get the price
	printf("Please enter the PRICE of the book: ");
	bookPrice = GetFloatFromUser();
	printf("Year: %.2lf\n", bookPrice);

	// Show list of possible publisher IDs
	if (!ReadAndGetPublisherTable(databaseObject, publisherIds, &size))
	{
		printf("ERROR reading PUBLISHER table! Please contact support...\n");
		return false;
	}

	// Make sure the user picks a valid publisherId for the book
	while (!goodPublisherId)
	{
		// Get the user input for the publisher they want
		printf("Please pick a publisherId from the above list: ");
		requestedPublisherId = GetIntegerFromUser();

		// CHECK to ensure they selected a valid publisherID
		for (int i = 0; i < size; i++)
		{
			//printf("PublisherId[%d]: %d\n", i, publisherIds[i]);
			if (requestedPublisherId == publisherIds[i])
			{
				goodPublisherId = true;
				break;
			}
		}

		if (!goodPublisherId)
		{
			printf("Invalid ID, do you want to exit adding a new book?\n");
			char response = PromptForYesOrNo();
			if (response == 'Y' || response == 'y')
			{
				printf("Exiting adding new book, returning to main menu...\n");
				return false;
			}
		}
	}

	char* query[MAX_STRING_SIZE] = { "\0" };
	sprintf(query,
		"INSERT INTO Book (Title, PageCount, `Year`, Price, Isbn, PublisherId) "
		"VALUES ('%s', '%d', '%d', %.2lf, '%lld', '%d');",
		bookTitle, pageCount, bookYear, bookPrice, isbnNumber, requestedPublisherId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Error
		return false;
	}

	// Added successfully
	return true;
}

// Read function COMPLETED
bool ReadBookTable(MYSQL* databaseObject)
{
	// This is the BASIS for reading the entire customer table
	char readBookTableQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(readBookTableQuery,
		"SELECT * FROM book");


	if (!SendQueryToDatabase(databaseObject, readBookTableQuery))
	{
		// Query was NOT successful
		printf("BOOK read table unsuccessful!");
		return false;
	}
	// Store the result from the query
	MYSQL_RES* bookTableReadResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (bookTableReadResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the BOOK didnt exist
	if (!CheckRowResult(bookTableReadResult))
	{
		printf("Nothing to read from BOOK table!!\n");
		return false;
	}
	// The result has at LEAST ONE row, the customer DOES exist!
	else
	{
		printf("----------- START READ FROM BOOK TABLE -----------\n");
		MYSQL_ROW bookRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		while ((bookRow = mysql_fetch_row(bookTableReadResult)) != NULL)
		{
			printf("Bookid: %s\n\tTitle: %s\n\tPageCount: %s\n\tYear: %s\n\tPrice: $%s\n\tISBN: %s\n\tPublisherId: %s\n\n", bookRow[0], bookRow[1], bookRow[2], bookRow[3], bookRow[4], bookRow[5], bookRow[6]);
		}
	}
	printf("----------- END READ FROM BOOK TABLE -----------\n");
	// Free the result for the customer so memory isnt still consumed by it
	mysql_free_result(bookTableReadResult);
	return true;

}

// Update function

// Delete function

/*
* MISC BOOK table functions
*/
bool CheckBookTableIsbnExists(MYSQL* databaseObject, long long isbnToCheck)
{
	// CHECK IF THE ISBN EXISTS
	if (!CheckBookIsbnExistsQuery(databaseObject, isbnToCheck))
	{
		// The query was NOT valid!
		printf("Error completing ISBN query in BOOK table!\n");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* isbnResult = mysql_store_result(databaseObject);

	// If the result is null, there was a problem
	if (isbnResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the isbn is unique
	// return a false
	if (!CheckRowResult(isbnResult))
	{
		return false;
	}

	// Free memory from the result
	mysql_free_result(isbnResult);

	// Return TRUE if the ISBN does exist, handle in the function calling this
	return true;
}

/*
* END OF
* BOOK TABLE CRUD FUNCTIONS
* -----------------------------------------------------
*/


/*
* ----------------------------------------------------
* ORDER TABLE CRUD FUNCTIONS START HERE
*/

// Create function
bool CreateOrder(MYSQL* databaseObject)
{
	// Used to verify proper bookid is used
	int bookIds[MAX_DATABASE_TABLE_ROWS];
	int bookArraySize = 0;
	int requestedBookId = 0;
	bool goodBookId = false;

	// Used to verify proper customerid is used
	int customerIds[MAX_DATABASE_TABLE_ROWS];
	int customerArraySize = 0;
	int requestedCustomerId = 0;
	bool goodCustomerId = false;

	// Order variables
	int bookQuantity = 0;

	// Show customer Ids
	if (!ReadAndGetCustomerTable(databaseObject, customerIds, &customerArraySize))
	{
		printf("ERROR reading CUSTOMER table! Please contact support...\n");
		return false;
	}

	// Make sure the user picks a valid customerId for the book
	while (!goodCustomerId)
	{
		// Get the user input for the customer they want
		printf("Please pick a customerId from the above list to tie the order to: ");
		requestedCustomerId = GetIntegerFromUser();

		// CHECK to ensure they selected a valid bookId
		for (int i = 0; i < customerArraySize; i++)
		{
			if (requestedCustomerId == customerIds[i])
			{
				goodCustomerId = true;
				break;
			}
		}

		if (!goodCustomerId)
		{
			printf("Invalid CUSTOMER ID, do you want to exit adding a new order?\n");
			char response = PromptForYesOrNo();
			if (response == 'Y' || response == 'y')
			{
				printf("Exiting adding new ORDER, returning to main menu...\n");
				return false;
			}
		}
	}

	// Show the bookids
	if (!ReadAndGetBookTable(databaseObject, bookIds, &bookArraySize))
	{
		printf("ERROR reading BOOK table! Please contact support...\n");
		return false;
	}

	// Make sure the user picks a valid publisherId for the book
	while (!goodBookId)
	{
		// Get the user input for the publisher they want
		printf("Please pick a bookId from the above list: ");
		requestedBookId = GetIntegerFromUser();

		// CHECK to ensure they selected a valid bookId
		for (int i = 0; i < bookArraySize; i++)
		{
			//printf("PublisherId[%d]: %d\n", i, publisherIds[i]);
			if (requestedBookId == bookIds[i])
			{
				goodBookId = true;
				break;
			}
		}

		if (!goodBookId)
		{
			printf("Invalid BOOK ID, do you want to exit adding a new order?\n");
			char response = PromptForYesOrNo();
			if (response == 'Y' || response == 'y')
			{
				printf("Exiting adding new ORDER, returning to main menu...\n");
				return false;
			}
		}
	}

	// How many of that book do they want to order?
	printf("Please enter the quantity for the number of books: ");
	bookQuantity = GetIntegerFromUser();

	// Create the onlineOrder
	char query[MAX_STRING_SIZE] = { "\0" };
	sprintf(query,
		"INSERT INTO OnlineOrder (Quantity, OrderDate, BookId, CustomerId) "
		"VALUES ('%d', NOW(), '%d', '%d');",
		bookQuantity, requestedBookId, requestedCustomerId);

	// Send the query to the database
	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Error adding it to the DB
		return false;
	}

	// Added to the DB!
	return true;
}

// Read function COMPLETED
bool ReadOrderTable(MYSQL* databaseObject)
{
	// This is the BASIS for reading the entire customer table
	char readOrderTableQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(readOrderTableQuery,
		"SELECT o.OnlineOrderId, o.Quantity, o.OrderDate, "
		"CONCAT(c.FirstName, ' ', c.LastName) AS CustomerFullName, "
		"CONCAT(a.StreetNumber, ' ', a.StreetName, ', ', a.PostalCode) AS CustomerAddress, "
		"b.Title AS BookTitle "
		"FROM OnlineOrder o "
		"JOIN Customer c ON o.CustomerId = c.CustomerId "
		"JOIN Address a ON c.AddressId = a.AddressId "
		"JOIN Book b ON o.BookId = b.BookId;");


	if (!SendQueryToDatabase(databaseObject, readOrderTableQuery))
	{
		// Query was NOT successful
		printf("ORDER read table unsuccessful!");
		return false;
	}
	// Store the result from the query
	MYSQL_RES* orderTableReadResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (orderTableReadResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the customer didnt exist
	if (!CheckRowResult(orderTableReadResult))
	{
		printf("Nothing to read from ORDER table!!\n");
		return false;
	}
	// The result has at LEAST ONE row, the customer DOES exist!
	else
	{
		printf("----------- START READ FROM ORDER TABLE -----------\n");
		MYSQL_ROW orderRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		while ((orderRow = mysql_fetch_row(orderTableReadResult)) != NULL)
		{
			printf("Order ID: %s\n\tQuantity: %s\n\tOrderDate: %s\n\tCustomerName: %s\n\tCustomerAddress: %s\n\tBookTitle: %s\n\n", orderRow[0], orderRow[1], orderRow[2], orderRow[3], orderRow[4], orderRow[5]);
		}
	}
	printf("----------- END READ FROM ORDER TABLE -----------\n");
	// Free the result for the customer so memory isnt still consumed by it
	mysql_free_result(orderTableReadResult);
	return true;

}

// Update function

// Delete function

/*
* MISC ORDER table functions
*/

/*
* END OF
* ORDER TABLE CRUD FUNCTIONS
* -----------------------------------------------------
*/


/*
*
* MISC Table Functions
*
*/

bool CreateNewAddress(MYSQL* databaseObject, int streetNumber, char* streetName, char* postalCode)
{
	char query[MAX_STRING_SIZE] = { "\0" };
	// Create the SQL query using sprintf
	sprintf(query,
		"INSERT INTO Address (StreetNumber, StreetName, PostalCode) VALUES ('%d', '%s', '%s');",
		streetNumber, streetName, postalCode);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Function to see if customer exists based on EMAIL
bool CheckAddressExistsQuery(MYSQL* databaseObject, int streetNumber, char* streetName)
{
	char query[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(query, "SELECT * FROM Address WHERE StreetNumber = \"%d\" AND StreetName = \"%s\";", streetNumber, streetName);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

bool CheckBookIsbnExistsQuery(MYSQL* databaseObject, long long isbnNumber)
{
	char query[MAX_STRING_SIZE]; // Where the query will be stored.

	sprintf(query, "SELECT * FROM Book WHERE Isbn = '%lld';", isbnNumber);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

bool SearchAddressTable(MYSQL* databaseObject, int streetNumber, char* streetName, char* addressId)
{
	// CHECK IF THE CUSTOMER EXISTS FIRST
	if (!CheckAddressExistsQuery(databaseObject, streetNumber, streetName))
	{
		// The query was NOT valid!
		printf("Invalid query\n");
		return false;
	}

	// The query was valid

		// Store the result from the query
	MYSQL_RES* addressResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (addressResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the address does not exist
	if (!CheckRowResult(addressResult))
	{
		printf("Customer with address %d %s does not exist.\n", streetNumber, streetName);
		return false;
	}

	// The result has at LEAST ONE row, the address DOES exist!
	else
	{
		MYSQL_ROW addressRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		while ((addressRow = mysql_fetch_row(addressResult)) != NULL)
		{
			printf("ADDRESS Records found: \n\tAddressId: %s\n\tStreetNumber: %s\n\tStreetName: %s\n\tPostalCode: %s\n\t", addressRow[0], addressRow[1], addressRow[2], addressRow[3]);
			strcpy(addressId, addressRow[0]); // Copy ID
			break;


		}
	}

	// Free the result for the customer so memory isnt still consumed by it
	mysql_free_result(addressResult);

	// Found the address
	return true;


}

bool ReadAndGetPublisherTable(MYSQL* databaseObject, int* publisherIds, int* size)
{
	char readPublisherTableQuery[MAX_STRING_SIZE];

	// Create the SQL query string to read all publishers ordered by PublisherId
	sprintf(readPublisherTableQuery,
		"SELECT * FROM publisher ORDER BY PublisherId;");

	// Send the query to the database
	if (!SendQueryToDatabase(databaseObject, readPublisherTableQuery))
	{
		printf("ORDER read table unsuccessful!\n");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* publisherTableResult = mysql_store_result(databaseObject);

	// If the result is NULL, there was an error
	if (publisherTableResult == NULL)
	{
		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
		return false;
	}

	// If no rows, print a message and return
	if (mysql_num_rows(publisherTableResult) == 0)
	{
		printf("Nothing to read from PUBLISHER table!\n");
		return false;
	}

	printf("----------- START READ FROM PUBLISHER TABLE -----------\n");

	// Initialize the size of publisherIds to 0
	*size = 0;

	MYSQL_ROW publisherRow;
	// Iterate through rows and store PublisherId in the array
	while ((publisherRow = mysql_fetch_row(publisherTableResult)) != NULL)
	{
		// Assuming publisherRow[0] is the PublisherId (as string) and needs to be converted to int
		int publisherId = atoi(publisherRow[0]);  // Convert PublisherId to int

		// Store the PublisherId in the array
		publisherIds[*size] = publisherId;
		(*size)++;  // Increment the size of the array

		// Print PublisherId and PublisherName for debug purposes
		printf("PublisherId: %s - PublisherName: %s\n", publisherRow[0], publisherRow[1]);
	}

	printf("----------- END READ FROM PUBLISHER TABLE -----------\n");

	// Free the result to avoid memory leaks
	mysql_free_result(publisherTableResult);
	return true;
}

bool ReadAndGetBookTable(MYSQL* databaseObject, int* bookIds, int* size)
{
	char readBookTableQuery[MAX_STRING_SIZE];

	// Create the SQL query string to read all publishers ordered by PublisherId
	sprintf(readBookTableQuery,
		"SELECT * FROM book ORDER BY bookid;");

	// Send the query to the database
	if (!SendQueryToDatabase(databaseObject, readBookTableQuery))
	{
		printf("ORDER read table unsuccessful!\n");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* bookTableResult = mysql_store_result(databaseObject);

	// If the result is NULL, there was an error
	if (bookTableResult == NULL)
	{
		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
		return false;
	}

	// If no rows, print a message and return
	if (mysql_num_rows(bookTableResult) == 0)
	{
		printf("Nothing to read from BOOK table!\n");
		return false;
	}

	printf("----------- START READ FROM BOOK TABLE -----------\n");

	// Initialize the size of publisherIds to 0
	*size = 0;

	MYSQL_ROW bookRow;
	// Iterate through rows and store PublisherId in the array
	while ((bookRow = mysql_fetch_row(bookTableResult)) != NULL)
	{
		// Assuming publisherRow[0] is the PublisherId (as string) and needs to be converted to int
		int bookId = atoi(bookRow[0]);  // Convert PublisherId to int

		// Store the PublisherId in the array
		bookIds[*size] = bookId;
		(*size)++;  // Increment the size of the array

		// Print PublisherId and PublisherName for debug purposes
		printf("bookId: %s - Book Title: %s\n", bookRow[0], bookRow[1]);
	}

	printf("----------- END READ FROM BOOK TABLE -----------\n");

	// Free the result to avoid memory leaks
	mysql_free_result(bookTableResult);
	return true;
}

bool ReadAndGetCustomerTable(MYSQL* databaseObject, int* customerIds, int* size)
{
	char readCustomerTableQuery[MAX_STRING_SIZE];

	// Create the SQL query string to read all publishers ordered by PublisherId
	sprintf(readCustomerTableQuery,
		"SELECT * FROM customer ORDER BY customerid;");

	// Send the query to the database
	if (!SendQueryToDatabase(databaseObject, readCustomerTableQuery))
	{
		printf("ORDER read table unsuccessful!\n");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* customerTableResult = mysql_store_result(databaseObject);

	// If the result is NULL, there was an error
	if (customerTableResult == NULL)
	{
		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
		return false;
	}

	// If no rows, print a message and return
	if (mysql_num_rows(customerTableResult) == 0)
	{
		printf("Nothing to read from BOOK table!\n");
		return false;
	}

	printf("----------- START READ FROM CUSTOMER TABLE -----------\n");

	// Initialize the size of publisherIds to 0
	*size = 0;

	MYSQL_ROW customerRow;
	// Iterate through rows and store PublisherId in the array
	while ((customerRow = mysql_fetch_row(customerTableResult)) != NULL)
	{
		// Assuming publisherRow[0] is the PublisherId (as string) and needs to be converted to int
		int customerId = atoi(customerRow[0]);  // Convert PublisherId to int

		// Store the PublisherId in the array
		customerIds[*size] = customerId;
		(*size)++;  // Increment the size of the array

		// Print PublisherId and PublisherName for debug purposes
		printf("customerId: %s - Customer Email: %s - Customer Name: %s %s\n", customerRow[0], customerRow[1], customerRow[2], customerRow[3]);
	}

	printf("----------- END READ FROM CUSTOMER TABLE -----------\n");

	// Free the result to avoid memory leaks
	mysql_free_result(customerTableResult);
	return true;
}




/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			DELETE FUNCTIONS BELOW

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


// Main delete menu
void deleteMenu(void)
{
	printf("***What do you want to delete?***\n\n");
	printf("\t(1) Customer\n");
	printf("\t(2) Book\n");
	printf("\t(3) Order\n");
	printf("\t(x) Cancel Deletion\n");
}


// Customer Deletion Functions

// Delete Customer Implications message
void deleteCustomerImplication(void)
{
	printf("You have chosen to delete a customer record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) Customer's personal information will be permanently deleted from the database.\n");
	printf("\t2) All order records associated with this customer will be deleted from the database.\n");
	printf("\t3) The address associated with this customer will be deleted from the database.\n\n");
	printf("Would you like to proceed with deleting a customer?\n\n");
	printf("Enter 'Y' to proceed or any input to cancel.\n");
}


/*
*
* CAN BE REUSED at some level, some changes will need to be made
* Must check if OnlineOrder is using customerID, cannot delete if being used in onlineOrder
* Should it delete the address associated with the customer?
*
*
*/
bool DeleteCustomerRecord(MYSQL* databaseObject)
{
	// Prompt user to confirm deleting a customer and explain implications.
	deleteCustomerImplication();
	// Get input from user

	char yesOrNo[MAX_STRING_SIZE] = { "" };
	fgets(yesOrNo, sizeof(yesOrNo), stdin);

	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (strcmp(yesOrNo, "Y\n") == 0)
	{
		// Clear screen
		system("cls");

		// Prompt customer id
		printf("You've chosen to proceed with deletion.\n\n");
		printf("Please enter the customer's ID:");
		int customerIdToCheck = GetIntegerFromUser(); // The ID to check

		// Check Customer Id
		if (!CheckCustomerIdExistsQuery(databaseObject, customerIdToCheck))
		{
			// The query was NOT valid!
			printf("Failed to find customer in the database!\n");
			return false;
		}
		// The query was valid
		else
		{
			// Store the result from the query
			MYSQL_RES* customerResult = mysql_store_result(databaseObject);

			// If the result is null, there was no result
			if (customerResult == NULL)
			{
				// Print the SQL error
				printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
				return false;
			}

			// If the result has NO rows, the customer didnt exist
			if (!CheckRowResult(customerResult))
			{
				printf("Customer with ID %d does not exist.\n", customerIdToCheck);
				return false;
			}
			// The result has at LEAST ONE row, the customer DOES exist!
			else
			{
				MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
				// Iterate over the row data until it is reading null, and print each entry (probably only 1)
				while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
				{
					printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n\n", customerRow[0], customerRow[2], customerRow[3]);
				}
			}
			// Free the result for the customer so memory isnt still consumed by it
			mysql_free_result(customerResult);
		}

		/*

		// we don't have a way to check status of order; may need to add order status to table for this...
		// we also don't have payment... maybe we should add this too???


		// Check if customer has returned all rented films
		int customerIdToCheckForOutstandRentals = customerIdToCheck;
		OutstandingRentalsQuery(databaseObject, customerIdToCheckForOutstandRentals);
		MYSQL_RES* checkOutstandingRentals = mysql_store_result(databaseObject);

		// If it failed to get any results for some reason
		if (checkOutstandingRentals == NULL)
		{
			// Print the SQL error
			printf("Failed to get the result set! %s", mysql_error(databaseObject));
			return EXIT_FAILURE;
		}




		// Check the returned rows to see if the query returned anything. If it doesn't, then delete the customer.
		if (!CheckRowResult(checkOutstandingRentals))
		{
			// Notify user that customer's information is being deleted
			printf("\nCustomer has no outstanding rentals!\nProceeding to delete customer's records...\n\n");

			// Delete customer rental records
			char deleteRentalQuery[MAX_STRING_SIZE];
			sprintf(deleteRentalQuery, "DELETE FROM rental WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteRentalQuery))
			{
				printf("Rental records deletion failed!\n");
				return false;
			}
			printf("Rental records deletion successful!\n");

			// Delete customer payment records
			char deletePaymentQuery[MAX_STRING_SIZE];
			sprintf(deletePaymentQuery, "DELETE FROM payment WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deletePaymentQuery))
			{
				printf("Payment records deletion failed!\n");
				return false;
			}
			printf("Payment records deletion successful!\n");

			// Delete customer payment records
			char deleteCustomerQuery[MAX_STRING_SIZE];
			sprintf(deleteCustomerQuery, "DELETE FROM customer WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteCustomerQuery))
			{
				printf("Customer records deletion failed!\n");
				return false;
			}
			printf("Customer records deletion successful!\n");

			// Delete customer payment records
			char deleteAddressQuery[MAX_STRING_SIZE];
			sprintf(deleteAddressQuery, "DELETE FROM address WHERE address_id = (SELECT address_id FROM customer WHERE customer_id = %d);", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteAddressQuery))
			{
				printf("Customer address records deletion failed!\n");
				return false;
			}
			printf("Customer address records deletion successful!\n");

			printf("Deletion process concluded.\n");

			return true;
		}
		// Otherwise, cancel the deletion
		else
		{
			printf("This customer has an outstanding rental! Information below:\n\n");

			MYSQL_ROW outstandingRentalData;
			while ((outstandingRentalData = mysql_fetch_row(checkOutstandingRentals)) != NULL)
			{
				printf("OUTSTANDING RENTAL: MOVIE info:\n\tID: %s\n\ttitle: %s\n\tRental Date: %s\n\tReturn date: %s \n\n", outstandingRentalData[0], outstandingRentalData[3], outstandingRentalData[1], outstandingRentalData[2]);
			}
			mysql_free_result(checkOutstandingRentals);

			printf("Unable to delete customer due to outstanding rental.\nCancelling deletion process...\n\n");

			return false;
		}

		*/
	}
	else
	{
		system("cls");
		printf("Deletion Process cancelled...\n");

		return false;
	}

	// Deleted the customer, return true!
	return true;
}


// Delete book functions

// Delete book implications message
void deleteBookImplication(void)
{
	printf("You have chosen to delete a book's record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) Book's details will be permanently deleted from the database.\n");
	printf("\t2) AuthorBook relating to this book will be permanently deleted from the database.\n");
	printf("\t3) OrderProduct relating to this book will be permanently deleted from the database.\n");
	printf("\t4) OnlineOrder relating to this book will be permanently deleted from the database.\n");
	printf("\t5) StoreInventory relating to this book will be permanently deleted from the database.\n");

	printf("Would you like to proceed with deleting a book?\n\n");
	printf("Enter 'Y' to proceed or any input to cancel.\n");
}


// Function to see if book exists based on ID
bool CheckBookIdExistsQuery(MYSQL* databaseObject, int bookIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM book\n"
		"WHERE BookId = %d; \n"
		, bookIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}


/*
*
* CAN BE REUSED at some level, some changes will need to be made
* Must check if OnlineOrder is using customerID, cannot delete if being used in onlineOrder
* Should it delete the address associated with the customer?
*
*
*/
bool DeleteBookRecord(MYSQL* databaseObject)
{
	// Prompt user to confirm deleting a book and explain implications.
	deleteBookImplication();

	// Get input from user
	char yesOrNo[MAX_STRING_SIZE] = { "" };
	fgets(yesOrNo, sizeof(yesOrNo), stdin);

	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (strcmp(yesOrNo, "Y\n") == 0)
	{
		// Clear screen
		system("cls");

		// Prompt book id
		printf("You've chosen to proceed with deletion.\n\n");
		printf("Please enter the book's ID:");
		int bookIdToCheck = GetIntegerFromUser(); // The ID to check

		// Check book Id
		if (!CheckBookIdExistsQuery(databaseObject, bookIdToCheck))
		{
			// The query was NOT valid!
			printf("Failed to find book in the database!\n");
			return false;
		}
		// The query was valid
		else
		{
			// Store the result from the query
			MYSQL_RES* bookResult = mysql_store_result(databaseObject);

			// If the result is null, there was no result
			if (bookResult == NULL)
			{
				// Print the SQL error
				printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
				return false;
			}

			// If the result has NO rows, the book didnt exist
			if (!CheckRowResult(bookResult))
			{
				printf("Book with ID %d does not exist.\n", bookIdToCheck);
				return false;
			}
			// The result has at LEAST ONE row, the book DOES exist!
			else
			{
				MYSQL_ROW bookRow; // Get the rows using MYSQL_ROW for printing
				// Iterate over the row data until it is reading null, and print each entry (probably only 1)
				while ((bookRow = mysql_fetch_row(bookResult)) != NULL)
				{
					printf("Book ID: Records found: Book ID: %s, Book Title: %s\n\n", bookRow[0], bookRow[1]);
				}
			}
			// Free the result for the customer so memory isnt still consumed by it
			mysql_free_result(bookResult);
		}

		// If book ID exists, proceed to delete any records related to the book

		// Notify user that book's information is being deleted
		printf("\nProceeding to delete book's records...\n\n");

		// Delete AuthorBook records related to the book
		char deleteAuthorBookQuery[MAX_STRING_SIZE];
		sprintf(deleteAuthorBookQuery, "DELETE FROM AuthorBook WHERE BookId = %d;", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteAuthorBookQuery))
		{
			printf("AuthorBook records deletion failed!\n");
			return false;
		}
		printf("AuthorBook records deletion successful!\n");

		// Delete OrderProduct records
		char deleteOrderProductQuery[MAX_STRING_SIZE];
		sprintf(deleteOrderProductQuery, "DELETE FROM OrderProduct WHERE BookId = %d;", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOrderProductQuery))
		{
			printf("OrderProduct records deletion failed!\n");
			return false;
		}
		printf("OrderProduct records deletion successful!\n");

		// Delete OnlineOrder records
		char deleteOnlineOrderQuery[MAX_STRING_SIZE];
		sprintf(deleteOnlineOrderQuery, "DELETE FROM OnlineOrder WHERE BookId = %d;", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOnlineOrderQuery))
		{
			printf("OnlineOrder records deletion failed!\n");
			return false;
		}
		printf("OnlineOrder records deletion successful!\n");

		// Delete StoreInventory records
		char deleteStoreInventoryQuery[MAX_STRING_SIZE];
		sprintf(deleteStoreInventoryQuery, "DELETE FROM StoreInventory WHERE BookId = %d;", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteStoreInventoryQuery))
		{
			printf("StoreInventory records deletion failed!\n");
			return false;
		}
		printf("StoreInventory records deletion successful!\n");

		// Delete Book records
		char deleteBookInventoryQuery[MAX_STRING_SIZE];
		sprintf(deleteBookInventoryQuery, "DELETE FROM Book WHERE BookId = %d;", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteBookInventoryQuery))
		{
			printf("Book records deletion failed!\n");
			return false;
		}
		printf("Book records deletion successful!\n");



		printf("Deletion process concluded.\n");

		return true;
		

	}
	else  // User chose to cancel deletion so cancel the deletion and exit
	{
		system("cls");
		printf("Deletion Process cancelled...\n");

		return false;
	}

	// Deleted the book, return true!
	return true;
}






/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			DELETE FUNCTIONS ABOVE

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



int main2()
{
	// What we've said our project will do
	// CRUD STATUS for Project:
	// 
	// CUSTOMER CRUD
	// Completed: 
	// Read (Good!)
	// 
	// Almost Completed:
	// Update - do we want to update based on the address ID or the address information and update the ID using that?
	//  - Example: If the customer wants to update their address, let them enter a new address, then check it against the table
	//		- if it exists, grab the existing ID and update the customer
	//		- otherwise: Create a new address entry and get that ID, then use it to update?
	// 
	// Not finished:
	// Customer CREATE: We need to decide what part of the customer info will be used to check if the customer already exists
	// email
	// firstname
	// lastname
	// addressid(FK) - Should we list the addresses, or just get the user input, check if it exists, and if it does:
	// Just add the addressId of the existing address without even telling them, since they wanted that address anyway?
	// 
	// Customer DELETE: Needs to be completed, and check onlineorder table to see if customerId is used for any order
	// 
	// BOOK CRUD
	// NOTHING COMPLETED YET
	// CREATE Requirements:
	// title
	// pagecount
	// year
	// price
	// ISBN
	// publisherID(FK) : We should probably display a list of publishers, so the user creating the book can enter a publisher ID to 
	// attach to the BOOK creation
	// 
	// READ: Simple to implement
	// 
	// UPDATE Requirements:
	// title
	// pagecount
	// year
	// price
	// ISBN
	// publisherID(FK) : Do we let the user search by publisher NAME or publisher id?
	// 
	// DELETE Requirements:
	// Checks need to be completed, a book cannot be deleted if that bookid exists in the following:
	// AuthorBook
	// OnlineOrder
	// OrderProduct
	// StoreInventory
	// 
	// 
	// ORDER CRUD
	// Nothing completed yet!
	// We should probably have an order check the stock level in the inventory table to either
	// let them order the book, or tell them the order will be fulfilled when the book is in stock for example?
	// 
	// CREATE Requirements: 
	// quantity
	// orderdate
	// bookId(FK) - Again, do we show a list, or use the book name?
	// customerId(FK) -  probably use customer email?
	// 
	// READ requirements: What? Just make the function lol
	// 
	// UPDATE Requirements:
	// quantity
	// orderdate
	// bookId(FK) - Again, do we show a list, or use the book name?
	// customerId(FK) -  probably use customer email?
	// 
	// DELETE Requirements:
	// Check if orderProduct uses onlineOrderId, if the order ID is used somewhere there, it cannot be deleted
	// 
	// 
	// Add a customer
	// add a book
	// create an order
	// look up/read a customer's details
	// look up/read a book's details
	// look up/read an order's details
	// update a customer
	// update a book
	// update an order
	// delete a customer
	// delete a book
	// delete an order
	// Ability to reconfigure connection settings and change IP address of MySQL server (I think wickens knows what this is...)
	// Ability to access a HELP page
	// Ability to save user’s credentials to access the project



	// General plan of what we could do in main:

	// 1) Login details
	// - user enters login details to enter the program
	// - first we check if there's any login information in the sql
	// - if it exists, we ask for a login and validate that what the user enters matches what we have
	// - otherwise, we ask the user to create a new login and we save that in the sql

	// 2) First menu
	// - This menu should be used to break down the options into their own menus. An example is below:
	// - 1) I want to do a process involving customers!
	// - 2) I want to do a process involving books!
	// - 3) I want to do a process involving orders!
	// - 4) I need help!!!
	// - 5) I need to change my login credentials!

	// 3) Sub menus
	// - these menus are the sub menus of the options above
	// 
	// 3.1) Customers
	// 1) Add customer
	// 2) Read customer
	// 3) update customer
	// 4) delete customer
	// 
	// 3.2) Book
	// 1) Add book
	// 2) Read book
	// 3) update book
	// 4) delete book
	// 
	// 3.3) Order
	// 1) Add order
	// 2) Read order
	// 3) update order
	// 4) delete order
	// 
	// 3.4) Help page
	// - this is mainly up to us on what we should have
	// - we should probably have an email and phone number for customer support
	// - maybe some basic details or an faq?
	// - this could also be a menu such as below:
	// 1) I need customer support!
	// 2) How do I add a customer?
	// 3) etc...
	// - if we want to make this extremely simple, we could just have it be a page detailing where they can 
	// reach us for help (i.e. email us; or call us at this number)
	// 
	// 3.5) Change login Credentials
	// - this section allows the user to change their credentials
	// - just ask for password
	// - if the password is right, basically follow creating a new login and we save to sql
	//
	// 
	// Notes:
	// - to make it easier for us we could integrate getch() for our menu system
	// - that way we only need actual validation for the sql parts where we're asking for information directly
	//
	//








	// 1) initialize a database connection objects
	MYSQL* databaseObject = mysql_init(NULL);
	if (databaseObject == NULL) // If the object is NULL, it didnt work.
	{
		printf("Error! DB is null!");
		return EXIT_FAILURE;

		/*
		*
		* THIS CLOSE MUST BE PUT INTO THE EXIT PATH OF THE SWITCH STATEMENT! THE DB MUST BE CLOSED!
		*/

		mysql_close(databaseObject);
	}

	if (ConnectToDatabase(databaseObject, DEFAULT_DATABASE_SERVER_ADDRESS, DEFAULT_DATABASE_USERNAME, DEFAULT_DATABASE_PASSWORD, DEFAULT_DATABASE_NAME))
	{
		printf("Connected to database: \"%s\"!\n\n", DEFAULT_DATABASE_NAME);
	}
	else
	{
		printf("Unable to connect to database:\"%s\"!\n\n", DEFAULT_DATABASE_NAME);
		return EXIT_FAILURE;
		// This is where the program goes to die, instead of the switch.
	}

	// Start of main menu
	int exitProgram = 0;


	/* Below are some test cases. Following along will show you changes to the database as well as what the user sees. It also
	*  includes errors the user can make and how the program handles it.
	*/
	while (!exitProgram)
	{
		printf("Please enter a menu selection: \n");
		printf("\t1) Create new rental record...\n");
		printf("\t2) Update customer details...\n");
		printf("\t3) Read customer rental history...\n");
		printf("\t4) Delete customer records...\n");
		printf("\t5) EXIT PROGRAM\n");

		// Get user menu selection
		int menuItem = GetIntegerFromUser();

		switch (menuItem)
		{
			/*	To create a new rental record enter 1. Enter a customer id that's valid such as 20. This will get you the customer
			*	SHARON ROBINSON. Enter the movie inventory_id such as 20. This will get you a movie called AMELIE HELLFIGHTERS and
			*	it is available for rent. Entering the staff_id as 1 will find the staff member Mike Hillyer. This will get be
			*	a successful rental entry.
			*
			*	Entering any information that doesn't exist will not add a new rental entry. For example, entering the value 1000 for
			*	for customer_id will tell the user that that customer Id doesn't exist, so the rental entry can't be added.
			*/

		case 1:
			//printf("\nAdd new rental transaction - Selected item #%d\n", menuItem);
			//if (!AddNewRental(databaseObject))
			//{
			//	printf("Did not successfully add new rental entry!\n\n");
			//}
			//else
			//{
			//	printf("Rental entry successful!!\n\n");
			//}

			break;

			/*	To update a customer's information enter 2. Enter Y to confirm you want to update a customer's information. Entering
			*	a N will return you to the menu. After entering Y, user is prompted to enter the ID of the customer they want to
			*	update. Entering characters will tell the user it's an invalid entry and to try again.In this example, we are going to
			*	input 20, which is the customer SHARON ROBINSON. User is then prompted to select an option they'd like to update,
			*   which is detailed below.
			*
			*	Choose to update First Name by inputting 1. When inputting a name, any whitespace will cause the user to go back to
			*	choosing an update option and alert them to not include white space. Inputting a first name such as Lily will update
			*	the name from SHARON to Lily. The user is then returned to the main menu.
			*
			*	Choose to update Last Name by inputting 2. The process for this option is the same as the First Name option, except
			*	the last name of the customer is updated. If we input Jones, the last name updates from ROBINSON to Jones.
			*
			*	Choose Email by inputting 3. If we input jjones@mail.com, the email updates from SHARON.ROBINSON@sakilacustomer.org
			*	to jjones@mail.com. The email input must include an '@' symbol and end in '.com'. Failure to follow this format will
			*	send an invalid entry message to the user and return them to the update menu. For example, inputting '5', 'mail.com',
			*	or 'jones@com' will trigger this response.
			*
			*	Choose Address by inputting 4. If we input 10, the address updates from 24 to 10. The inputted id must be between 1
			*	and 605 since those are the available addresses. So inputting any characters such as 'address' or 'd' will notify
			*	user it's invalid and allow them to re-enter another input. Inputting integers out of the range such as '0' or '606'
			*	will inform them the ID is invalid and return them to the update menu.
			*
			*	Choose Return to main menu by inputting 5. This option returns the user to the main menu and alerts them they didn't
			*	update customer information.
			*
			*	Inputting a character such as 'd' or 'menu' will alert the user it's an invalid entry and to try again. Inputting an
			*	integer that isn't listed as an option such as '0' or '6' will alert the user to choose one of the listed options and
			*	allow them to input a valid integer.
			*/


		case 2:
			//printf("\nUpdate Customer Information - Selected item #%d\n", menuItem);
			//if (!UpdateCustomerInformation(databaseObject))
			//{
			//	printf("Did not update customer information!\n\n");
			//}
			//else
			//{
			//	printf("Customer information successfully updated!\n\n");
			//}
			break;

			/*	To read a customer's rental history, input 3. Below are the following inputs:
			*
			*	Enter a customer Id: input 21 to find the records of the customer MICHELLE CLARK. Inputting a customer ID that
			*	doesn't exist such 1000 will alert the user the ID doesn't exist and returns the user to the main menu.
			*
			*	Enter a starting date range: input 1900. Inputting characters such as 'd' or 'date' will alert the user it's invalid
			*	and to enter another input. Inputting an integer out of range such as '0' or '3000' will alert the user to enter an
			*	integer within range.
			*
			*	Enter a starting month: input 1. Error messages are given same as above if characters are inputted or integers out
			*	of range are inputted.
			*
			*	Enter a day: input 1. Error messages are given same as above if characters are inputted or integers out of range
			*	are inputted.
			*
			*	The starting date is 1900-01-01. Now we do the ending date which follows the same process as choosing the starting
			*	date.
			*
			*	For the ending date range, select the year to be 2024, the month to be 12, and the day to be 31. This will give an
			*	ending date of 2024-12-31.
			*
			*	The rental history of MICHELLE CLARK between these two dates is now visible to the user. Some examples of what you
			*	should see are shown below:
			*
			*	Customer Name: MICHELLE CLARK
			*	Movie Title: DWARFS ALTER
			*	Rental Date: 2006-02-14 15:16:03
			*	Return Date: (null)
			*
			*	Customer Name: MICHELLE CLARK
			*	Movie Title: BREAKING HOME
			*	Rental Date: 2005-05-26 15:42:20
			*	Return Date: 2005-05-31 13:21:20
			*
			*	The user is now returned to the main menu.
			*/

		case 3:
			//printf("Complex Query - Viewing Rental History with Filters - Selected item #%d\n", menuItem);
			//if (!CheckRentalHistory(databaseObject))
			//{
			//	printf("Unsuccessful query for rental history, try again!\n\n");
			//}
			//else
			//{
			//	printf("Rental history query successful!\n\n");
			//}

			break;


			/*	To delete a customer, input 4. Below are the following inputs:
			*
			*	User is shown the effects of deleting a customer and asked if they want to proceed. Input 'Y' to proceed with
			*	deletion. If any other input is inputted, the user is returned to the main menu.
			*
			*	Enter customer ID: input 25. Customer is DEBORAH WALKER. This customer has no outstanding rental, so they can be
			*	deleted. The user is shown the deletion process is going forward with the customer's rental, payment, customer info,
			*	and customer address is deleted. The deletion process is concluded and the user is returned to the main menu.
			*
			*	Enter customer ID: input 22. Customer is LAURA RODRIGUEZ. This customer has an outstanding rental, so they can't be
			*	deleted. The user is informed of this and the deletion process is cancelled.
			*
			*	If user inputs an id that doesn't exist such as '1000', the user is told the customer doesn't exist and the deletion
			*	process is cancelled and they are returned to the main menu.
			*/

		case 4:
			//printf("\nDeleting a Customer Record - Selected item #%d\n", menuItem);
			//if (!DeleteCustomerRecord(databaseObject))
			//{
			//	printf("Did not delete customer!\n\n");
			//}
			//else
			//{
			//	printf("Customer deletion successful!!\n\n");
			//}
			break;

			// To exit program, input 5. This exits the loop, which exits the program. If user inputs anything else aside from the
			// availble menu options, they are told it's invalid and to choose a valid menu option.
		case 5:
			printf("EXIT: Selected item #%d\n", menuItem);
			exitProgram = 1;
			break;

		default:
			printf("Invalid menu selection, try again!\n\n");
			break;
		}
	}


	// Close the connection to the DB
	mysql_close(databaseObject);

	return EXIT_SUCCESS;
}





// I just did this so we can test creating new functions while maintaining the old main()
// We can just remove this later when it's time to compile everything into the cohesive program menu
int main()
{

	// 1) initialize a database connection objects
	MYSQL* databaseObject = mysql_init(NULL);
	if (databaseObject == NULL) // If the object is NULL, it didnt work.
	{
		printf("Error! DB is null!");
		return EXIT_FAILURE;

		mysql_close(databaseObject);
	}

	// Used for logging in with program defaults

	if (DatabaseLoginWithProgramDefaults(databaseObject))
	{
		printf("Connected to database: \"%s\"!\n\n", DEFAULT_DATABASE_NAME);
	}
	else
	{
		printf("Unable to connect to database:\"%s\"!\n\n", DEFAULT_DATABASE_NAME);
		return EXIT_FAILURE;
		// This is where the program goes to die, instead of the switch.
	}

	// Used for logging in with user input

	//if (DatabaseLoginWithUserInput(databaseObject))
	//{
	//	printf("Connected to database: \"%s\"!\n\n", DEFAULT_DATABASE_NAME);
	//}
	//else
	//{
	//	printf("Unable to connect to database:\"%s\"!\n\n", DEFAULT_DATABASE_NAME);
	//	return EXIT_FAILURE;
	//	// This is where the program goes to die, instead of the switch.
	//}


	// CODE GOES HERE
	// Customer CREATE testing


	// CUSTOMER table CREATE
	//if (!CreateCustomer(databaseObject))
	//{
	//	printf("Failed to create new customer! - MAIN\n");
	//}
	//else
	//{
	//	printf("Add new customer so far so good!! - MAIN\n");
	//}

	// CUSTOMER table READ
	//if (!ReadCustomer(databaseObject))
	//{
	//	printf("failed to read customer - MAIN!!\n");
	//}
	//else
	//{
	//	printf("FINISHED SUCCESSFUL READ OF CUSTOMER!! - MAIN!!\n");
	//}



	//// BOOK TABLE READ
	//if (!ReadBookTable(databaseObject))
	//{
	//	printf("failed to read BOOK - MAIN!!\n");
	//}
	//else
	//{
	//	printf("FINISHED SUCCESSFUL READ OF BOOK!! - MAIN!!\n");
	//}

	//// ORDER table read
	//if (!ReadOrderTable(databaseObject))
	//{
	//	printf("failed to read ORDER - MAIN!!\n");
	//}
	//else
	//{
	//	printf("FINISHED SUCCESSFUL READ OF ORDER!! - MAIN!!\n");
	//}

	// CREATE BOOK
	//if (!CreateBookEntry(databaseObject))
	//{
	//	printf("Failed to add book!\n");
	//}
	//else
	//{
	//	printf("Book added successfully!\n");
	//}

	// UPDATE CUSTOMER
	if (!UpdateCustomerInformation(databaseObject))
	{
		printf("Failed to update customer information.\n");
	}
	else
	{
		printf("Customer information updated.\n");
	}

	// CREATE ORDER
	//if (!CreateOrder(databaseObject))
	//{
	//	printf("Failed to create order!\n");
	//}
	//else
	//{
	//	printf("Order created successfully!!\n");
	//}

	// CODE STOPS HERE




	// NECESSARY AT END
	// Close the connection to the DB
	mysql_close(databaseObject);

	return EXIT_SUCCESS;

}