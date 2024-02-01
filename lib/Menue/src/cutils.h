/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * utility functions
 * ***********************************************************/
#ifndef C_UTILS_H
#define C_UTILS_H

#include <cstring>
#include "my_vector.h"

#ifdef SIMU_DISPLAY
#define A_UML_CAP    "Ä"
#define A_UML_SMALL  "ä"
#define O_UML_CAP    "Ö"
#define O_UML_SMALL  "ö"
#define U_UML_CAP    "Ü"
#define U_UML_SMALL  "ü"
#define S_UML        "ß"
#define DEG_SIGN     "°"
#else
#define A_UML_CAP    "\xc3\x84"
#define A_UML_SMALL  "\xc3\xa4"
#define O_UML_CAP    "\xc3\x96"
#define O_UML_SMALL  "\xc3\xb6"
#define U_UML_CAP    "\xc3\x9c"
#define U_UML_SMALL  "\xc3\xbc"
#define S_UML        "\xc3\x9f"
#define DEG_SIGN     "\xc2\xb0"
#endif
class cUtils
{
 public:
  static int stripExtension(const char* fileName, char* result, size_t resultSize);
  static int getExtension(const char* fileName,char* result, size_t resultSize);
  static const char* getFileName(const char* fullPath);
  static int subStr(const char* str, size_t start, size_t end, char* result, size_t resultSize);
  static int replace(const char* str, const char* toSearch, const char* replaceStr, char* result, size_t resultSize); 
  static void replaceCharsInPlace(char* str, size_t strSize, char toSearch, char replaceCh);
  static int replaceAll(const char* str, const char* toSearch, const char* replaceStr, char* result, size_t resultSize);
  static int replaceUTF8withInternalCoding(const char* str, char* result, size_t resultSize);
  static int replaceInternalCodingWithUTF8(const char* str, char* result, size_t resultSize);
  static bool isWhiteSpace(char c);
  static bool match(const char *pattern, const char * str);
  static size_t splitStr(char* str, char s, my_vector<char*, 20>& out);
};

#endif  //#ifndef C_UTILS_H
