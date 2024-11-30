#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 500
// Function prototypes BECAUSE FUCKING C GOD DAMNIT
int GetIntegerFromUser();
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase);
bool CustomerExistsQuery(MYSQL* databaseObject, int customerIdNumber);
bool StaffIdExistsQuery(MYSQL* databaseObject, int staffIdNumber);
bool IsFilmAvailableQuery(MYSQL* databaseObject, int movieIdToCheck);
bool OutstandingRentalsQuery(MYSQL* databaseObject, int customer_id);
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString);
bool CheckRowResult(MYSQL_RES* resultToCheck);
void PromptForYesOrNo();
void clearCarriageReturn(char buffer[]); // Stealing Seans valor, AGAIN. Because C.


// Function headers for operation functions
bool AddNewRental(MYSQL* databaseObject);

// Gets an integer, DUH.
int GetIntegerFromUser()
{
	char userInput[MAX_STRING_SIZE] = { "\0" };
	int inputAsInt = 0;
	fgets(userInput, MAX_STRING_SIZE, stdin);

	while (sscanf(userInput, "%i", &inputAsInt) != 1 || inputAsInt < 0)
	{
		printf("Invalid entry, try again: ");
		fgets(userInput, MAX_STRING_SIZE, stdin);
	}

	return inputAsInt;
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

	//printf("SENDING THIS: %s", newQuery);

	// Send the query
	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}

// Generates and sends query to see if a customer has an outstanding rental
// returns true/false
/*
* 
* 
* USED FOR THE DELETION OPTION, TO SEE IF THE CUSTOMER HAS AN OUTSTANDING RENTAL, WE CANT DELETE THEM!
* 
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
void clearCarriageReturn(char buffer[])
{
	char* whereCR = strchr(buffer, '\n');
	if (whereCR != NULL)
	{
		*whereCR = '\0';
	}
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

	/*
	*
	* CHECK CUSTOMER_ID
	*
	*/
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
	// End of customer_id check


	/*
	*
	* CHECK FILM INVENTORY
	*
	*/
	if (!IsFilmAvailableQuery(databaseObject, inventoryIdToCheck))
	{
		printf("Failed to find the film in the database!\n");
		return false;
	}

	MYSQL_RES* checkInventoryResult = mysql_store_result(databaseObject);

	if (checkInventoryResult == NULL)
	{
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
		return false;
	}

	if (!CheckRowResult(checkInventoryResult))
	{
		printf("\nMOVIE #%d NOT AVAILABLE\n", inventoryIdToCheck);
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


	/*
	* 
	* CHECK STAFF_ID
	* 
	*/
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
		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
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
		printf("Failed to add new rental entry!");
		return false;
	}

	printf("Rental entry successful!");
	return true;
}


