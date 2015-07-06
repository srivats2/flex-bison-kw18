//Author: Rengarajan Pelapur
//TinyDir usage extended to use wildcard!--just like TinyDir - works with POSIX and Windows (warps around dirent)
//Also handles all invalid inputs gracefully
//Copyright 2014 Rengarajan Pelapur University of Missouri
//inputs : path and wildcard as char* and also an int echo...this echos the filenames that were filtered (wildcard or not)
//output : filenames struct with char **filenames (names), number of filenames after filtering(int used), size of the array of char* (int size) 
//Added Natural Sort - 3-20-2014 - RP --- Still echos in machine sort order but i swear the result will be natural sorted

#ifndef	_FILELISTING_H
#define	_FILELISTING_H	1



#include <stdio.h>
#include "tinydir.h"
#include <string>
#include<ctype.h>


typedef struct{
	char **names;
	int used;
	int size;
}filenames;

filenames file_listing(char*, char*, int);
void insertNames(filenames*, char*);
void initNames(filenames*, int);
void freefilenames(filenames*);
static int qsort_natural(const void *, const void *);
int strcompare(const char*, const char*);
#endif
