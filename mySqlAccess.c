#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <conio.h>

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 500
#define MIN_PRICE 0.01

// Connection details
#define DEFAULT_DATABASE_SERVER_ADDRESS "localhost"
#define DEFAULT_DATABASE_USERNAME "root"
#define DEFAULT_DATABASE_PASSWORD "Sah@-123"
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

// Main Menu Options
#define ADD_CUSTOMER		1
#define ADD_BOOK			2	
#define ADD_ORDER			3
#define READ_CUSTOMER		4	
#define READ_BOOK			5
#define READ_ORDER			6
#define UPDATE_CUSTOMER		7
#define UPDATE_BOOK			8
#define UPDATE_ORDER		9
#define DELETE_CUSTOMER		10
#define DELETE_BOOK			11
#define DELETE_ORDER		12
#define HELP_PAGE			13
#define EXIT_PROGRAM		14


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
void GetString(char* buffer);
bool ValidatePostalCode(char* postalCode);

// Database specific prototypes
bool DatabaseLoginWithUserInput(MYSQL* databaseObject);
bool DatabaseLoginWithProgramDefaults(MYSQL* databaseObject);
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase);
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString);
bool CheckRowResult(MYSQL_RES* resultToCheck);

// CUSTOMER Table CRUD
// Create
bool CreateCustomer(MYSQL* databaseObject);

// Read
bool ReadCustomerTable(MYSQL* databaseObject);
bool CheckCustomerIdExistsQuery(MYSQL* databaseObject, int customerIdNumber);
bool CheckCustomerEmailExistsQuery(MYSQL* databaseObject, char* customerEmail);
bool SearchCustomerTableForEmail(MYSQL* databaseObject, char* emailToCheck);

// UPDATE
bool UpdateCustomerInformation(MYSQL* databaseObject);
bool UpdateCustomerFirstName(MYSQL* databaseObject, int customer_id, char* customer_name);
bool UpdateCustomerLastName(MYSQL* databseObject, int customer_id, char* customer_lastName);
bool UpdateCustomerEmail(MYSQL* databaseObject, int customer_id, char* customer_email);
bool UpdateCustomerAddressId(MYSQL* databaseObject, int customer_id, int address_id);

// Delete
bool DeleteCustomerRecord(MYSQL* databaseObject);
bool SearchCustomerTableById(MYSQL* databaseObject, int customerToUpdate);
void deleteCustomerImplication(void);
bool CheckAddressSharedQuery(MYSQL* databaseObject, int customerIdNumber);
bool CheckAddressQuery(MYSQL* databaseObject, int customerIdNumber);
bool UpdatePublicationYear(MYSQL* databaseObject, int bookId, int year);

// BOOK table CRUD
// CREATE
bool CreateBookEntry(MYSQL* databaseObject);

// READ
bool ReadBookTable(MYSQL* databaseObject);

// UPDATE
bool UpdateBookInformation(MYSQL* databaseObject);
bool UpdatePageCount(MYSQL* databaseObject, int pages, int bookId);
bool UpdateBookTitle(MYSQL* databaseObject, int bookId, char* title);
bool UpdateBookPrice(MYSQL* databaseObject, int bookId, float price);
bool UpdateISBN(MYSQL* databaseObject, int bookId, long long isbnNumber);
bool CheckPublisherIdExists(MYSQL* databaseObject, int publisherIdNumber);
bool SearchBookTableWithId(MYSQL* databaseObject, int bookId);
bool UpdatePublisherId(MYSQL* databaseObject, int publisherID, int bookId);

// DELETE
void deleteBookImplication(void);
bool CheckBookIdExistsQuery(MYSQL* databaseObject, int bookIdNumber);
bool DeleteBookRecord(MYSQL* databaseObject);

/*
* MISC Book table functions
*/
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
bool GetAndDisplayPublisherTable(MYSQL* databaseObject, int* publisherIds, int* size);
bool GetAndDisplayBookTable(MYSQL* databaseObject, int* bookIds, int* size);
bool GetAndDisplayCustomerTable(MYSQL* databaseObject, int* customerIds, int* size);

