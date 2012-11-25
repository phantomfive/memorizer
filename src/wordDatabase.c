/** Copyright 2012 by Andrew. Usable under the terms of the GPL 3
	 Interfacing with the database turned out to be the most difficult
	 part but it was still easier than writing a custom solution.*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "wordDatabase.h"

//our database handle
static sqlite3 *db=NULL;

//forward declarations
static BOOL init();
static BOOL createSchema();
static BOOL handleError(const char*message, sqlite3_stmt *pStmt);
static BOOL runResultlessQuery(const char *query, int querySize);
static BOOL getSingleIntegerFromQuery(const char *query, int size, int *result);
static BOOL getLastReviewTime(int *lastReviewTime);
static BOOL incrementLastReviewTime();
static BOOL fillWordFromQuery(char *query, WordForReview *word);
static BOOL postpendOmitDuplicates(char *query, int querySize, 
                                   WordForReview*word, int index);

//The following constants are for use with checkVersion()
#define CORRECT_VERSION   1
#define INCORRECT_VERSION 2
#define VERSION_ERROR     3
static int checkVersion();

//----------------------------------------------------------------------------
// public API
//----------------------------------------------------------------------------
BOOL databaseFillWordFromGroup(WordForReview *word, int index, 
                               WordGroupType type) {
	char query[10000];
	if(!init()) return FAIL;
	
	snprintf(query, sizeof(query), "SELECT * FROM WORDS where type=%d",
	                               type);

	postpendOmitDuplicates(query, sizeof(query), word, index);
	printf("chose index %d\n", index);
	if(!fillWordFromQuery(query, &word[index])) {
		fprintf(stderr, "Couldn't fill word from group\n");
		return FAIL;
	}
	return SUCCESS;
}

BOOL databaseFillWordFromGroupOrderByLeastRecent(WordForReview*word, int index,
                                                 WordGroupType type) {
	char query[10000];
	if(!init()) return FAIL;

	snprintf(query, sizeof(query), "SELECT * FROM WORDS where type=%d", type);
	postpendOmitDuplicates(query, sizeof(query), word, index);
	strncatSafe(query, " ORDER BY lastReviewedTime LIMIT 1", sizeof(query));
	

	if(!fillWordFromQuery(query, &word[index])) {
		fprintf(stderr, "Couldn't fill word from group order by least recent\n");
		return FAIL;
	}

	return SUCCESS;
}


BOOL databaseFillWordFromGroupOrderByLeastSkilled(WordForReview*word,int index,
                                                 WordGroupType type) {
	char query[10000];	
	if(!init()) return FAIL;
	
	snprintf(query, sizeof(query),  "SELECT * FROM WORDS where type=%d", type);
	postpendOmitDuplicates(query, sizeof(query), word, index);
	strncatSafe(query, " ORDER BY competencyLevel LIMIT 1", sizeof(query));

	if(!fillWordFromQuery(query, &word[index])){
		fprintf(stderr, "Couldn't fill word ordered by least skilled\n");
		return FAIL;
	}

	return SUCCESS;
}


BOOL databaseGetCountForWordGroup(WordGroupType type, int *count) {
	char query[1000];
	if(!init()) return FAIL;

	snprintf(query,sizeof(query),
	               "SELECT COUNT (*) FROM WORDS WHERE type=%d", type);

	if(!getSingleIntegerFromQuery(query, sizeof(query), count)) {
		fprintf(stderr, "Failed getting count for word group\n");
		return FAIL;
	}

	return SUCCESS;
}

BOOL databaseUpdateWord(const WordForReview *word) {
	int lastReviewedTime = -1;
	char query[1000];
	if(!init()) return FAIL;

	if(!getLastReviewTime(&lastReviewedTime)) {
		fprintf(stderr, "Couldn't get last review time\n");
		return FAIL;
	}

	snprintf(query, sizeof(query),
	         "UPDATE WORDS SET language='%s', localWord='%s',"\
				                 "foreignWord='%s', competencyLevel=%d, "\
									  "type=%d, lastReviewedTime='%d' "\
									  " WHERE id=%d",
			   word->language, word->localWord, word->foreignWord,
				word->competencyLevel, word->type, lastReviewedTime, word->id);
	
	printf("running query: %s\n", query);
	
	if(!runResultlessQuery(query, sizeof(query))) {
		printf("Couldn't update word!\n");
		return FAIL;
	}

	if(!incrementLastReviewTime()) {
		fprintf(stderr, "Updated word, but couldn't update review time!\n");
		return SUCCESS;
	}
	return SUCCESS;
}

BOOL databaseAddWord(const WordForReview *word) {
	char query[1000];
	int lastReviewedTime=-1;
	if(!init()) return FAIL;

	
	if(!getLastReviewTime(&lastReviewedTime)) {
		fprintf(stderr, "Couldn't get last reviewed time!\n");
		return FAIL;
	}

	snprintf(query,sizeof(query), 
	     "INSERT INTO words\n"\
	     "(language, localWord, foreignWord, competencyLevel, "\
		  "type,lastReviewedTime)\n"\
		  "VALUES\n"\
		  "('%s', '%s', '%s', %d, %d, %d)\n",
		  word->language, word->localWord, word->foreignWord,
		  word->competencyLevel, word->type, lastReviewedTime);
	
	if(!runResultlessQuery(query, sizeof(query))) {
		printf("Couldn't add word!\n");
		return FAIL;
	}
	
	if(!incrementLastReviewTime()) {
		fprintf(stderr, "Added word, but couldn't update review time!\n");
		return SUCCESS;
	}
	return SUCCESS;
}

//-----------------------------------------------------------------------------
// private methods
//-----------------------------------------------------------------------------
static BOOL initialized = NO;

/**initialzes the database, if it hasn't been done already. OK to call twice.
  *If the database hasn't been created yet, this will create it.
  *NOTE: There should be a 'create schema' API or something.*/
