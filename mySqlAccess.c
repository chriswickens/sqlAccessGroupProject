#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#pragma warning(disable : 4996)

#define MAX_STRING_SIZE 250
// Function prototypes BECAUSE FUCKING C GOD DAMNIT
int GetInteger();
bool ConnectToDatabase(MYSQL* databaseObject, char* server, char* userName, char* password, char* defaultDatabase);
bool CheckIfCustomerExists(MYSQL* databaseObject, int customerIdNumber);
bool CheckIfFilmIsAvailable(MYSQL* databaseObject, int movieIdToCheck);
bool SendQueryToDatabase(MYSQL* databaseObject, char* queryString);
bool CheckRowResult(MYSQL_RES* resultToCheck);

void clearCarriageReturn(char buffer[]); // Stealing Seans valor, AGAIN. Because C.

// Gets an integer, DUH.
int GetInteger()
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

bool CheckIfCustomerExists(MYSQL* databaseObject, int customerIdNumber)
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


// RETURNS true/false based on return of SENDQUERYTODB
// PART ONE AFTER getting CUSTOMER_ID
bool CheckIfFilmIsAvailable(MYSQL* databaseObject, int movieIdToCheck)
{
	char newQuery[MAX_STRING_SIZE]; // Where the query will be stored.

	// Create the SQL query string and store it in the 'query' char array
	sprintf(newQuery,
		"SELECT f.title, COUNT(i.inventory_id) AS available_inventory\n"
		"FROM film f\n"
		"JOIN inventory i ON f.film_id = i.film_id\n"
		"LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL\n"
		"WHERE f.film_id = %d  -- Where the FILM ID goes USING %d from the ABOVE insert to simulate NOT RENTALBLE\n"
		"GROUP BY f.film_id, f.title\n"
		"HAVING COUNT(i.inventory_id) > COUNT(r.inventory_id);",
		movieIdToCheck, movieIdToCheck); // Pass the values in to use them in sprintf

	//printf("SENDING THIS: %s", newQuery);

	// Send the query
	if (!SendQueryToDatabase(databaseObject, newQuery))
	{
		// Query was NOT successful
		return false;
	}

	return true;
}




// Sends a query, and lets you know if it failed for some reason
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

// Checks how many rows were returns in the MYSQL_RES (result)
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

void clearCarriageReturn(char buffer[])
{
	char* whereCR = strchr(buffer, '\n');
	if (whereCR != NULL)
	{
		*whereCR = '\0';
	}
}

int main()
{
	// BIG NOTE: If SendQueryToDatabase FAILS, it is BAD, time to exit! (most functions will use this!)



	// Server crap
	const char* server = "localhost";
	const char* userName = "root";
	const char* password = "cwickens01";
	const char* defaultDatabase = "sakila";

	// 1) initialize a database connection objects
	MYSQL* databaseObject = mysql_init(NULL);
	if (databaseObject == NULL) // If the object is NULL, it didnt work.
	{
		printf("Error! DB is null!");
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	// 2) connect the database - turned into a function
	if (ConnectToDatabase(databaseObject, server, userName, password, defaultDatabase))
	{
		printf("Connected!\n");
	}
	else
	{
		printf("Did not connect! EEP!\n");
	}


	/*
	*
	* TRY TO GET CUSTOMER
	*
	*/
	printf("------ Does Customer Exist Testing ------\n\n");

	int customerIdToCheck = 11; // The ID to check

	if (!CheckIfCustomerExists(databaseObject, customerIdToCheck))
	{
		// Bad things happened
		printf("Failed to find customer!\n");
		return EXIT_FAILURE;
	}
	else
	{
		MYSQL_RES* customerResult = mysql_store_result(databaseObject);

		// If it failed to get any results for some reason
		if (customerResult == NULL)
		{
			// Print the SQL error
			printf("Failed to get the result set! %s", mysql_error(databaseObject));
			return EXIT_FAILURE;
		}

		// Check if the result set is empty (customer does not exist)
		// Check the mysql_num_rows to see if any rows were returned.
		if (!CheckRowResult(customerResult))
		{
			printf("Customer with ID %d does not exist.\n", customerIdToCheck);
		}
		else
		{
			MYSQL_ROW customerRow;
			while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
			{
				printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
			}
		}

		mysql_free_result(customerResult);
	}


	// END OF CUSTOMER CHECKING


	/*
	*
	* BEGIN CHECKING MOVIE AVAILABILITY
	*
	*
	*/
	printf("\n-------- Movie Available Testing -----------\n");

	// Check the film
	 // Note 1001 is NOT available and will return the opposite result!
	// It is setup in my database to be a movie that CANNOT be rented
	int filmToCheck = 1000;
	CheckIfFilmIsAvailable(databaseObject, filmToCheck);

	MYSQL_RES* checkFilmResult = mysql_store_result(databaseObject);

	// If it failed to get any results for some reason
	if (checkFilmResult == NULL)
	{
		// Print the SQL error
		printf("Failed to get the result set! %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
	}

	// Check the returned rows to see if the query returned anything
	if (!CheckRowResult(checkFilmResult))
	{
		printf("\nMOVIE #%d NOT AVAILABLE\n", filmToCheck);
	}
	// The customer DOES exist, print out information!
	else
	{
		MYSQL_ROW movieRow;
		while ((movieRow = mysql_fetch_row(checkFilmResult)) != NULL)
		{
			printf("MOVIE ID: Records found: MOVIE info: %s\nMOVIE IS AVAILABLE IN INVENTORY FOR RENT\n\n", movieRow[0]);
		}
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

	//// Close the connection to the DB
	mysql_close(databaseObject);

	return EXIT_SUCCESS;
}