// Delete customer record function
bool DeleteCustomerRecord(MYSQL* databaseObject)
{
	// Prompt user to confirm deleting a customer and explain implications.
	printf("You have chosen to delete a customer record.\n\n");
	printf("Doing so will have the following effects:\n");
	printf("\t1) Customer's personal information will be permanently deleted from the database.\n");
	printf("\t2) All rental records associated with this customer will be deleted from the database.\n");
	printf("\t3) All payment records associtated with this customer will be deleted from the database.\n");
	printf("\t4) The address associated with this customer will be deleted from the database.\n\n");
	printf("Would you like to proceed with deleting a customer?\n\n");
	printf("Enter 'y' to proceed or any input to cancel.\n");

	// Get char input from user
	char input;

	// printf("Type Y or N: ");
	input = getchar();

	// Clear any extra characters from the input buffer
	//while (getchar() != '\n');



	// If user enters 'y', then continue with deletion logic. Otherwise, cancel deletion.
	if (input == 'y')
	{
		//Check------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		printf("User entered 'y', so proceed with deletion. input deletion logic here...\n");

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


		// Check if customer has returned all rented films
			/*

			CHECK IF SOMEONE HAS OUTSTANDING RENTALS!
			This will call the function to send the query to check using the customer ID when the deletion menu option is selected
			It will tell you if the customer has outstanding rentals, which is when you can decide which action to take either delete or not


			*/
		// has returns: 11, 33, 66
		// does not, 1
		int customerIdToCheckForOutstandRentals = customerIdToCheck;
		OutstandingRentalsQuery(databaseObject, customerIdToCheckForOutstandRentals);
		MYSQL_RES* checkOutstandingRentals = mysql_store_result(databaseObject);
		//MYSQL_RES* checkFilmResult = mysql_store_result(databaseObject);

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
			printf("\nCustomer has NO outstanding rentals!\n Proceeding to delete customer's records!");

			// Delete customer rental records
			char deleteRentalQuery[MAX_STRING_SIZE];
			sprintf(deleteRentalQuery, "DELETE FROM rental WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteRentalQuery))
			{
				printf("Failed to delete customer's rental records!\n");
				return false;
			}
			printf("Rental records deletion successful!\n");
			
			// Delete customer payment records
			char deletePaymentQuery[MAX_STRING_SIZE];
			sprintf(deletePaymentQuery, "DELETE FROM payment WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deletePaymentQuery))
			{
				printf("Failed to delete customer's payment records!\n");
				return false;
			}
			printf("Payment records deletion successful!\n");

			// Delete customer payment records
			char deleteCustomerQuery[MAX_STRING_SIZE];
			sprintf(deleteCustomerQuery, "DELETE FROM customer WHERE customer_id = %d;", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteCustomerQuery))
			{
				printf("Failed to delete customer's records!\n");
				return false;
			}
			printf("Customer records deletion successful!\n");			
			
			// Delete customer payment records
			char deleteAddressQuery[MAX_STRING_SIZE];
			sprintf(deleteAddressQuery, "DELETE FROM address WHERE address_id = (SELECT address_id FROM customer WHERE customer_id = %d);", customerIdToCheck);

			if (!SendQueryToDatabase(databaseObject, deleteAddressQuery))
			{
				printf("Failed to delete customer's address records!\n");
				return false;
			}
			printf("Customer address records deletion successful!\n");




			return true;
							
		}
		// Otherwise, cancel the deletion
		else
		{
			MYSQL_ROW outstandingRentalData;
			while ((outstandingRentalData = mysql_fetch_row(checkOutstandingRentals)) != NULL)
			{
				printf("OUTSTANDING RENTAL: MOVIE info:\n\tID: %s\n\ttitle: %s\n\tRental Date: %s\n\tReturn date: %s \n\n", outstandingRentalData[0], outstandingRentalData[3], outstandingRentalData[1], outstandingRentalData[2]);
			}
			mysql_free_result(checkOutstandingRentals);

			printf("This customer has an outstanding rental. Unable to delete customer.\nCancelling deletion process...\n\n");


			return false;

		}


		

	}
	else
	{
		//Check------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		printf("user didn't enter 'y', so cancel\n"); 

		return false;
	}

	return true;
}











