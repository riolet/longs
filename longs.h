#ifndef LONGS_H_
#define LONGS_H_
#include "wafer.h"
#include "waferapi.h"
#include "sqlite3.h"

typedef struct {
    Request * request;
    Response * response;
    char * longUrl;
} Params;


static void form(Request *request, Response *response);
static void favicon(Request *request, Response *response) ;
static void shorten(char *longUrl, Response *response);
static void getLongUrl(char *shortUrl, Params * params);
static char * alphabet_base_encode(char *hash1, int id);
static int fetch_id(void *data, int argc, char **argv, char **azColName);
static int fetch_longUrl (void *data, int argc, char **argv, char **azColName);
static char* alphabet_base_encode(char *hash1, int id);
long alphabet_base_decode(const char *hash1);
static void printShortUrl(char * hash, Params * params);

#define MAX_HASH_LENGTH 8
#define ALPHABET_BASE
char alphebet[ALPHABET_BASE] = "abcdefghjklmnpqrstuvwxyz23456789";
char alphabet_table [255] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,25,26,27,28,29,30,31,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,2,
    3,4,5,6,7,-1,8,9,10,11,12,-1,13,14,15,16,17,18,19,20,21,22,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

char valid_html_chars [255] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

long powerof32[MAX_HASH_LENGTH] = {34359738368, 1073741824, 33554432, 1048576, 32768, 1024, 32, 1};


sqlite3 *db;  // db initialisation
char *baseUrl = "lon.gs"; // base url of shorted urls
char *dbPath = "URL_Records.db"; //path/to/db
int hashLength = 5;
#define SIZE 2048
#endif
