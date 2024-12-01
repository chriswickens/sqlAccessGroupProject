#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 500

// Connection details
#define SERVER "localhost"
#define USERNAME "root"
#define PASSWORD "Sah@-123"
#define DEFAULTDATABASE "sakila"

// Date ranges
#define MONTHMIN 1
#define MONTHMAX 12
#define DAYMIN 1
#define DAYMAX 31
#define YEARMIN 1900
#define YEARMAX 2024

// Function prototypes
int GetIntegerFromUser();
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase);
bool CustomerExistsQuery(MYSQL* databaseObject, int customerIdNumber);
bool StaffIdExistsQuery(MYSQL* databaseObject, int staffIdNumber);
bool IsFilmAvailableQuery(MYSQL* databaseObject, int movieIdToCheck);
bool OutstandingRentalsQuery(MYSQL* databaseObject, int customer_id);
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString);
bool CheckRowResult(MYSQL_RES* resultToCheck);
bool UpdateCustomerInformation(MYSQL* databaseObject);
bool UpdateCustomerFirstName(MYSQL* databaseObject, int customer_id, char* customer_name);
bool UpdateCustomerLastName(MYSQL* databseObject, int customer_id, char* customer_lastName);
bool UpdateCustomerEmail(MYSQL* databaseObject, int customer_id, char* customer_email);
bool ValidateEmailAddress(char* address);
bool UpdateCustomerAddressId(MYSQL* databaseObject, int customer_id, int address_id);
void PromptForYesOrNo();
void ClearCarriageReturn(char buffer[]);
void GetDateFromUser(char dateString[]);
bool CheckRentalHistory(MYSQL* databaseObject);

// Function headers for operation functions
bool AddNewRental(MYSQL* databaseObject);
bool DeleteCustomerRecord(MYSQL* databaseObject);


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

// Function to see if customer exists
bool CustomerExistsQuery(MYSQL* databaseObject, int customerIdNumber)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT * FROM customer\n"
		"WHERE customer_id = %d; \n"
		, customerIdNumber);

	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Check if the staff exists
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

