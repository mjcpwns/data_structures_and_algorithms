
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "FlightDb.h"
#include "AVLTree.h"

struct flightDb {
	Tree byFlightNumberAndTime;
	Tree byDepartureAirportAndDay;
	Tree byDayAndTime;
};

////////////////////////////////////////////////////////////////////////
// Comparison functions

/**
 * Compares two records by flightNumber only and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 * Comparisons are done according to the lexicographical order.
 */
static int compareByFlightNumber(Record r1, Record r2) {
    return strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
}

/**
 * Compares two records by Departure Day only and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 */
static int compareByDepartureDay(Record r1, Record r2) {
    return RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
}

/**
 * Compares two records by Departure Time only and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 */
static int compareByDepartureTime(Record r1, Record r2) {

	int hour_diff = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);

	if (hour_diff) return hour_diff;

	int min_diff = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	
	return min_diff;
}

/**
 * Compares two records by Departure Airport only and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 * Comparisons are done according to the lexicographical order.
 */
static int compareByDepartureAirport(Record r1, Record r2) {
    return strcmp(RecordGetDepartureAirport(r1), RecordGetDepartureAirport(r2));
}

/**
 * Compares two records by flightNumber, Day and Departure Time
 * and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 * Assume that flights of the same flight number always have the same
 * departure and arrival airports.
 */
static int compareByFlightNumberAndTime(Record r1, Record r2) {

	int flight_diff = compareByFlightNumber(r1, r2);

	if (flight_diff) return flight_diff;

	int day_diff = compareByDepartureDay(r1, r2);

	return day_diff ? day_diff : compareByDepartureTime(r1, r2);
}

/**
 * Compares two records by departure day, departure time and flight number
 * and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 */
static int compareByDayAndTime(Record r1, Record r2) {

	int day_diff = compareByDepartureDay(r1, r2);

	if (day_diff) return day_diff;

	int time_diff = compareByDepartureTime(r1, r2);

	return time_diff ? time_diff : compareByFlightNumber(r1, r2);
}

/**
 * Compares two records by departure airport, departure day, departure time
 * and flight number and returns:
 * - A negative number if the first record is less than the second
 * - Zero if the records are equal
 * - A positive number if the first record is greater than the second
 */
static int compareByDepartureAirportAndDay(Record r1, Record r2) {

	int dep_airport_diff = compareByDepartureAirport(r1, r2);

	if (dep_airport_diff) return dep_airport_diff;

	return compareByDayAndTime(r1, r2);
}

////////////////////////////////////////////////////////////////////////

/**
 * Creates a new flight DB. 
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
FlightDb DbNew(void) {

    FlightDb db = malloc(sizeof(*db));

    if (db == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    db->byFlightNumberAndTime = TreeNew(compareByFlightNumberAndTime);
	db->byDepartureAirportAndDay = TreeNew(compareByDepartureAirportAndDay);
	db->byDayAndTime = TreeNew(compareByDayAndTime);

    return db;
}

/**
 * Frees all memory allocated to the given flight DB
 */
void     DbFree(FlightDb db) {
	TreeFree(db->byFlightNumberAndTime, false);
	TreeFree(db->byDepartureAirportAndDay, false);
	TreeFree(db->byDayAndTime, true);
	free(db);
}

/**
 * Inserts  a  flight  record  into the given DB if there is not already
 * record with the same flight number, departure airport, day, hour  and
 * minute.
 * If  inserted successfully, this function takes ownership of the given 
 * record (so the caller should not modify or free it). 
 * Returns true if the record was successfully inserted,  and  false  if
 * the  DB  already  contained  a  record  with  the same flight number,
 * departure airport, day, hour and minute.
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
bool     DbInsertRecord(FlightDb db, Record r) {
	if (TreeInsert(db->byFlightNumberAndTime, r) && 
		TreeInsert(db->byDepartureAirportAndDay, r) && 
		TreeInsert(db->byDayAndTime, r)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Searches  for  all  records with the given flight number, and returns
 * them all in a list in increasing order of  (day, hour, min).  Returns
 * an empty list if there are no such records. 
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByFlightNumber(FlightDb db, char *flightNumber) {
	
	Record dummy_low = RecordNew(flightNumber, "", "", 0, 0, 0, 0);
	Record dummy_high = RecordNew(flightNumber, "", "", 6, 23, 59, 0);

    List l = TreeSearchBetween(db->byFlightNumberAndTime, dummy_low, dummy_high);

    RecordFree(dummy_low);
	RecordFree(dummy_high);

	return l;
}

/**
 * Searches  for all records with the given departure airport and day of
 * week (0 to 6), and returns them all in a list in increasing order  of
 * (hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByDepartureAirportDay(FlightDb db, char *departureAirport,
                                     int day) {

	Record dummy_low = RecordNew("1", departureAirport, "", day, 0, 0, 0);
	Record dummy_high = RecordNew("zzzzzzzz", departureAirport, "", day, 23, 59, 0);

    List l = TreeSearchBetween(db->byDepartureAirportAndDay, dummy_low, dummy_high);

    RecordFree(dummy_low);
	RecordFree(dummy_high);

	return l;
}


/**
 * Searches  for  all  records  between  (day1, hour1, min1)  and (day2,
 * hour2, min2), and returns them all in a list in increasing  order  of
 * (day, hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindBetweenTimes(FlightDb db, 
                            int day1, int hour1, int min1, 
                            int day2, int hour2, int min2) {

	Record dummy_low = RecordNew("1", "", "", day1, hour1, min1, 0);
	Record dummy_high = RecordNew("zzzzzzzz", "", "", day2, hour2, min2, 0);

	List l = TreeSearchBetween(db->byDayAndTime, dummy_low, dummy_high);

    RecordFree(dummy_low);
	RecordFree(dummy_high);

	return l;
}

/**
 * Searches  for  and  returns  the  earliest next flight from the given
 * departure airport, on or after the given (day, hour, min).
 * The returned record must not be freed or modified. 
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
Record   DbFindNextFlight(FlightDb db, char *departureAirport, 
                          int day, int hour, int min) {
							  
	Record dummy = RecordNew("", departureAirport, "", day, hour, min, 0);
    Record r = TreeNext(db->byDepartureAirportAndDay, dummy);
	Record min_dummy = RecordNew("", departureAirport, "", 0, 0, 0, 0);

	// If no record (for departure airport) is found to be on or after the given
	// day, wrap around the function and get the earliest flight of the week 
	if (!r || strcmp(RecordGetDepartureAirport(r), departureAirport)) {
		r = TreeNext(db->byDepartureAirportAndDay, min_dummy);
	}

    RecordFree(dummy);
	RecordFree(min_dummy);

	// If the next flight found still has a different departure airport, 
	// return NULL (indicating no flights found)
	if (strcmp(RecordGetDepartureAirport(r), departureAirport)) return NULL;

    return r;
}