static BOOL init() {
	int result;
   if(initialized) return SUCCESS;

	//open the database
	result = sqlite3_open(DATABASE_NAME, &db);
	if(result!=SQLITE_OK) {
		printf("Couldn't open database: %s\n", sqlite3_errmsg(db));
		return FAIL;
	}
	
	//check the database version
	int versionResult = checkVersion();
	if(versionResult==CORRECT_VERSION){ 
		initialized = YES;
		return SUCCESS;
	}
	if(versionResult==INCORRECT_VERSION) {
		fprintf(stderr,"This database version is incorrect!!\n");
		sqlite3_close(db);
		return FAIL;
	}

	//otherwise it's probably a blank database, and we should create the schema
	fprintf(stderr, "New database detected, trying to create schema\n");
	if(createSchema()==FAIL) {
		fprintf(stderr, "Couldn't create schema for the database!!\n");
		sqlite3_close(db);
		return FAIL;
	}

	fprintf(stderr,"Database schema was created successfully. Ready to go!\n");
	initialized = YES;
	return SUCCESS;
}

/**Tries to create the schema for the newly opened datase. Returns SUCCESS,
  *or prints an error message and returns FAIL*/
static BOOL createSchema() {
	char query[1000];
	WordForReview *word;

	snprintf(query, sizeof(query), 
	                         "CREATE TABLE WORDS(\n"\
	                         "id INTEGER PRIMARY KEY AUTOINCREMENT,\n"\
									 "language VARCHAR[%lu],\n"\
	                         "localWord VARCHAR[%lu],\n"\
									 "foreignWord VARCHAR[%lu],\n"\
									 "competencyLevel INTEGER,\n"\
									 "type             INTEGER,\n"\
									 "lastReviewedTime  INTEGER);\n",
									 sizeof(word->language)-1, sizeof(word->localWord)-1,
									 sizeof(word->foreignWord)-1);
						          //We use sizeof() here so that if the size of the
								    //struct in the header file changes, this changes
								    //automatically to match. Of course, only on new
								    //databases, the old ones won't update automatically
	if(!runResultlessQuery(query, sizeof(query))) return FAIL;


	//Create a table with singleton variables, one for the last reviewed integer,
	//which will always increment, and one with the version of the db schema.
	//right now we are not using the version, but it never hurts to have one.
	snprintf(query, sizeof(query), "CREATE TABLE GLOBAL(\n"\
	                               "lastReviewed INTEGER,\n"\
											 "version      INTEGER);\n");
	if(!runResultlessQuery(query, sizeof(query))) return FAIL;

	//Insert the version and the starting 'lastReviewed', which will be
	//incremented every time a word is reviewed, and this will start at zero
	snprintf(query, sizeof(query), "INSERT INTO GLOBAL VALUES "\
	                               " (0,%d)",DATABASE_VERSION);
	return runResultlessQuery(query, sizeof(query));

}

/** Runs a query that doesn't return anything, or ignores the results.
  * query should be the query, and auerySize should be the max size of query.
  * prints an error message on error, and returns FAIL*/
static BOOL runResultlessQuery(const char *query, int querySize) {
	sqlite3_stmt *ppStmt;
	int result;
	if(sqlite3_prepare_v2(db, query, querySize, &ppStmt, NULL)!=SQLITE_OK)
		return handleError("Error while runningQuery", ppStmt);
	result = sqlite3_step(ppStmt);
	if(result!=SQLITE_DONE && result!=SQLITE_ROW)
		return handleError("Error while stepping query", ppStmt);
	if(sqlite3_finalize(ppStmt) != SQLITE_OK)
		return handleError("Error while finalizing query", NULL);

	return SUCCESS;
}

