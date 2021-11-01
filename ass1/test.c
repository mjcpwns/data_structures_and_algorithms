// Sample tests for Assignment 1

////////////////////////////////////////////////////////////////////////
/*

IMPORTANT:
- These tests are by no means comprehensive - you should do your own
  testing by creating functions containing your own tests, and extending
  the switch statement in the main function to call these functions.

*/
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "AVLTree.h"
#include "FlightDb.h"

////////////////////////////////////////////////////////////////////////
// Auxiliary functions

static void usage(char *progName);

static void testDbNewDbFree(void);
static void testDbInsertRecord(void);
static void testDbFindByFlightNumber(void);
static void testDbFindByDepartureAirportDay(void);
static void testDbFindBetweenTimes(void);
static void testDbFindNextFlight(void);

static void doTestDbInsertRecord(Record recs[], int numRecs);
static void doTestDbFindByFlightNumber(FlightDb db, char *flightNumber);
static void doTestDbFindByDepartureAirportDay(FlightDb db,
                                              char *departureAirport,
                                              int day);
static void doTestDbFindBetweenTimes(FlightDb db,
                                     int day1, int hour1, int minute1,
                                     int day2, int hour2, int minute2);
static void doTestDbFindNextFlight(FlightDb db, char *departureAirport,
                                   int day, int hour, int minute);

static FlightDb createSampleDb(void);
static void showRecordList(List l);
static char *dayToName(int day);

////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    if (argc != 2) {
        argv[1] = "6";
    }
    
    switch (atoi(argv[1])) {
        case 1:  testDbNewDbFree();                 break;
        case 2:  testDbInsertRecord();              break;
        case 3:  testDbFindByFlightNumber();        break;
        case 4:  testDbFindByDepartureAirportDay(); break;
        case 5:  testDbFindBetweenTimes();          break;
        case 6:  testDbFindNextFlight();            break;
        default: usage(argv[0]);                    break;
    }
}