void MenuPlaceHolder()
{
	/*
	*
	*
	* YOU FORGOT ABOUT SSCANF, YOU CAN USE IT TO FORMAT EXPECTED STRING INPUT
	* 2025-22-10 for example
	* or chris@home.com
	* Dumbass....
	*
	*
	*/
	int exitProgram = 0;

	while (!exitProgram)
	{
		printf("Welcome to my shitty database program written in C...for reasons!!!\n\n");
		printf("Please enter a menu selection: \n");
		printf("\t1) Create new rental record...\n");
		printf("\t2) Update customer details...\n");
		printf("\t3) Read customer rental history...\n");
		printf("\t4) Delete customer records...\n");

		int menuItem = 10;

		switch (menuItem)
		{
		case 1:
			/*
			* Create function to call for this:
			*
			* Add new rental transaction:
			* Function call to add new transaction:
			* Get the following from the user:
			* int customer_id - GetInteger()
			* int inventory_id - GetInteger()
			* int staff_id - GetInteger()
			*
			* Function for this will:
			* Get information (function)
			* Pass information to function to CREATE.
			*
			*
			*
			* THEN
			* CheckIfFilmIsAvailable(databaseObject, inventory_id)
			* if movie is available:
			*	InsertRentalTransaction()
			* if not available:
			*	Ask user if they want to be on a waitlist (not coding this crap)
			*
			*/
			printf("Add new rental transaction - Selected item #%d\n", menuItem);
			break;

		case 2:
			/*
			* Get the following:
			* int customer_id
			* CheckIfCustomerExists(databaseObject, customer_id)
			* If they do exist, you can update:
			* char first_name
			* char last_name
			* char email
			* int address_id - GetInteger()
			*/
			printf("Update Customer Information - Selected item #%d\n", menuItem);
			break;

		case 3:
			/*
			* View rental history for customer using date range
			* customer_id,
			* start_date (sscanf 2022-10-01)
			* end_date (sscanf too)
			*/
			printf("Complex Query - Viewing Rental History with Filters - Selected item #%d\n", menuItem);
			break;

		case 4:
			/*
			*
			* DELETE RENTAL RECORDS for a customer
			* get
			* customer_id
			*
			* check they have returned all their rentals, if not, do not delete them
			*
			*/
			printf("Deleting a Customer Record - Selected item #%d\n", menuItem);
			break;

		case 5:
			printf("Selected item #%d\n", menuItem);
			exitProgram = 1;
			break;
		default:
			break;
		}
	}
}

