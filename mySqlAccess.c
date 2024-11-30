#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 500

// Connection details
#define SERVER "localhost"
#define USERNAME "root"
#define PASSWORD "cwickens01"
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
void PromptForYesOrNo();
void ClearCarriageReturn(char buffer[]);
void GetDateFromUser(char dateString[]);
bool CheckRentalHistory(MYSQL* databaseObject);


// Function headers for operation functions
bool AddNewRental(MYSQL* databaseObject);
bool DeleteCustomerRecord(MYSQL* databaseObject);
// Gets an integer, DUH.

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
		printf("Failed on query!");
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

// Option 1
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
	printf("Enter 'y' to proceed or any input to cancel.\n");

	// Get char input from user
	//char input;

	// printf("Type Y or N: ");
	char input = getchar();

	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (input == 'y')
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