// Attempts to execute a query, and lets you know if it failed for some reason
// returns true or false
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString)
{
	if (mysql_query(databaseObject, queryString) != 0)
	{
		printf("Failed on query!\n");
		mysql_close(databaseObject);
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

void PromptForYesOrNo()
{
	char input;

	printf("Type Y or N: ");
	input = getchar();

	// Clear any extra characters from the input buffer
	while (getchar() != '\n');

	// The function does nothing with the input and returns nothing
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


bool UpdateCustomerInformation(MYSQL* databaseObject)
{
	char yesOrNo[MAX_STRING_SIZE];						// temporarily changed to yes to make my life easier
	char newEntry[MAX_STRING_SIZE];			// hold the input for the update
	int menu = 0;							// menu control for choosing what to update
	int newAddressId = 0;					// if updating the address this holds the id for new address
	int customerToUpdate = 0;				// holds the customer id for the person we are updating info for
	bool update = false;					// controls whether or not we are updating something - defaulted to false
	bool updateReturn = false;				// this determines whether or not the update was successful for the user to know
	bool validEmail = false;				// control for if an email is valid or not

	printf("Would you like to update a customer's information? Y/N\n");
	//yesOrNo = getchar();					// defaulting to yes to make my life easier for now...
	fgets(yesOrNo, sizeof(yesOrNo), stdin);

	switch (yesOrNo[0])
	{
	case 'Y':
		printf("You have chosen yes.\n");
		printf("Choose the ID of the customer to update.\n");
		customerToUpdate = GetIntegerFromUser();
		update = true;						// change this to true to access menu for options to update
		break;
	case 'N':
		printf("Returning to menu.\n");		// let the user know they are returning to our default menu
		break;
	default:
		printf("Invalid input. Closing request.\n");		// if someone is stupid and can't read, this probably won't help
		updateReturn = false;				// let's the main() know nothing was updated
	}
	
	if (update)								// if update is true we go here
	{
		while (menu == 0)
		{
			// menu presented to user - populates through every loop
			printf("\nPlease choose what you would like to update:\n");
			printf("1)\tFirst Name\n");
			printf("2)\tLast Name\n");
			printf("3)\tEmail\n");
			printf("4)\tAddress\n");
			printf("5)\tReturn to main menu.\n\n");
			menu = GetIntegerFromUser();					// get their input for which menu item - appears until a success or user enters 5

			switch (menu)
			{
			case 1:
				printf("\nPlease enter a new first name. Do not use any whitespace.\n");

				// get the new name from the user
				fgets(newEntry, sizeof(newEntry), stdin);
				// clear the \n from the entry so it doesn't interfere with the database query
				ClearCarriageReturn(newEntry);

				// if the update of the first name is false then notify the user that it didn't update and reload the menu
				updateReturn = UpdateCustomerFirstName(databaseObject, customerToUpdate, newEntry);
				if (updateReturn == false)
				{
					printf("\nThere was an error. Did not update.\n");
					menu = 0;
				}

				// leave the loop and return to the main interface if menu != 0
				break;
			case 2:
				printf("\nPlease enter a new last name. Do not use any whitespace.\n");

				// get the new last name from the user
				fgets(newEntry, sizeof(newEntry), stdin);
				// clear the \n from the entry so it doesn't interfere with the database query
				ClearCarriageReturn(newEntry);

				// if the update of the last name is false then notify the user that it didn't update and reload the menu
				updateReturn = UpdateCustomerLastName(databaseObject, customerToUpdate, newEntry);
				if (updateReturn == false)
				{
					printf("\nThere was an error. Did not update.\n");
					menu = 0;
				}

				// leave the loop and return to the main interface if menu != 0
				break;
			case 3:
				printf("\nPlease enter a new email address. Do not user any whitespace. It must include an '@' symbol and end in '.com'.\n");

				// get the new email from the user
				fgets(newEntry, sizeof(newEntry), stdin);

				// validate the email by checking for the '@' and for the ending of '.com'
				validEmail = ValidateEmailAddress(newEntry);

				// if the email didn't meet the validation requirements, notify the user and reload the menu
				if (validEmail == false)
				{
					printf("\nThe email address is not valid.\n");
					menu = 0;
					break;
				}

				// if email is valid remove the \n so it doesn't interfere with the database query
				ClearCarriageReturn(newEntry);

				// if the update of the email is false then notify the user that it didn't update and reload the menu
				updateReturn = UpdateCustomerEmail(databaseObject, customerToUpdate, newEntry);
				if (updateReturn == false)
				{
					printf("\nThere was an error. Did not update.\n");
					menu = 0;
				}

				// leave the loop and return to the main interface if menu != 0
				break;
			case 4:
				printf("\nPlease enter a new address id for the customer. It cannot be less than 1 or greater than 605.\n");

				// get an integer from the user
				newAddressId = GetIntegerFromUser();
				// if the integer is not between 1 and 605 inclusive, notify the user it wasn't valid and reload the menu
				if (newAddressId < 1 || newAddressId > 605)
				{
					printf("\nThe address ID was not valid.\n");
					menu = 0;
					break;
				}

				// if the update of the address ID is false then notify the user that it didn't update and reload the menu
				updateReturn = UpdateCustomerAddressId(databaseObject, customerToUpdate, newAddressId);
				if (updateReturn == false)
				{
					printf("\nThere was an error. Did not update.\n");
					menu = 0;
				}

				// leave the loop and return to the main interface if menu != 0
				break;
			case 5:
				printf("\nReturning to main menu.\n");		// if they don't want to update they can exit using this option
				// set the return value for the function to false because no changes were made
				updateReturn = false;

				// leave the loop and return to the main interface if menu != 0
				break;
			default:
				printf("\nPlease chose one of the listed options.\n");		// if they didn't chose a valid option there
				menu = 0;													// this makes sure we go back to the menu and re-prompt them
			}
		}
	}

	return updateReturn; 
}


bool UpdateCustomerFirstName(MYSQL* databaseObject, int customer_id, char* customer_name)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET first_name = '%s'\n"
		"WHERE customer_id = %d\n", customer_name, customer_id);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true; 
}


bool UpdateCustomerLastName(MYSQL* databaseObject, int customer_id, char* customer_lastName)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET last_name = '%s'\n"
		"WHERE customer_id = %d", customer_lastName, customer_id);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true; 
}


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


bool UpdateCustomerEmail(MYSQL* databaseObject, int customer_id, char* customer_email)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET email = '%s'\n"
		"WHERE customer_id = %d", customer_email, customer_id);


	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true; 
}


bool UpdateCustomerAddressId(MYSQL* databaseObject, int customer_id, int address_id)
{
	char query[MAX_STRING_SIZE];

	sprintf(query,
		"UPDATE customer\n"
		"SET address_id = %d\n"
		"WHERE customer_id = %d", address_id, customer_id);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}


