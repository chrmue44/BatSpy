/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#include "cutils.h"
#include <cstring>
#include "Arduino.h"
#include "ctext.h"

int cUtils::stripExtension(const char* fileName, char* result, size_t resultSize) {
  const char* p = strrchr(fileName, '.');
  if (p != nullptr) {
    size_t end = p - fileName - 1;
    return subStr(fileName, 0, end, result, resultSize);
  }
  else
    return 1;
}


int cUtils::getExtension(const char* fileName,char* result, size_t resultSize) {
  const char* p = strrchr(fileName, '.');
  if (p != nullptr) {
    size_t start = p - fileName + 1;
    return subStr(fileName, start, 256, result, resultSize);
  }
  else
    return 1;
}

const char* cUtils::getFileName(const char* fullPath)
{
  const char* p = strrchr(fullPath, '/');
  p++;
  return p;
}

int cUtils::subStr(const char* str, size_t start, size_t end, char* result, size_t resultSize)
{
  size_t j = 0;
  int retVal = 0;
  for (size_t i = start; i <= end; i++) {
    if (j < resultSize) {
      result[j] = str[i];
      j++;
    }
    else
      retVal = 1;
    if(str[j] == 0)
      break;
  }
  if (j < resultSize)
    result[j] = 0;
  else
    retVal = 1;
  return retVal;
}


int cUtils::replace(const char* str, const char* toSearch, const char* replaceStr, char* result, size_t resultSize)
{
  int retVal = 0;
  const char* p = strstr(str, toSearch);
  if (p != nullptr) {
    if(p != str)
    {
      size_t end = p - str - 1;
      retVal = subStr(str, 0, end, result, resultSize);
    }
    else
      result[0]= 0;
    strncat(result, replaceStr, resultSize);
    p += strlen(toSearch);
    strncat(result, p, resultSize);
  }
  else {
    retVal = -1;
  }
  return retVal;
}

void cUtils::replaceCharsInPlace(char* str, size_t strSize, char toSearch, char replaceCh)
{
  size_t len = strlen(str);
  if (len > strSize)
    len = strSize;

  for (size_t i = 0; i < len; i++) {
    if (str[i] == toSearch)
      str[i] = replaceCh;
  }
}

int cUtils::replaceAll(const char* str, const char* toSearch, const char* replaceStr, char* result, size_t resultSize)
{
  int retVal = 0;
  bool found = false;
  char buf[512];
  strncpy(buf, str, sizeof(buf));

  do {
    retVal = cUtils::replace(buf, toSearch, replaceStr, result, resultSize);
    if (retVal == 0) {
      strncpy(buf, result, resultSize);
      found = true;
    }
  } while (retVal == 0);
  if (found)
    return 0;
  else
    return -1;
}

int cUtils::replaceUTF8withInternalCoding(const char* str, char* result, size_t resultSize) {
  if(str == nullptr)
     return 0;

  bool found = false;
  int retVal = replaceAll(str, "\xc3\x9f", CH_SS, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\x84", CH_AE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\x96", CH_OE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\x9c", CH_UE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\xa4", CH_AEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\xb6", CH_OEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc3\xbc", CH_UEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, "\xc2\xb0", CH_DEG, result, resultSize);
  if(retVal == 0)
    found = true;
  if(!found)
    strncpy(result, str, resultSize);
  return retVal;
}

//https://www.geeksforgeeks.org/wildcard-character-matching/
bool cUtils::isWhiteSpace(char c) 
{
  return ((c == ' ') || (c == '\t') || (c == '\n'));
}

// The main function that checks if two given strings
// match. The first string may contain wildcard characters
bool cUtils::match(const char *first, const char * second)
{
    // If we reach at the end of both strings, we are done
    if (*first == '\0' && *second == '\0')
        return true;

    // Make sure that the characters after '*' are present
    // in second string. This function assumes that the first
    // string will not contain two consecutive '*'
    if (*first == '*' && *(first+1) != '\0' && *second == '\0')
        return false;

    // If the first string contains '?', or current characters
    // of both strings match
    if (*first == '?' || *first == *second)
        return match(first+1, second+1);

    // If there is *, then there are two possibilities
    // a) We consider current character of second string
    // b) We ignore current character of second string.
    if (*first == '*')
        return match(first+1, second) || match(first, second+1);
    return false;
}
