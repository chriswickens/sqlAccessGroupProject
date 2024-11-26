#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#pragma warning(disable : 4996)

void GetCustomer()
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
	// 2) connect the database
	if (!mysql_real_connect(databaseObject, server, userName, password, defaultDatabase, 0, NULL, 0))
	{
		printf("Failed to connect to the DB: Error %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}


	/*
	*
	* TRY TO GET CUSTOMER
	*
	*/
	int customerIdToCheck = 600;
	char customerIdToString[20] = { "\0" };

	// Translate the customer_id into a string:
	sprintf(customerIdToString, "%d", customerIdToCheck);

	// char* customerQuery = "SELECT * FROM customer WHERE customer_id="; // ORIGINAL LINE, uneditable obviously
	char customerQuery[500] = "SELECT * FROM customer WHERE customer_id=";

	strcat(customerQuery, customerIdToString);

	if (mysql_query(databaseObject, customerQuery) != 0)
	{
		printf("Failed on query!");
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	MYSQL_RES* customerResult = mysql_store_result(databaseObject);

	if (customerResult == NULL)
	{
		printf("failed to get the result set! %s", mysql_error(databaseObject));
		return EXIT_FAILURE;
		// Close the connection to the DB
		mysql_close(databaseObject);
	}

	MYSQL_ROW customerRow;
	while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
	{
		printf("Records found: Customer Name: %s %s\n", customerRow[2], customerRow[3]);
	}

	//bool foundCustomer = false;
	//while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
	//{
	//	// ID, store, first name, last name, email address, address, active, create date, last update
	//	//printf("Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
	//	char custToFind[] = "600";
	//	if (strcmp(customerRow[0], custToFind) == 0)
	//	{
	//		printf("Found customer #%s: Customer name: %s %s\n", custToFind, customerRow[2], customerRow[3]);
	//		foundCustomer = true;
	//	}
	//}
	//if (!foundCustomer)
	//{
	//	printf("No customer with that ID found!!");
	//}

	// Free the result set, done with it!
	mysql_free_result(customerResult);
	//
	//
	//


	/*
	*
	*
	*/
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