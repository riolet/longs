#include "longs.h"
#include <assert.h>

#define dbgprintf printf

bool validate_url(char *c) {
    char * http_string = "http";
    int c_len = strlen(c);
    int i;

    for (i=0;i<4;i++) {
        if (i>=c_len||c[i]!=http_string[i]) {
            dbgprintf ("Invalid Scheme http A %c, E %c, %d\n",c[i],http_string[i],i);
            return false;
        }
    }
    if (i<c_len&&c[i]=='s') {
        i++;
    }
    int cursor = i;
    char * colslashslash = "://";
    for (i=cursor;i<cursor+3;i++) {
        if (i>=c_len||c[i]!=colslashslash[i-cursor]) {
            dbgprintf ("Invalid Scheme :// A %c, E %c, %d\n",c[i],colslashslash[i-cursor],i);
            return false;
        }
    }

    cursor=i;

    for (i=cursor;i<c_len;i++) {
        if (!valid_html_chars[c[i]]) {
            dbgprintf ("Invalid Scheme %c, %d\n",c[i],i);
            return false;
        }
    }
    return true;
}

static char* alphabet_base_encode(char *hash1, int id) {
	int i;
	dbgprintf("%d\n", id);

	for (i=0;i<hashLength;i++) {
        hash1[i]=alphebet[id/powerof32[(MAX_HASH_LENGTH-hashLength)+i]];
        id %= powerof32[(MAX_HASH_LENGTH-hashLength)+i];
	}
	hash1[i]='\0';
	dbgprintf("%s\n", hash1);

	return hash1;
}

long alphabet_base_decode(const char *hash1) {
	int i;
	long id = 0;
	for (i=0;i<hashLength;i++) {
        id += powerof32[(MAX_HASH_LENGTH-hashLength)+i]*alphabet_table[hash1[i+1]];
	}
	dbgprintf("%d\n", id);
	return id;
}

void main () {
    assert(validate_url("http://riolet.com")==true);
    assert(validate_url("https://riolet.com")==true);
    assert(validate_url("httpriolet.com")==false);
    assert(validate_url("httpsriolet.com")==false);
    assert(validate_url("riolet.com")==false);
    hashLength=3;
    char hash[MAX_HASH_LENGTH];
    char hash2[MAX_HASH_LENGTH];
    alphabet_base_encode(hash,32);
    assert(!strcmp("aba",hash));
    sprintf(hash2,"/%s",hash);
    int id=alphabet_base_decode(hash2);
    assert(id==32);
}
