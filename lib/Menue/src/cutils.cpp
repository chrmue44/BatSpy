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
  strncpy(buf, str, sizeof(buf) - 1);

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
  int retVal = replaceAll(str, S_UML, CH_SS, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, A_UML_CAP, CH_AE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, O_UML_CAP, CH_OE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, U_UML_CAP, CH_UE, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, A_UML_SMALL, CH_AEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, O_UML_SMALL, CH_OEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, U_UML_SMALL, CH_UEs, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, DEG_SIGN, CH_DEG, result, resultSize);
  if(retVal == 0)
    found = true;
  if(!found)
    strncpy(result, str, resultSize);
  return retVal;
}

int cUtils::replaceInternalCodingWithUTF8(const char* str, char* result, size_t resultSize) {
  if(str == nullptr)
     return 0;

  bool found = false;
  int retVal = replaceAll(str, CH_SS, S_UML,  result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_AE, A_UML_CAP,  result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_OE, O_UML_CAP, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_UE, U_UML_CAP,  result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_AEs, A_UML_SMALL, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_OEs, O_UML_SMALL,  result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_UEs, U_UML_SMALL, result, resultSize);
  if(retVal == 0)
    found = true; 
  retVal = replaceAll(str, CH_DEG, DEG_SIGN, result, resultSize);
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

size_t cUtils::splitStr(char* str, char s, my_vector<char*, 20>& out)
{
  size_t retVal = 1;
  out.clear();
  out.push_back(str);
  char* p = str;

  while(retVal < (out.maxSize() - 1)) 
  {
    p = strchr(p, s);
    if(p == nullptr)
      break;
    *p = 0;
    p++;
    retVal++;
    out.push_back(p);
  }
  return retVal;
}
