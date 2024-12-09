///* THIS IS OLD CODE/UNUSED CODE. IF YOU DON'T NEED THE CODE IN MAIN C, YOU CAN PUT IT HERE FOR REFERENCE
//
//
//
//
//
//
//
///*
//*
//* CAN BE DELETED ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//* CAN BE DELETED----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
////bool CheckRentalHistory(MYSQL* databaseObject)
////{
////	printf("Please enter a customer ID to check: ");
////	int customerIdToCheck = GetIntegerFromUser();
////	// CHECK IF THE CUSTOMER EXISTS FIRST
////	if (!CustomerExistsQuery(databaseObject, customerIdToCheck))
////	{
////		// The query was NOT valid!
////		printf("Failed to find customer in the database!\n");
////		return false;
////	}
////	// The query was valid
////	else
////	{
////		// Store the result from the query
////		MYSQL_RES* customerResult = mysql_store_result(databaseObject);
////
////		// If the result is null, there was no result
////		if (customerResult == NULL)
////		{
////			// Print the SQL error
////			printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
////			return false;
////		}
////
////		// If the result was NO rows, the customer didnt exist
////		if (!CheckRowResult(customerResult))
////		{
////			printf("Customer with ID %d does not exist.\n", customerIdToCheck);
////			return false;
////		}
////		// The result has at LEAST ONE row, the customer DOES exist!
////		else
////		{
////			MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
////			// Iterate over the row data until it is reading null, and print each entry (probably only 1)
////			while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
////			{
////				printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
////			}
////		}
////
////		// Free the result for the customer so memory isnt still consumed by it
////		mysql_free_result(customerResult);
////	}
////
////
////	// Get the date range
////	printf("Please enter the starting date range: ");
////	char startDate[MAX_STRING_SIZE];  // Static buffer to hold the formatted date
////	GetDateFromUser(startDate);
////	printf("Start Date Date: %s\n", startDate);
////
////	printf("Please enter the ending date range: ");
////	char endDate[MAX_STRING_SIZE];  // Static buffer to hold the formatted date
////	GetDateFromUser(endDate);
////	printf("End Date Date: %s\n", endDate);
////
////	char query[MAX_STRING_SIZE];
////
////	sprintf(query,
////		"SELECT CONCAT(c.first_name, ' ', c.last_name) AS customer_name, f.title, r.rental_date, r.return_date "
////		"FROM rental r "
////		"JOIN customer c ON r.customer_id = c.customer_id "
////		"JOIN inventory i ON r.inventory_id = i.inventory_id "
////		"JOIN film f ON i.film_id = f.film_id "
////		"WHERE c.customer_id = %d "  // Customer ID
////		"AND r.rental_date BETWEEN '%s' AND '%s' "  // Date range
////		"ORDER BY r.rental_date DESC;",
////		customerIdToCheck, startDate, endDate
////	); 
////
////	if (!SendQueryToDatabase(databaseObject, query))
////	{
////		printf("Error sending query to DB...\n");
////		return false;
////	}
////
////	// Store the result from the query
////	MYSQL_RES* rentalHistoryResult = mysql_store_result(databaseObject);
////
////	// If the result is null, there was no result
////	if (rentalHistoryResult == NULL)
////	{
////		// Print the SQL error
////		printf("SQL Query Execution problem, ERROR: %s", mysql_error(databaseObject));
////		return false;
////	}
////
////	// If the result was NO rows, the customer didnt exist
////	if (!CheckRowResult(rentalHistoryResult))
////	{
////		printf("No rental history found!\n");
////		return false;
////	}
////	// The result has at LEAST ONE row, the customer DOES exist!
////	else
////	{
////		MYSQL_ROW historyRow; // Get the rows using MYSQL_ROW for printing
////		// Iterate over the row data until it is reading null, and print each entry (probably only 1)
////		printf("\n---------START OF Rental History---------\n");
////		while ((historyRow = mysql_fetch_row(rentalHistoryResult)) != NULL)
////		{
////			printf("Customer Name: %s\nMovie Title: %s\nRental Date: %s\nReturn Date: %s\n\n",
////				historyRow[0], historyRow[1], historyRow[2], historyRow[3]);
////
////		}
////		printf("\n---------END OF Rental History---------\n");
////	}
////	return true;
////}
//// CAN BE DELETED ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// CAN BE DELETED ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//
///*
//*
//* CAN BE DELETED
//*
//*/
//bool AddNewRental(MYSQL* databaseObject)
//{
//	// Start: Get customer_id to add a new rental for
//	printf("Please enter the customer_id:");
//	int customerIdToCheck = GetIntegerFromUser(); // The ID to check
//	printf("Please enter the movie inventory_id:");
//	int inventoryIdToCheck = GetIntegerFromUser(); // Get the inventory_id for the movie
//	printf("Please enter the staff_id:");
//	int staffIdToCheck = GetIntegerFromUser(); // Get the staff ID from the user
//
//	// Check the customer ID
//	if (!CheckCustomerIdExistsQuery(databaseObject, customerIdToCheck))
//	{
//		// The query was NOT valid!
//		printf("Failed to find customer in the database!\n");
//		return false;
//	}
//
//	// The query was valid
//	else
//	{
//		// Store the result from the query
//		MYSQL_RES* customerResult = mysql_store_result(databaseObject);
//
//		// If the result is null, there was no result
//		if (customerResult == NULL)
//		{
//			// Print the SQL error
//			printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
//			return false;
//		}
//
//		// If the result was NO rows, the customer didnt exist
//		if (!CheckRowResult(customerResult))
//		{
//			printf("Customer with ID %d does not exist.\n", customerIdToCheck);
//			return false;
//		}
//		// The result has at LEAST ONE row, the customer DOES exist!
//		else
//		{
//			MYSQL_ROW customerRow; // Get the rows using MYSQL_ROW for printing
//			// Iterate over the row data until it is reading null, and print each entry (probably only 1)
//			while ((customerRow = mysql_fetch_row(customerResult)) != NULL)
//			{
//				printf("CUSTOMER ID: Records found: Customer ID: %s, Customer Name: %s %s\n", customerRow[0], customerRow[2], customerRow[3]);
//			}
//		}
//
//		// Free the result for the customer so memory isnt still consumed by it
//		mysql_free_result(customerResult);
//	}
//	// End of customer_id check
//
//
//	// Check if the film is available
//	if (!IsFilmAvailableQuery(databaseObject, inventoryIdToCheck))
//	{
//		printf("Failed to find the film in the database!\n");
//		return false;
//	}
//
//	MYSQL_RES* checkInventoryResult = mysql_store_result(databaseObject);
//
//	if (checkInventoryResult == NULL)
//	{
//		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
//		return false;
//	}
//
//	if (!CheckRowResult(checkInventoryResult))
//	{
//		printf("\nMOVIE #%d NOT AVAILABLE, do you want to be added to the waitlist?\n", inventoryIdToCheck);
//		PromptForYesOrNo();
//		return false;
//	}
//	else
//	{
//		MYSQL_ROW inventoryRow;
//		while ((inventoryRow = mysql_fetch_row(checkInventoryResult)) != NULL)
//		{
//			printf("MOVIE ID: Records found: MOVIE info: %s\nMOVIE IS AVAILABLE IN INVENTORY FOR RENT\n\n", inventoryRow[0]);
//		}
//		mysql_free_result(checkInventoryResult);
//	}
//
//	// Check the staff ID
//	if (!StaffIdExistsQuery(databaseObject, staffIdToCheck))
//	{
//		// Bad things happened
//		printf("Failed to find staff_id!\n");
//		return false;
//	}
//
//	MYSQL_RES* checkStaffResult = mysql_store_result(databaseObject);
//
//	// If it failed to get any results for some reason
//	if (checkStaffResult == NULL)
//	{
//		// Print the SQL error
//		printf("SQL Query Execution problem, ERROR: %s\n", mysql_error(databaseObject));
//		return false;
//	}
//
//	// Check the returned rows to see if the query returned anything
//	if (!CheckRowResult(checkStaffResult))
//	{
//		printf("\nERROR: Staff ID NOT FOUND: %d\n", staffIdToCheck);
//		return false;
//	}
//	// The customer DOES exist, print out information!
//	else
//	{
//		MYSQL_ROW staffRow;
//		while ((staffRow = mysql_fetch_row(checkStaffResult)) != NULL)
//		{
//			printf("Staff ID FOUND: Name: %s %s\n\n", staffRow[1], staffRow[2]);
//		}
//		mysql_free_result(checkStaffResult);
//	}
//	// END of STAFF_ID CHECKS
//
//	// If it gets to this point, time to add the rental
//	char addRentalQuery[MAX_STRING_SIZE];
//	sprintf(addRentalQuery,
//		"INSERT INTO rental (rental_date, inventory_id, customer_id, return_date, staff_id) "
//		"VALUES (NOW(), %d, %d, NULL, %d);",
//		inventoryIdToCheck, customerIdToCheck, staffIdToCheck);
//
//	if (!SendQueryToDatabase(databaseObject, addRentalQuery))
//	{
//		printf("Failed to add new rental entry!\n");
//		return false;
//	}
//
//	// Added the rental, return true!
//	return true;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//*/