// UPDATE
bool UpdateOrderInformation(MYSQL* databaseObject);
bool UpdateQuantity(MYSQL* databaseObject, int orderId, int quantity, int bookId);
bool UpdateOrderBooks(MYSQL* databaseObject, int orderId, int bookId, int quantity);

// DELETE
void deleteOrderImplication(void);
bool CheckOrderIdExistsQuery(MYSQL* databaseObject, int orderIdNumber);
bool DeleteOrderRecord(MYSQL* databaseObject);

// Customer Support Prototype
void customerSupportInformation(void);



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
	}
	return year;
}

// Check for white space in a string
bool NoWhitespaceCheck(char* name)
{
	int asciiValue = ' '; // character we are searching for
	char* pointer = NULL;

	pointer = strchr(name, asciiValue);	// searches the user input email address for the ' '
	if (pointer != NULL) // if the pointer find a ' ' character return false
	{
		return false;
	}

	return true;
}

// Prompt for Y/N user choices
char PromptForYesOrNo()
{
	char input = NULL;
	input = getch();

	while (input != 'y' && input != 'Y' && input != 'n' && input != 'N')
	{
		printf("Please try again....\n");
		input = getch();
	}
	return input;
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

	const char emailEnding[MAX_STRING_SIZE] = ".com"; // used to search user string for the first occurrence of '.com'
	char* dotCom = NULL; // pointer used for strstr function
	int ch = '@'; // the character, in ASCII value, that we are searching for '@'
	char* pointer = NULL; // pointer used for strchr function

	pointer = strchr(address, ch); // searches the user input email address for the '@'
	dotCom = strstr(address, emailEnding); // searches for the first occurrence of '.com' -- this could possible be tricked?

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
	// Check 6 pieces of string
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

	// Attempt connection
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
	// Attempt connection using program defaults
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

		// Connection failed
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
		return false;
	}
	// The query was successful!
	return true;
}

// Checks to see if any rows were returned from a MYSQL_RESULT
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

/*
* ----------------------------------------------------
* CUSTOMER TABLE CRUD FUNCTIONS START HERE
*/

// Create functions
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

	// Check if the customer email does exist and return to main menu if they do
	if (SearchCustomerTableForEmail(databaseObject, email))
	{
		printf("Cannot add customer, a customer with the email %s already exists!\nPress any key to return to main menu...\n", email);
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

		// If they do want to use the existing ID
		// No more processing is necessary
		getPostalCode = false;
	}

	// This will be skipped in the event the user wishes to re-use
	// an existing address that was found, otherwise it will run to complete getting
	// the address postal code, and it will create the new address to be used for the new customer
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
			// CreateNewAddress was unsuccessful
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

// Read functions
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

// Update functions
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
					didItWork = CreateNewAddress(databaseObject, streetNumber, streetName, postalCode);
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