/**Prints an error message to stderr combining 'message' and the latest error
 * message from sqlite3. If pStmt!=NULL, will also finalize it.
 * Always returns FAIL.*/
static BOOL handleError(const char*message, sqlite3_stmt *pStmt) {
	fprintf(stderr, "%s: %s\n", message, sqlite3_errmsg(db));
	if(pStmt!=NULL)
		sqlite3_finalize(pStmt);
	
	return FAIL;
}

/**Checks the database version to make sure we have the correct version of the
  *database. Returns CORRECT_VERSION, INCORRECT_VERSION, or VERSION_ERROR*/
static int checkVersion() {
	int version;
	char query[1000];

	//query the database
	snprintf(query, sizeof(query), "SELECT VERSION FROM GLOBAL");
	if(getSingleIntegerFromQuery(query, sizeof(query), &version)!=SUCCESS)
		return VERSION_ERROR;
	
	//check the version
	if(version==DATABASE_VERSION)
		return CORRECT_VERSION;
	fprintf(stderr,"Incompatible database version: expected %d, got %d\n",
	        DATABASE_VERSION, version);
	return INCORRECT_VERSION;
}

/** Runs the query, and returns the integer in the first column of the
  * resultant query. Puts the result in 'result', which shouldn't be NULL.
  * returns SUCESS or FAIL on error, and DOESN'T print an error*/
static BOOL getSingleIntegerFromQuery(const char*query, int size, int*result) {
	sqlite3_stmt *ppStmt;

	if(sqlite3_prepare_v2(db, query, size, &ppStmt, NULL)!=SQLITE_OK)
		return handleError("Error while running int query", ppStmt);
	if(sqlite3_step(ppStmt) !=SQLITE_ROW)
		return handleError("Error while stepping int query", ppStmt);
	
	*result = sqlite3_column_int(ppStmt, 0);

	if(sqlite3_finalize(ppStmt) != SQLITE_OK)
		return handleError("Error while finalizing int query", NULL);
	
	return SUCCESS;
}

/**Returns a constant representing the last review time*/
static BOOL getLastReviewTime(int *lastReviewTime) {
	char query[] = "SELECT lastReviewed FROM GLOBAL";
	return getSingleIntegerFromQuery(query, strlen(query), lastReviewTime);
}

/**Increments lastReviewTime in the database*/
static BOOL incrementLastReviewTime() {
	char query[] = "UPDATE GLOBAL SET lastReviewed = lastReviewed+1";
	return runResultlessQuery(query, sizeof(query));
}


/** Runs a query, and then fills 'word' with the first result.  
 *  Expects columns to be selected in the order they are in the database.
 *  That is, (id, language, localWord, foreignWord, competencyLevel, type).
 *  On error, returns FAIL*/
static BOOL fillWordFromQuery(char *query, WordForReview *word) {
	sqlite3_stmt *ppStmt;
	int rv;

	printf("running query %s\n", query);
	if(sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, NULL)!=SQLITE_OK)
		return handleError("Error while filling word 1", ppStmt);
	if((rv=sqlite3_step(ppStmt)) != SQLITE_ROW) {
		printf("got rv %d\n", rv);
		return handleError("Couldn't find row while stepping db", ppStmt);
	}

	word->id=sqlite3_column_int(ppStmt, 0);
	strcpy(word->language, (char*)sqlite3_column_text(ppStmt, 1));
	strcpy(word->localWord, (char*)sqlite3_column_text(ppStmt, 2));
	strcpy(word->foreignWord, (char*)sqlite3_column_text(ppStmt, 3));
	word->competencyLevel = sqlite3_column_int(ppStmt, 4);
	word->type = sqlite3_column_int(ppStmt, 5);

	if(sqlite3_finalize(ppStmt) != SQLITE_OK)
		return handleError("Error while finalizing fillWord query", NULL);

	return SUCCESS;
}

/**Postpends a query to omit duplicates. For example, if 'word'
 * contains a word with id 5, a word with id 7, and a word with id 8,
 * this function will append ' WHERE id!=5 AND id!=7 AND id!=8 ' to
 * the end of query. If there is not enough space, will return FAIL,
 * though query may still be altered at that point. Index is the number
 * of elements in word to be postpended */
static BOOL postpendOmitDuplicates(char *query, int querySize, 
                                   WordForReview*word, int index) {
	int i;
	char s[1000];
	BOOL shouldAddWhere = YES;

	//don't add WHERE if it's already there
	if(strcasestr(query, "where")) {
		shouldAddWhere = NO;
	}

	for(i=0;i<index;i++) {
		if(i==0 && shouldAddWhere) {
			snprintf(s, sizeof(s), " WHERE id!=%d ", word[i].id);
		}
		else {
			snprintf(s, sizeof(s), " AND id!=%d ", word[i].id);
		}
		if(!strncatSafe(query, s, querySize)) return FAIL;
	}

	return SUCCESS;
}
