#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>

/* STRING MANAGEMENT */

char *stringDuplication(const char *src);

char *stringNDuplication(const char *src, size_t size);

char *stringConcatenation(const char *srcOne, const char *srcTwo);

/* STRING SPLITTING */

char **splitStringByDelimiter(const char *src, const char *delim, int *numSubstr);

char **splitStringNByDelimiter(const char *src, const char *delim, const int numSubstr);

char **splitStringBySize(const char *src, const size_t size, int *numSubstr);

char **splitStringBySection(const char *src, const size_t *ssize, const int numsubstr);

/* ARRAY (DE)SERIALIZATION */

char *arraySerialization(char **array, const int numItems, const char *delim);

char **arrayDeserialization(const char *sarray, const char *delim, int *numItems);

/* VARIOUS */

char *getTime(void);

char *getUserInput(const char *descr);

#endif /* STRINGUTIL_H_ */