// Delete functions
// 
// Delete customer record
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

		// Notify user that customer's information is being deleted
		printf("\nProceeding to delete customer's records...\n\n");

		// Delete OrderProduct records that only have this book as part of its order
		char deleteOrderProductQuery[MAX_STRING_SIZE];
		sprintf(deleteOrderProductQuery, "DELETE FROM OrderProduct WHERE OnlineOrderId = (SELECT OnlineOrderId FROM OnlineOrder WHERE CustomerId = %d);", customerIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOrderProductQuery))
		{
			printf("OrderProduct records deletion failed!\n");
			return false;
		}
		printf("OrderProduct records deletion successful!\n");

		// Delete OnlineOrder records
		char deleteOnlineOrderQuery[MAX_STRING_SIZE];
		sprintf(deleteOnlineOrderQuery, "DELETE FROM OnlineOrder WHERE CustomerId = %d;", customerIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOnlineOrderQuery))
		{
			printf("OnlineOrder records deletion failed!\n");
			return false;
		}
		printf("OnlineOrder records deletion successful!\n");

		// Saves address count result
		char addressResult[MAX_STRING_SIZE] = "";

		// Saves address id result from sql
		char addressIdString[MAX_STRING_SIZE] = "";

		// Saves address Id integer
		int addressIdInteger = 0;

		// Get Address Id for customer in case we need to delete the address if it's not shared between customers
		if (!CheckAddressQuery(databaseObject, customerIdToCheck))
		{
			// The query was NOT valid!
			printf("Failed to find address in the database!\n");
			return false;
		}
		// The query was valid
		else
		{
			// Store the result from the query
			MYSQL_RES* addressIdResult = mysql_store_result(databaseObject);

			// If the result is null, there was no result
			if (addressIdResult == NULL)
			{
				// Print the SQL error
				printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
				return false;
			}

			// If the result has NO rows, the address didnt exist
			if (!CheckRowResult(addressIdResult))
			{
				printf("Address for customer ID %d does not exist.\n", customerIdToCheck);
				return false;
			}
			// The result has at LEAST ONE row, the address DOES exist!
			else
			{
				MYSQL_ROW addressRow; // Get the rows using MYSQL_ROW for printing
				// Iterate over the row data until it is reading null, and print each entry (probably only 1)
				while ((addressRow = mysql_fetch_row(addressIdResult)) != NULL)
				{
					printf("Address ID: Records found: Address ID: %s\n\n", addressRow[0]);
					strcpy(addressIdString, addressRow[0]);
					printf("addressIdString = %s\n", addressIdString);
					// Convert address id string to integer
					addressIdInteger = atoi(addressIdString);
					printf("address id int = %d\n", addressIdInteger);
				}
			}
			// Free the result for the address so memory isnt still consumed by it
			mysql_free_result(addressIdResult);
		}


		// Check Customer Id
		if (!CheckAddressSharedQuery(databaseObject, customerIdToCheck))
		{
			// The query was NOT valid!
			printf("Failed to find customer in the database!\n");
			return false;
		}
		// The query was valid
		else
		{
			// Store the result from the query
			MYSQL_RES* addressCountResult = mysql_store_result(databaseObject);

			// If the result is null, there was no result
			if (addressCountResult == NULL)
			{
				// Print the SQL error
				printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
				return false;
			}

			// If the result has NO rows, the customer didnt exist
			if (!CheckRowResult(addressCountResult))
			{
				printf("There's no rows, so there's no count.\n");
				return false;
			}
			// The result has at LEAST ONE row, the customer DOES exist! which means i can delete them
			else
			{
				MYSQL_ROW addressCountRow; // Get the rows using MYSQL_ROW for printing
				// Iterate over the row data until it is reading null, and print each entry (probably only 1)
				while ((addressCountRow = mysql_fetch_row(addressCountResult)) != NULL)
				{
					printf("Records found: address count: %s\n\n", addressCountRow[0]);
					strcpy(addressResult, addressCountRow[0]);
					printf("addressresult = %s\n", addressResult);


					// If the result is equal to 0, then there's only one address, so you can delete the address since, it's not shared.
					if ((strcmp(addressResult, "1")) == 0)
					{
						// Delete customer records
						char deleteCustomerQuery[MAX_STRING_SIZE];
						sprintf(deleteCustomerQuery, "DELETE FROM customer WHERE customerid = %d;", customerIdToCheck);

						if (!SendQueryToDatabase(databaseObject, deleteCustomerQuery))
						{
							printf("Customer records deletion failed!\n");
							return false;
						}
						printf("Customer records deletion successful!\n");


						// Delete customer address
						char deleteAddressQuery[MAX_STRING_SIZE];
						sprintf(deleteAddressQuery, "DELETE FROM address WHERE AddressId = %d;", addressIdInteger);

						if (!SendQueryToDatabase(databaseObject, deleteAddressQuery))
						{
							printf("Customer address records deletion failed!\n");
							return false;
						}
						printf("Customer address records deletion successful!\n");

					}
					else
					{
						printf("Customer's address is shared between multiple addresses! Canceled address deletion process...\n");

						// Delete customer records
						char deleteCustomerQuery[MAX_STRING_SIZE];
						sprintf(deleteCustomerQuery, "DELETE FROM customer WHERE customerid = %d;", customerIdToCheck);

						if (!SendQueryToDatabase(databaseObject, deleteCustomerQuery))
						{
							printf("Customer records deletion failed!\n");
							return false;
						}
						printf("Customer records deletion successful!\n");


					}

				}
			}
			// Free the result for the customer so memory isnt still consumed by it
			mysql_free_result(addressCountResult);
		}

		// End of deletion process
		printf("Deletion process concluded.\n");

		return true;
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