static void usage(char *progName) {
    printf("Usage: %s <test number (1-6)>\n", progName);
    exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////

static void testDbNewDbFree(void) {
    // This test just checks that DbNew and DbFree don't cause errors
    FlightDb db = DbNew();
    DbFree(db);
    
    printf("DbNew and DbFree did not crash\n");
}

////////////////////////////////////////////////////////////////////////

static void testDbInsertRecord(void) {
    printf("Testing DbInsertRecord\n\n");

    Record recs[] = {
        RecordNew("QF409", "SYD", "MEL", 0,  7,  5,  90),
        
        // this should be rejected, because it is exactly the same as
        // the one above
        RecordNew("QF409", "SYD", "MEL", 0,  7,  5,  90),
        
        // this should be rejected even though duration is different,
        // because flight number and time are the same, 
        RecordNew("QF409", "SYD", "MEL", 0,  7,  5,  91),
        
        // this should be accepted because the time is different
        RecordNew("QF409", "SYD", "MEL", 0,  7,  6,  90),
        
        // this should be accepted because flight number is different
        RecordNew("QF401", "SYD", "MEL", 0,  7,  5,  90),
        
        // this should be accepted
        RecordNew("VA323", "MEL", "BNE", 6, 11,  0, 110),
    };
    
    doTestDbInsertRecord(recs, 6);
}

static void doTestDbInsertRecord(Record recs[], int numRecs) {
    printf("Creating the DB\n\n");
    FlightDb db = DbNew();
    
    for (int i = 0; i < numRecs; i++) {
        printf("Attempting to insert: ");
        RecordShow(recs[i]);
        printf("\n");
        
        bool result = DbInsertRecord(db, recs[i]);
        printf("Insertion %s\n", result ? "succeeded" : "failed");
        if (!result) RecordFree(recs[i]);
    }
    
    printf("\nDiscarding the DB\n");
    DbFree(db);
}

////////////////////////////////////////////////////////////////////////

static void testDbFindByFlightNumber(void) {
    printf("Testing DbFindByFlightNumber\n");
    printf("Using sample DB from testAss1.c\n\n");
    
    FlightDb db = createSampleDb();
    
    doTestDbFindByFlightNumber(db, "QF409");
    doTestDbFindByFlightNumber(db, "VA323");
    doTestDbFindByFlightNumber(db, "JQ771");
    doTestDbFindByFlightNumber(db, "JQ560");
    doTestDbFindByFlightNumber(db, "QF500");
    doTestDbFindByFlightNumber(db, "QF530");
    
    DbFree(db);
}

static void doTestDbFindByFlightNumber(FlightDb db, char *flightNumber) {
    printf("Searching for flights with flight number %s\n",
           flightNumber);
    
    List l = DbFindByFlightNumber(db, flightNumber);
    
    if (ListSize(l) == 0) {
        printf("No flights found\n");
    } else {
        printf("Found flights:\n");
        showRecordList(l);
    }
    
    printf("\n");
    ListFree(l);
}

////////////////////////////////////////////////////////////////////////

static void testDbFindByDepartureAirportDay(void) {
    printf("Testing DbFindByDepartureAirportDay\n");
    printf("Using sample DB from testAss1.c\n\n");
    
    FlightDb db = createSampleDb();
    
    doTestDbFindByDepartureAirportDay(db, "SYD", 0);
    doTestDbFindByDepartureAirportDay(db, "SYD", 1);
    doTestDbFindByDepartureAirportDay(db, "SYD", 2);
    doTestDbFindByDepartureAirportDay(db, "SYD", 3);
    doTestDbFindByDepartureAirportDay(db, "SYD", 4);
    doTestDbFindByDepartureAirportDay(db, "SYD", 5);
    doTestDbFindByDepartureAirportDay(db, "SYD", 6);
    
    DbFree(db);
}

static void doTestDbFindByDepartureAirportDay(FlightDb db,
                                              char *departureAirport,
                                              int day) {
    printf("Searching for flights from %s on %s\n",
           departureAirport, dayToName(day));
    
    List l = DbFindByDepartureAirportDay(db, departureAirport, day);
    
    if (ListSize(l) == 0) {
        printf("No flights found\n");
    } else {
        printf("Found flights:\n");
        showRecordList(l);
    }
    
    printf("\n");
    ListFree(l);
}

////////////////////////////////////////////////////////////////////////

static void testDbFindBetweenTimes(void) {
    printf("Testing DbFindBetweenTimes\n");
    printf("Using sample DB from testAss1.c\n\n");
    
    FlightDb db = createSampleDb();
    
    // First time will ALWAYS be earlier than second
    // time in ALL tests including automarking. Days
    // of the week will NOT wrap around (e.g., there
    // won't  be tests where day1 is SUNDAY and day2
    // is MONDAY)
    
    // Monday 7am-8am
    doTestDbFindBetweenTimes(db, 0,  7, 0, 0,  8,  0);
    
    // Monday 10am-2pm
    doTestDbFindBetweenTimes(db, 0, 10, 0, 0, 14,  0);
    
    // Monday morning
    doTestDbFindBetweenTimes(db, 0,  0, 0, 0, 11, 59);
    
    // Monday all day
    doTestDbFindBetweenTimes(db, 0,  0, 0, 0, 23, 59);
    
    // Weekend
    doTestDbFindBetweenTimes(db, 5,  0, 0, 6, 23, 59);
    
    // Friday afternoon
    doTestDbFindBetweenTimes(db, 4, 12, 0, 4, 16, 59);
    
    DbFree(db);
}

static void doTestDbFindBetweenTimes(FlightDb db,
                                     int day1, int hour1, int minute1,
                                     int day2, int hour2, int minute2) {
    printf("Searching for flights between %s %02d%02d and %s %02d%02d\n",
           dayToName(day1), hour1, minute1,
           dayToName(day2), hour2, minute2); 

    List l = DbFindBetweenTimes(db,
                                day1, hour1, minute1,
                                day2, hour2, minute2);
    
    if (ListSize(l) == 0) {
        printf("No flights found\n");
    } else {
        printf("Found flights:\n");
        showRecordList(l);
    }
    
    printf("\n");
    ListFree(l);
}

////////////////////////////////////////////////////////////////////////

static void testDbFindNextFlight(void) {
    printf("Testing DbFindNextFlight\n");
    printf("Using sample DB from test.c\n\n");
    
    // FlightDb db = createSampleDb();
    
    // DbFindNextFlight must be able to wrap around to the
    // beginning of the next week if there are no flights later
    // in the week. Note that all flights in the DB repeat every
    // week.
    
    FlightDb db = DbNew();
    // // QF419 from SYD to MEL
    DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 0,  8,  0,  90));
    DbInsertRecord(db, RecordNew("VA323", "MEL", "BNE", 1, 11,  0, 110));
    DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 3,  8,  0,  90));
    // DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 2,  8,  0,  90));
    // DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 5,  8,  0,  90));

    // // VA323 from MEL to BNE
    doTestDbFindNextFlight(db, "MEL", 2,  1, 59);
    
    // doTestDbFindNextFlight(db, "SYD", 0,  7, 6);
    
    // doTestDbFindNextFlight(db, "SYD", 0, 14, 6);
    
    // doTestDbFindNextFlight(db, "SYD", 2, 12, 0);
    
    // doTestDbFindNextFlight(db, "SYD", 6, 14, 0);
    
    DbFree(db);
}