int main()
{
	// BIG NOTE: If SendQueryToDatabase FAILS, it is BAD, time to exit! (most functions will use this!)

	// Server crap
	const char* server = "localhost";
	const char* userName = "root";
	const char* password = "Sah@-123";
	const char* defaultDatabase = "sakila";

	// 1) initialize a database connection objects
	MYSQL* databaseObject = mysql_init(NULL);
	if (databaseObject == NULL) // If the object is NULL, it didnt work.
	{
		printf("Error! DB is null!");
		return EXIT_FAILURE;
		// Close the connection to the DB
		
		
		
		/*
		* 
		* THIS CLOSE MUST BE PUT INTO THE EXIT PATH OF THE SWITCH STATEMENT! THE DB MUST BE CLOSED!
		*/
		
		mysql_close(databaseObject);
	}

	// 2) connect the database - turned into a function
	if (ConnectToDatabase(databaseObject, server, userName, password, defaultDatabase))
	{
		printf("Connected!\n");
		// This is where the switch lives
	}
	else
	{
		printf("Did not connect! EEP!\n");
		return EXIT_FAILURE;
		// This is where the program goes to die, instead of the switch.
	}


	/*
	* TRY TO GET CUSTOMER
	*/
	printf("------ DO NOT DELETE THIS ADD NEW RENTAL TESTING ------\n\n");

	// UNCOMMENT TO ADD A NEW RENTAL< WORKING DO NOT TOUCHEY
	//printf("\nCHECK FIRST FUNCTION--------\n");
	//
	//if (!AddNewRental(databaseObject))
	//{
	//	printf("There was a problem with the whole thing...\n");
	//}
	//else
	//{
	//	printf("You entered valid information!\n");
	//}


	//printf("\nEND CHECK FIRST FUNCTION--------\n");


	printf("\n-------- DO NOT DELETE THIS Outstanding RENTALS Testing -----------\n");
	/*

	CHECK IF SOMEONE HAS OUTSTANDING RENTALS!
	This will call the function to send the query to check using the customer ID when the deletion menu option is selected
	It will tell you if the customer has outstanding rentals, which is when you can decide which action to take either delete or not


	*/
	// has returns: 11, 33, 66
	// does not, 1
	int customerIdToCheckForOutstandRentals = 1;
	OutstandingRentalsQuery(databaseObject, customerIdToCheckForOutstandRentals);
	MYSQL_RES* checkOutstandingRentals = mysql_store_result(databaseObject);
	//MYSQL_RES* checkFilmResult = mysql_store_result(databaseObject);

	// If it failed to get any results for some reason
	if (checkOutstandingRentals == NULL)
	{
		// Print the SQL error
		printf("Failed to get the result set! %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
	}

	// Check the returned rows to see if the query returned anything
	if (!CheckRowResult(checkOutstandingRentals))
	{
		printf("\nCustomer has NO outstanding rentals!\n");
	}
	// The customer DOES exist, print out information!
	else
	{
		MYSQL_ROW outstandingRentalData;
		while ((outstandingRentalData = mysql_fetch_row(checkOutstandingRentals)) != NULL)
		{
			printf("OUTSTANDING RENTAL: MOVIE info:\n\tID: %s\n\ttitle: %s\n\tRental Date: %s\n\tReturn date: %s \n\n", outstandingRentalData[0], outstandingRentalData[3], outstandingRentalData[1], outstandingRentalData[2]);
		}
		mysql_free_result(checkOutstandingRentals);
	}


	/*
	*
	*
	*/
	//printf("\n\n\n\n\n");
	//// 3) Setup the query
	//const char* query = "SELECT actor_id, first_name, last_name FROM actor WHERE first_name LIKE \'M%\'";
	//if (mysql_query(databaseObject, query) != 0)
	//{
	//	printf("Failed on query!");
	//	return EXIT_FAILURE;
	//	// Close the connection to the DB
	//	mysql_close(databaseObject);
	//}

	//MYSQL_RES* resultSet = mysql_store_result(databaseObject);

	//if (resultSet == NULL)
	//{
	//	printf("failed to get the result set! %s", mysql_error(databaseObject));
	//	return EXIT_FAILURE;
	//	// Close the connection to the DB
	//	mysql_close(databaseObject);
	//}

	//MYSQL_ROW row;

	////while ((row = mysql_fetch_row(resultSet)) != NULL)
	////{
	////	printf("Actor ID: %s, First Name: %s, Last Name: %s\n", row[0], row[1], row[2]);

	////}


	//// Free the result set, done with it!
	//mysql_free_result(resultSet);

	//// Update existing record
	//const char* getActorWithActorId1 = "SELECT * FROM actor WHERE actor_id = 1";

	//if (mysql_query(databaseObject, getActorWithActorId1) != 0)

	//{
	//	printf("Failed to get actor with value of 1 in actor_id!\n");
	//	return EXIT_FAILURE;
	//	// Close the connection to the DB
	//	mysql_close(databaseObject);
	//}

	//// Store the result from the response
	//MYSQL_RES* resultForActorIdOfOne = mysql_store_result(databaseObject);
	//int numberOfRows = mysql_num_rows(resultForActorIdOfOne);
	//// if zero, didnt find the actor!
	//if (numberOfRows == 0)
	//{
	//	printf("No actord with actor_id of 1!");
	//	return EXIT_FAILURE;
	//	// Close the connection to the DB
	//	mysql_close(databaseObject);
	//}
	//if (numberOfRows == 1)
	//{
	//	MYSQL_ROW rowForActorIdOne = mysql_fetch_row(resultForActorIdOfOne);
	//	printf("Actor ID: %s, First Name: %s, Last Name: %s\n", rowForActorIdOne[0], rowForActorIdOne[1], rowForActorIdOne[2]);
	//}


	//const char* updateActor = "UPDATE actor SET first_name = 'John' WHERE actor_id = 1";





	// Feature 4: Deleting a Customer Record

	printf("----------------------- Feature 4: Deleting a Customer Record ------------------------------\n");
	DeleteCustomerRecord(databaseObject);




	printf("----------------------- Feature 4: Deleting a Customer Record -------------------------------\n");



	//// Close the connection to the DB
	mysql_close(databaseObject);

	return EXIT_SUCCESS;
}