/*
*
* MISC CUSTOMER table functions
*
*/

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

// Delete Customer Implications message
void deleteCustomerImplication(void)
{
	printf("You have chosen to delete a customer record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) Customer's personal information will be permanently deleted from the database.\n");
	printf("\t2) OrderProduct relating to this order will be permanently deleted from the database.\n");
	printf("\t3) All order records associated with this customer will be deleted from the database.\n");
	printf("\t4) The address associated with this customer will be deleted from the database.\n\n");
	printf("Would you like to proceed with deleting a customer?\n\n");
	printf("Enter 'Y' to proceed or any input to cancel.\n");
}

// Function to see if address is shared
bool CheckAddressSharedQuery(MYSQL* databaseObject, int customerIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery, "SELECT COUNT(*) AS NumberOfCustomersWithSameAddress FROM Customer WHERE AddressId = (SELECT AddressId FROM Customer WHERE CustomerId = %d);", customerIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Function to check address of customer
bool CheckAddressQuery(MYSQL* databaseObject, int customerIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery, "SELECT AddressId FROM customer WHERE CustomerId = %d;", customerIdNumber);

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

// Create functions
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
	if (!GetAndDisplayPublisherTable(databaseObject, publisherIds, &size))
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

// Read functions
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

// Update functions
bool UpdateBookInformation(MYSQL* databaseObject)
{
	bool didItWork = false;
	int menu = 0;
	int bookID = 0;
	int pageCount = 0;
	int publicationYear = 0;
	int publisherID = 0;
	float price = 0.00;
	long long isbnNumber = 0;
	char bookTitle[MAX_STRING_SIZE];

	printf("Please enter the ID of the book you'd like to update:\n");
	bookID = GetIntegerFromUser();

	if (!SearchBookTableWithId(databaseObject, bookID))
	{
		printf("Book does not exist.\n");
	}

	while (menu == 0)
	{
		printf("\nPlease choose what you would like to update:\n");
		printf("1)\tPage Count\n");
		printf("2)\tTitle\n");
		printf("3)\tYear\n");
		printf("4)\tPrice\n");
		printf("5)\tISBN\n");
		printf("6)\tPublisher ID\n");
		printf("7)\tReturn to main menu\n");
		menu = GetIntegerFromUser();

		switch (menu)
		{
		case 1:
			printf("Enter the new page count:\n");
			pageCount = GetIntegerFromUser();

			didItWork = UpdatePageCount(databaseObject, pageCount, bookID);
			break;

		case 2:
			printf("Please enter a new book title:\n");
			GetString(bookTitle);

			didItWork = UpdateBookTitle(databaseObject, bookID, bookTitle);
			break;

		case 3:
			printf("Please enter a new publication year:\n");
			publicationYear = GetIntegerFromUser();
			if ((publicationYear < YEARMIN) || (publicationYear > YEARMAX))
			{
				printf("Invalid year of publication.\n");

				didItWork = false;
				break;
			}

			didItWork = UpdatePublicationYear(databaseObject, bookID, publicationYear);
			break;

		case 4:
			price = GetFloatFromUser();

			didItWork = UpdateBookPrice(databaseObject, bookID, price);
			break;

		case 5:
			isbnNumber = GetIsbnFromUser();

			didItWork = UpdateISBN(databaseObject, bookID, isbnNumber);
			break;

		case 6:
			printf("Please enter a new publisher ID:\n");
			publisherID = GetIntegerFromUser();

			didItWork = UpdatePublisherId(databaseObject, publisherID, bookID);
			break;

		case 7:
			printf("Returning to main menu.\n");
			break;

		default:
			printf("Please select one of the listed options.\n");
			menu = 0;		// loop back to menu for another prompt
			break;
		}
	}

	return didItWork;
}
bool UpdatePageCount(MYSQL* databaseObject, int pages, int bookId)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET PageCount = %d\n"
		"WHERE BookId = %d;", pages, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
bool UpdateBookTitle(MYSQL* databaseObject, int bookId, char* title)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET Title = '%s'\n"
		"WHERE BookId = %d;", title, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
bool UpdatePublicationYear(MYSQL* databaseObject, int bookId, int year)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET Year = %d\n"
		"WHERE BookId = %d", year, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
bool UpdateBookPrice(MYSQL* databaseObject, int bookId, float price)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET Price = %2lf\n"
		"WHERE BookId = %d;", price, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;

}
bool UpdateISBN(MYSQL* databaseObject, int bookId, long long isbnNumber)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET Isbn = '%lld'\n"
		"WHERE BookId = %d;", isbnNumber, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Delete functions