static void doTestDbFindNextFlight(FlightDb db, char *departureAirport,
                                   int day, int hour, int minute) {
    printf("Finding first available flight at %s from %s %02d%02d\n",
           departureAirport, dayToName(day), hour, minute);
    
    Record r = DbFindNextFlight(db, departureAirport, day, hour, minute);
    
    if (r == NULL) {
        printf("No flights found\n");
    } else {
        printf("Found flight:\n");
        RecordShow(r);
        printf("\n");
    }
    
    printf("\n");
}

////////////////////////////////////////////////////////////////////////

static FlightDb createSampleDb(void) {
    FlightDb db = DbNew();

    // Insert approx. 2892960 flight records 
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 24; j++) {
            for (int k = 0; k < 59; k++) {
            DbInsertRecord(db, RecordNew("QF409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QF759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QT759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WF759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("QR759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RE759", "SYD", "MEL", i,  j,  k,  90));
            
            DbInsertRecord(db, RecordNew("MF409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MF759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("MT759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("WM759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RM609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("DD759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("BT759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT709", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("IT719", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT729", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT739", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT769", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RT759", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF409", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF419", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF429", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF439", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF449", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF459", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF509", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF519", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF529", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF539", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF549", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF559", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF609", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF619", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF629", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF639", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF669", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF659", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF309", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF319", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF329", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF339", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF369", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RF359", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW109", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW119", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW129", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW139", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW119", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW159", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW209", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW219", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW229", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW239", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW269", "SYD", "MEL", i,  j,  k,  90));
            DbInsertRecord(db, RecordNew("RW259", "SYD", "MEL", i,  j,  k,  90));
            }
        }
    }

    // // QF419 from SYD to MEL
    // DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 0,  8,  0,  90));
    // DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 2,  8,  0,  90));
    // DbInsertRecord(db, RecordNew("QF419", "SYD", "MEL", 5,  8,  0,  90));

    // // VA323 from MEL to BNE
    // DbInsertRecord(db, RecordNew("VA323", "MEL", "BNE", 1, 11,  0, 110));
    // DbInsertRecord(db, RecordNew("VA323", "MEL", "BNE", 3, 11, 30, 110));
    // DbInsertRecord(db, RecordNew("VA323", "MEL", "BNE", 4, 11,  0, 110));
    // DbInsertRecord(db, RecordNew("VA323", "MEL", "BNE", 6, 11, 30, 110));
    
    // // JQ771 from ADL to MEL
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 1, 11, 55, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 3, 11, 55, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 5, 11, 55, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 6, 11, 55, 105));
    
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 0, 14,  5, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 1, 14,  5, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 2, 14,  5, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 3, 14,  5, 105));
    // DbInsertRecord(db, RecordNew("JQ771", "ADL", "MEL", 4, 14,  5, 105));

    // // JQ560 from MEL to BNE
    // DbInsertRecord(db, RecordNew("JQ560", "MEL", "BNE", 0,  9,  0,  85));
    // DbInsertRecord(db, RecordNew("JQ560", "MEL", "BNE", 1,  9,  0,  85));
    // DbInsertRecord(db, RecordNew("JQ560", "MEL", "BNE", 3,  9,  0,  85));
    // DbInsertRecord(db, RecordNew("JQ560", "MEL", "BNE", 4,  9,  0,  85));
    
    // // QF500 from SYD to BNE
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 0,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 1,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 2,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 3,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 4,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 5,  4,  0,  30));
    // DbInsertRecord(db, RecordNew("QF500", "SYD", "BNE", 6,  4,  0,  30));
    
    // // QF530 from SYD to BNE
    // DbInsertRecord(db, RecordNew("QF530", "SYD", "BNE", 0, 13,  5,  30));
    // DbInsertRecord(db, RecordNew("QF530", "SYD", "BNE", 2, 13,  5,  30));
    // DbInsertRecord(db, RecordNew("QF530", "SYD", "BNE", 3, 13,  5,  30));
    // DbInsertRecord(db, RecordNew("QF530", "SYD", "BNE", 4, 13,  5,  30));
    // DbInsertRecord(db, RecordNew("QF530", "SYD", "BNE", 6, 13,  5,  30));
    
    return db;
}

////////////////////////////////////////////////////////////////////////

static void showRecordList(List l) {
    ListIterator it = ListItNew(l);
    while (ListItHasNext(it)) {
        RecordShow(ListItNext(it));
        printf("\n");
    }
    ListItFree(it);
}

static char *dayToName(int day) {
    assert(day >= 0 && day <= 6);
    
    char *days[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday", "Sunday"
    };
    
    return days[day];
}