bool AddNewRental(MYSQL* databaseObject)
{
	// Start: Get customer_id to add a new rental for
	printf("Please enter the customer_id:");
	int customerIdToCheck = GetIntegerFromUser(); // The ID to check
	printf("Please enter the movie inventory_id:");
	int inventoryIdToCheck = GetIntegerFromUser(); // Get the inventory_id for the movie
	printf("Please enter the staff_id:");
	int staffIdToCheck = GetIntegerFromUser(); // Get the staff ID from the user

	// Check the customer ID
	if (!CustomerExistsQuery(databaseObject, customerIdToCheck))
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
			printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
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
	}
	// End of customer_id check


	// Check if the film is available
	if (!IsFilmAvailableQuery(databaseObject, inventoryIdToCheck))
	{
		printf("Failed to find the film in the database!\n");
		return false;
	}

	MYSQL_RES* checkInventoryResult = mysql_store_result(databaseObject);

	if (checkInventoryResult == NULL)
	{
		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
		return false;
	}

	if (!CheckRowResult(checkInventoryResult))
	{
		printf("\nMOVIE #%d NOT AVAILABLE, do you want to be added to the waitlist?\n", inventoryIdToCheck);
		PromptForYesOrNo();
		return false;
	}
	else
	{
		MYSQL_ROW inventoryRow;
		while ((inventoryRow = mysql_fetch_row(checkInventoryResult)) != NULL)
		{
			printf("MOVIE ID: Records found: MOVIE info: %s\nMOVIE IS AVAILABLE IN INVENTORY FOR RENT\n\n", inventoryRow[0]);
		}
		mysql_free_result(checkInventoryResult);
	}

	// Check the staff ID
	if (!StaffIdExistsQuery(databaseObject, staffIdToCheck))
	{
		// Bad things happened
		printf("Failed to find staff_id!\n");
		return false;
	}

	MYSQL_RES* checkStaffResult = mysql_store_result(databaseObject);

	// If it failed to get any results for some reason
	if (checkStaffResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
		return false;
	}

	// Check the returned rows to see if the query returned anything
	if (!CheckRowResult(checkStaffResult))
	{
		printf("\nERROR: Staff NOT FOUND %d\n", staffIdToCheck);
		return false;
	}
	// The customer DOES exist, print out information!
	else
	{
		MYSQL_ROW staffRow;
		while ((staffRow = mysql_fetch_row(checkStaffResult)) != NULL)
		{
			printf("Staff ID FOUND: Name: %s %s\n\n", staffRow[1], staffRow[2]);
		}
		mysql_free_result(checkStaffResult);
	}
	// END of STAFF_ID CHECKS

	// If it gets to this point, time to add the rental
	char addRentalQuery[MAX_STRING_SIZE];
	sprintf(addRentalQuery,
		"INSERT INTO rental (rental_date, inventory_id, customer_id, return_date, staff_id) "
		"VALUES (NOW(), %d, %d, NULL, %d);",
		inventoryIdToCheck, customerIdToCheck, staffIdToCheck);

	if (!SendQueryToDatabase(databaseObject, addRentalQuery))
	{
		printf("Failed to add new rental entry!\n");
		return false;
	}

	// Added the rental, return true!
	return true;
}


// Option 2
bool DeleteCustomerRecord(MYSQL* databaseObject)
{
	// Prompt user to confirm deleting a customer and explain implications.
	printf("You have chosen to delete a customer record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) Customer's personal information will be permanently deleted from the database.\n");
	printf("\t2) All rental records associated with this customer will be deleted from the database.\n");
	printf("\t3) All payment records associated with this customer will be deleted from the database.\n");
	printf("\t4) The address associated with this customer will be deleted from the database.\n\n");
	printf("Would you like to proceed with deleting a customer?\n\n");
	printf("Enter 'Y' to proceed or any input to cancel.\n");

	// Get char input from user
	//char input;

	// printf("Type Y or N: ");
	//char input = getchar();
	
	char yesOrNo[MAX_STRING_SIZE];						// temporarily changed to yes to make my life easier

	fgets(yesOrNo, sizeof(yesOrNo), stdin);


	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (yesOrNo == 'Y')
	{
		// Prompt customer id
		printf("You've chosen to proceed with deletion.\n\n");
		printf("Please enter the customer_id:");
		int customerIdToCheck = GetIntegerFromUser(); // The ID to check

		// Check Customer Id
		if (!CustomerExistsQuery(databaseObject, customerIdToCheck))
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
	}
	else
	{
		printf("Deletion Process cancelled...\n");

		return false;
	}

	// Deleted the customer, return true!
	return true;
}

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

