#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#pragma warning(disable : 4996)

bool CreateRentalRecord()
{

}


int GetInteger()
{
	int num;
	int valid_input;

	// Loop until a valid integer is entered
	do
	{
		printf("Please enter an integer: ");

		// Try to read an integer
		valid_input = scanf("%d", &num);

		// Check if the input was valid
		if (valid_input != 1)
		{
			// Clear the invalid input from the buffer
			while (getchar() != '\n');
			printf("Invalid input. Please enter a valid integer.\n");
		}
	} while (valid_input != 1);


	return num;
	// Output the valid integer entered
	//printf("You entered: %d\n", num);
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


int main()
{



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
	if (ConnectToDatabase(databaseObject,  server, userName, password, defaultDatabase))
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
	int customerIdToCheck = 600; // The int ID to check
	char customerIdToString[20] = { "\0" }; // The string to store the int ID, since C.

	// Translate the customer_id into a string, because C
	sprintf(customerIdToString, "%d", customerIdToCheck);

	char customerQuery[500] = "SELECT * FROM customer WHERE customer_id=";

	strcat(customerQuery, customerIdToString); // Add the customer ID to the end...BECAUSE C

	// Send the query to the database and check the BOOL return from the function
	if (SendQueryToDatabase(databaseObject, customerQuery))
	{
		printf("Successful query!\n");
	}
	else
	{
		printf("Query failed!(\n");
	}

	MYSQL_RES* customerResult = mysql_store_result(databaseObject);

	// If it failed to get any results for some reason
	if (customerResult == NULL)
	{
		printf("Failed to get the result set! %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
	}

	// Check if the result set is empty (customer does not exist)
	// Check the mysql_num_rows to see if any rows were returned.
	if (mysql_num_rows(customerResult) == 0)
	{
		printf("Customer with ID %d does not exist.\n", customerIdToCheck);
	}
	// The customer DOES exist, print out information!
	else
	{
		MYSQL_ROW customerRow;
		while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
		{
			printf("Records found: Customer Name: %s %s\n", customerRow[2], customerRow[3]);
		}
	}
	// Free the result set, done with it!
	mysql_free_result(customerResult);



	/*
	*
	*
	*/
	printf("\n\n\n\n\n");
	// 3) Setup the query
	const char* query = "SELECT actor_id, first_name, last_name FROM actor WHERE first_name LIKE \'M%\'";
	if (mysql_query(databaseObject, query) != 0)
	{
		printf("Failed on query!");
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	MYSQL_RES* resultSet = mysql_store_result(databaseObject);

	if (resultSet == NULL)
	{
		printf("failed to get the result set! %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	MYSQL_ROW row;

	//while ((row = mysql_fetch_row(resultSet)) != NULL)
	//{
	//	printf("Actor ID: %s, First Name: %s, Last Name: %s\n", row[0], row[1], row[2]);

	//}


	// Free the result set, done with it!
	mysql_free_result(resultSet);

	// Update existing record
	const char* getActorWithActorId1 = "SELECT * FROM actor WHERE actor_id = 1";

	if (mysql_query(databaseObject, getActorWithActorId1) != 0)

	{
		printf("Failed to get actor with value of 1 in actor_id!\n");
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	// Store the result from the response
	MYSQL_RES* resultForActorIdOfOne = mysql_store_result(databaseObject);
	int numberOfRows = mysql_num_rows(resultForActorIdOfOne);
	// if zero, didnt find the actor!
	if (numberOfRows == 0)
	{
		printf("No actord with actor_id of 1!");
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}
	if (numberOfRows == 1)
	{
		MYSQL_ROW rowForActorIdOne = mysql_fetch_row(resultForActorIdOfOne);
		printf("Actor ID: %s, First Name: %s, Last Name: %s\n", rowForActorIdOne[0], rowForActorIdOne[1], rowForActorIdOne[2]);
	}


	const char* updateActor = "UPDATE actor SET first_name = 'John' WHERE actor_id = 1";

	// Close the connection to the DB
	mysql_close(databaseObject);

	return EXIT_SUCCESS;
}