// Delete book record
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

		// Delete OrderProduct records where OnlineOrderId has multiple books, including this book
		char deleteMultipleOnlineOrderQuery[MAX_STRING_SIZE];
		sprintf(deleteMultipleOnlineOrderQuery, "DELETE FROM OrderProduct WHERE OnlineOrderId IN (SELECT OnlineOrderId FROM OnlineOrder WHERE BookId = %d); ", bookIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteMultipleOnlineOrderQuery))
		{
			printf("OrderProduct records where OnlineOrderId containing multiple books, but including this book ID deletion failed!\n");
			return false;
		}
		printf("OrderProduct records where OnlineOrderId containing multiple books, but including this book ID deletion successful!\n");

		// Delete OrderProduct records that only have this book as part of its order
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
bool CheckPublisherIdExists(MYSQL* databaseObject, int publisherIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM book\n"
		"WHERE PublisherId = %d;"
		, publisherIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

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
		"WHERE BookId = %d;"
		, bookIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

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
	if (!GetAndDisplayCustomerTable(databaseObject, customerIds, &customerArraySize))
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
	if (!GetAndDisplayBookTable(databaseObject, bookIds, &bookArraySize))
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

// Read function
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
bool UpdateOrderInformation(MYSQL* databaseObject)
{
	bool didItWork = false;
	printf("Please enter the order ID of the order you'd like to modify.\n");

	int orderId = GetIntegerFromUser();

	printf("Would you like to:\n");
	printf("1) Add more copies of a book to your order\n");
	printf("2) Add another book to your order\n");

	int menu = GetIntegerFromUser();
	while (menu == 0)
	{
		int bookId = 0;
		switch (menu)
		{
		case 1:
			printf("Enter the book ID you'd like to change and the order ID associated with it.\n");
			printf("Book ID: \n");
			bookId = GetIntegerFromUser();
			if (!SearchBookTableWithId(databaseObject, bookId))
			{
				printf("Book does not exist.\n");
				didItWork = false;
			}
			else
			{
				printf("Quantity: \n");
				int quantity = GetIntegerFromUser();
				didItWork = UpdateQuantity(databaseObject, orderId, quantity, bookId);
			}

			break;

		case 2:
			printf("Enter the book ID you would like to add to your order:\n");
			bookId = GetIntegerFromUser();
			if (!SearchBookTableWithId(databaseObject, bookId))
			{
				printf("Book does not exist.\n");
				didItWork = false;
			}
			else
			{
				printf("Enter the number of copies you'd like to add to the order:\n");
				int quantity = GetIntegerFromUser();
				didItWork = UpdateOrderBooks(databaseObject, orderId, bookId, quantity);
			}

			break;

		case 3:
			printf("Returning to main menu...\n");
			break;

		default:
			printf("Please choose one of the listed options.\n");
			menu = 0;
			break;
		}
	}

	return didItWork;
}
bool UpdateQuantity(MYSQL* databaseObject, int orderId, int quantity, int bookId)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE OnlineOrder\n"
		"SET Quantity = '%d'\n"
		"WHERE OnlineOrderId = %d AND BookId = %d;", quantity, orderId, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
bool UpdateOrderBooks(MYSQL* databaseObject, int orderId, int bookId, int quantity)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE OnlineOrder\n"
		"SET Quantity = '%d'\n"
		"WHERE OnlineOrderId = %d AND BookId = %d;", quantity, orderId, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

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
bool GetAndDisplayPublisherTable(MYSQL* databaseObject, int* publisherIds, int* size)
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
bool GetAndDisplayBookTable(MYSQL* databaseObject, int* bookIds, int* size)
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
bool GetAndDisplayCustomerTable(MYSQL* databaseObject, int* customerIds, int* size)
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
bool UpdatePublisherId(MYSQL* databaseObject, int publisherID, int bookId)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE Book\n"
		"SET publisherId = '%d'\n"
		"WHERE BookId = %d;", publisherID, bookId);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}