bool CheckRentalHistory(MYSQL* databaseObject)
{
	printf("Please enter a customer ID to check: ");
	int customerIdToCheck = GetIntegerFromUser();
	// CHECK IF THE CUSTOMER EXISTS FIRST
	if (!CustomerExistsQuery(databaseObject, customerIdToCheck))
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
	}


	// Get the date range
	printf("Please enter the starting date range: ");
	char startDate[MAX_STRING_SIZE];  // Static buffer to hold the formatted date
	GetDateFromUser(startDate);
	printf("Start Date Date: %s\n", startDate);

	printf("Please enter the ending date range: ");
	char endDate[MAX_STRING_SIZE];  // Static buffer to hold the formatted date
	GetDateFromUser(endDate);
	printf("End Date Date: %s\n", endDate);

	char query[MAX_STRING_SIZE];

	sprintf(query,
		"SELECT CONCAT(c.first_name, ' ', c.last_name) AS customer_name, f.title, r.rental_date, r.return_date "
		"FROM rental r "
		"JOIN customer c ON r.customer_id = c.customer_id "
		"JOIN inventory i ON r.inventory_id = i.inventory_id "
		"JOIN film f ON i.film_id = f.film_id "
		"WHERE c.customer_id = %d "  // Customer ID
		"AND r.rental_date BETWEEN '%s' AND '%s' "  // Date range
		"ORDER BY r.rental_date DESC;",
		customerIdToCheck, startDate, endDate
	);

	if (!SendQueryToDatabase(databaseObject, query))
	{
		printf("Error sending query to DB...\n");
		return false;
	}

	// Store the result from the query
	MYSQL_RES* rentalHistoryResult = mysql_store_result(databaseObject);

	// If the result is null, there was no result
	if (rentalHistoryResult == NULL)
	{
		// Print the SQL error
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	// If the result was NO rows, the customer didnt exist
	if (!CheckRowResult(rentalHistoryResult))
	{
		printf("No rental history found!\n");
		return false;
	}
	// The result has at LEAST ONE row, the customer DOES exist!
	else
	{
		MYSQL_ROW historyRow; // Get the rows using MYSQL_ROW for printing
		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
		printf("\n---------START OF Rental History---------\n");
		while ((historyRow = mysql_fetch_row(rentalHistoryResult)) != NULL)
		{
			printf("Customer Name: %s\nMovie Title: %s\nRental Date: %s\nReturn Date: %s\n\n",
				historyRow[0], historyRow[1], historyRow[2], historyRow[3]);

		}
		printf("\n---------END OF Rental History---------\n");
	}
	return true;
}


int GetIntegerFromUser()
{
	char userInput[MAX_STRING_SIZE] = { 0 };
	int inputAsInt = 0;
	fgets(userInput, MAX_STRING_SIZE, stdin);
	ClearCarriageReturn(userInput);

	while (sscanf(userInput, "%i", &inputAsInt) != 1 || inputAsInt < 0)
	{
		printf("Invalid entry, try again: ");
		fgets(userInput, MAX_STRING_SIZE, stdin);
	}

	return inputAsInt;
}


int main()
{
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

	if (ConnectToDatabase(databaseObject, SERVER, USERNAME, PASSWORD, DEFAULTDATABASE))
	{
		printf("Connected to database: \"%s\"!\n\n", DEFAULTDATABASE);
	}
	else
	{
		printf("Unable to connect to database:\"%s\"!\n\n", DEFAULTDATABASE);
		return EXIT_FAILURE;
		// This is where the program goes to die, instead of the switch.
	}

	// Start of menu
	int exitProgram = 0;

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
		case 1:
			printf("\nAdd new rental transaction - Selected item #%d\n", menuItem);
			if (!AddNewRental(databaseObject))
			{
				printf("Did not successfully add new rental entry!\n\n");
			}
			else
			{
				printf("Rental entry successful!!\n\n");
			}

			break;

		case 2:
			printf("\nUpdate Customer Information - Selected item #%d\n", menuItem);
			if (!UpdateCustomerInformation(databaseObject))
			{
				printf("Unsuccessful query for updating customer information, try again!\n\n");
			}
			else
			{
				printf("Customer information successfully updated!\n\n");
			}
			break;

		case 3:
			printf("Complex Query - Viewing Rental History with Filters - Selected item #%d\n", menuItem);
			if (!CheckRentalHistory(databaseObject))
			{
				printf("Unsuccessful query for rental history, try again!\n\n");
			}
			else
			{
				printf("Rental history query successful!\n\n");
			}

			break;

		case 4:
			printf("\nDeleting a Customer Record - Selected item #%d\n", menuItem);
			if (!DeleteCustomerRecord(databaseObject))
			{
				printf("Unable to delete customer, error during operation!\n\n");
			}
			else
			{
				printf("Customer deletion successful!!\n\n");
			}
			break;

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