bool SearchBookTableWithId(MYSQL* databaseObject, int bookId)
{
	// CHECK IF THE CUSTOMER EXISTS FIRST
	if (!CheckBookIdExistsQuery(databaseObject, bookId))
	{
		// The query was NOT valid!
		printf("Invalid query\n");
		return false;
	}

	// The query was valid
	// Store the result from the query
	MYSQL_RES* bookIdResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (bookIdResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the address does not exist
	if (!CheckRowResult(bookIdResult))
	{
		printf("Book with ID: %d does not exist.\n", bookId);
		return false;
	}

	// The result has at LEAST ONE row, the address DOES exist!
	else
	{
		MYSQL_ROW bookIdRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		if ((bookIdRow = mysql_fetch_row(bookIdResult)) != NULL)
		{
			mysql_free_result(bookIdResult);
			return true;
		}
	}

	// Free the result for the book so memory isnt still consumed by it
	mysql_free_result(bookIdResult);

	// Found the book
	return false;
}


// Delete order functions

// Delete order record
bool DeleteOrderRecord(MYSQL* databaseObject)
{
	// Prompt user to confirm deleting an order and explain implications.
	deleteOrderImplication();

	// Get input from user
	char yesOrNo[MAX_STRING_SIZE] = { "" };
	fgets(yesOrNo, sizeof(yesOrNo), stdin);

	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (strcmp(yesOrNo, "Y\n") == 0)
	{
		// Clear screen
		system("cls");

		// Prompt order id
		printf("You've chosen to proceed with deletion.\n\n");
		printf("Please enter the order's ID:");
		int orderIdToCheck = GetIntegerFromUser(); // The ID to check

		// Check order Id
		if (!CheckOrderIdExistsQuery(databaseObject, orderIdToCheck))
		{
			// The query was NOT valid!
			printf("Failed to find order in the database!\n");
			return false;
		}
		// The query was valid
		else
		{
			// Store the result from the query
			MYSQL_RES* orderResult = mysql_store_result(databaseObject);

			// If the result is null, there was no result
			if (orderResult == NULL)
			{
				// Print the SQL error
				printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
				return false;
			}

			// If the result has NO rows, the order didnt exist
			if (!CheckRowResult(orderResult))
			{
				printf("Order with ID %d does not exist.\n", orderIdToCheck);
				return false;
			}
			// The result has at LEAST ONE row, the order DOES exist!
			else
			{
				MYSQL_ROW orderRow; // Get the rows using MYSQL_ROW for printing
				// Iterate over the row data until it is reading null, and print each entry (probably only 1)
				while ((orderRow = mysql_fetch_row(orderResult)) != NULL)
				{
					printf("OnlineOrder ID: Records found: OnlineOrder ID: %s, OrderDate: %s\n\n", orderRow[0], orderRow[2]);
				}
			}
			// Free the result for the customer so memory isnt still consumed by it
			mysql_free_result(orderResult);
		}

		// If order ID exists, proceed to delete any records related to the order

		// Notify user that order's information is being deleted
		printf("\nProceeding to delete order's records...\n\n");

		// Delete OrderProduct records
		char deleteOrderProductQuery[MAX_STRING_SIZE];
		sprintf(deleteOrderProductQuery, "DELETE FROM OrderProduct WHERE OnlineOrderId = %d;", orderIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOrderProductQuery))
		{
			printf("OrderProduct records deletion failed!\n");
			return false;
		}
		printf("OrderProduct records deletion successful!\n");

		// Delete OnlineOrder records
		char deleteOnlineOrderQuery[MAX_STRING_SIZE];
		sprintf(deleteOnlineOrderQuery, "DELETE FROM OnlineOrder WHERE OnlineOrderId = %d;", orderIdToCheck);

		if (!SendQueryToDatabase(databaseObject, deleteOnlineOrderQuery))
		{
			printf("OnlineOrder records deletion failed!\n");
			return false;
		}
		printf("OnlineOrder records deletion successful!\n");


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

// Delete order implications message
void deleteOrderImplication(void)
{
	printf("You have chosen to delete an order's record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) OnlineOrder's details will be permanently deleted from the database.\n");
	printf("\t2) OrderProduct relating to this order will be permanently deleted from the database.\n");

	printf("Would you like to proceed with deleting an order?\n\n");
	printf("Enter 'Y' to proceed or any input to cancel.\n");
}


// Function to see if order exists based on ID
bool CheckOrderIdExistsQuery(MYSQL* databaseObject, int orderIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM onlineorder\n"
		"WHERE OnlineOrderId = %d; \n"
		, orderIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}


// Customer support information
void customerSupportInformation(void)
{
	printf("\nIf you require assistance, please contact us via email or phone.\n\n");
	printf("Contact Information\n\n");
	printf("Email:\tfinalProject@email.com\n");
	printf("Phone:\t(519) 123-4567\n");
}


int main(void)
{
	// 1) initialize a database connection objects
	MYSQL* databaseObject = mysql_init(NULL);
	if (databaseObject == NULL) // If the object is NULL, it didnt work.
	{
		printf("Error! DB is null!");
		return EXIT_FAILURE;

		mysql_close(databaseObject);
	}

	// Prompt user to confirm if they want to login with defaults or with inputs.
	printf("Do you want to login with user inputs or defaults?\n");
	printf("Enter 'y' to do so. Any other input will login with defaults.\n");

	char yesOrNo = PromptForYesOrNo();

	// If user enters 'Y', then allow them to enter with
	if (yesOrNo == 'Y' || yesOrNo == 'y')
	{
		 //Used for logging in with user input

		if (DatabaseLoginWithUserInput(databaseObject))
		{
			printf("Connected to database: \"%s\"!\n\n", DEFAULT_DATABASE_NAME);
		}
		else
		{
			printf("Unable to connect to database:\"%s\"!\n\n", DEFAULT_DATABASE_NAME);
			return EXIT_FAILURE;
			// This is where the program goes to die, instead of the switch.
		}
	}
	else
	{
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
	}
	// Start of main menu
	int exitProgram = 0;


	while (!exitProgram)
	{

		// Main menu
		printf("\nPlease enter a menu selection: \n");
		printf("\t1) Add customer.\n");
		printf("\t2) Add book.\n");
		printf("\t3) Add order.\n");
		printf("\t4) Look up a customer's details.\n");
		printf("\t5) Look up a book's details.\n");
		printf("\t6) Look up an order's details.\n");
		printf("\t7) Update customer.\n");
		printf("\t8) Update book.\n");
		printf("\t9) Update order.\n");
		printf("\t10) Delete customer.\n");
		printf("\t11) Delete book.\n");
		printf("\t12) Delete order.\n");
		printf("\t13) Customer Support Details.\n");
		printf("\t14) EXIT PROGRAM\n");

		// Get user menu selection
		int menuItem = GetIntegerFromUser();

		switch (menuItem)
		{
		case ADD_CUSTOMER:
			printf("\nAdd new customer - Selected item #%d\n", menuItem);
			if (!CreateCustomer(databaseObject))
			{
				printf("Failed to create new customer! - MAIN\n");
			}
			else
			{
				printf("Add new customer so far so good!! - MAIN\n");
			}

			break;

		case ADD_BOOK:
			printf("\nAdd new book - Selected item #%d\n", menuItem);
			if (!CreateBookEntry(databaseObject))
			{
				printf("Failed to add book!\n");
			}
			else
			{
				printf("Book added successfully!\n");
			}

			break;

		case ADD_ORDER:
			printf("\nAdd new order - Selected item #%d\n", menuItem);
			if (!CreateOrder(databaseObject))
			{
				printf("Failed to create order!\n");
			}
			else
			{
				printf("Order created successfully!!\n");
			}
			break;

		case READ_CUSTOMER:
			printf("\nRead customer table - Selected item #%d\n", menuItem);
			if (!ReadCustomerTable(databaseObject))
			{
				printf("failed to read customer - MAIN!!\n");
			}
			else
			{
				printf("FINISHED SUCCESSFUL READ OF CUSTOMER!! - MAIN!!\n");
			}

			break;

		case READ_BOOK:
			printf("\nRead book table - Selected item #%d\n", menuItem);
			if (!ReadBookTable(databaseObject))
			{
				printf("failed to read BOOK - MAIN!!\n");
			}
			else
			{
				printf("FINISHED SUCCESSFUL READ OF BOOK!! - MAIN!!\n");
			}

			break;

		case READ_ORDER:
			printf("\nRead order table - Selected item #%d\n", menuItem);
			if (!ReadOrderTable(databaseObject))
			{
				printf("failed to read ORDER - MAIN!!\n");
			}
			else
			{
				printf("FINISHED SUCCESSFUL READ OF ORDER!! - MAIN!!\n");
			}

			break;

		case UPDATE_CUSTOMER:
			printf("\nUpdate customer - Selected item #%d\n", menuItem);
			if (!UpdateCustomerInformation(databaseObject))
			{
				printf("Failed to update customer information.\n");
			}
			else
			{
				printf("Customer information updated.\n");
			}

			break;

		case UPDATE_BOOK:
			printf("\nUpdate book - Selected item #%d\n", menuItem);
			if (!UpdateBookInformation(databaseObject))
			{
				printf("Failed to update book information!\n");
			}
			else
			{
				printf("Book information updated!\n");
			}

			break;

		case UPDATE_ORDER:
			printf("\nUpdate Order - Selected item #%d\n", menuItem);
			if (!UpdateOrderInformation(databaseObject))
			{
				printf("Failed to update order information!\n");
			}
			else
			{
				printf("Order information updated!\n");
			}

			break;

		case DELETE_CUSTOMER:
			printf("\nDeleting a customer record - Selected item \n");
			if (!DeleteCustomerRecord(databaseObject))
			{
				printf("Did not delete customer!\n\n");
			}
			else
			{
				printf("Customer deletion successful!!\n\n");
			}
			break;

		case DELETE_BOOK:
			printf("\nDeleting a book record - Selected item \n");
			if (!DeleteBookRecord(databaseObject))
			{
				printf("Did not delete book!\n\n");
			}
			else
			{
				printf("Book deletion successful!!\n\n");
			}
			break;

		case DELETE_ORDER:
			printf("\nDeleting an order record - Selected item \n");
			if (!DeleteOrderRecord(databaseObject))
			{
				printf("Did not delete order!\n\n");
			}
			else
			{
				printf("Order deletion successful!!\n\n");
			}
			
			break;

		case HELP_PAGE:
			customerSupportInformation();
			break;

		case EXIT_PROGRAM:
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