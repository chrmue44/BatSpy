/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}.com             *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * utility functions
 * ***********************************************************/
#ifndef C_UTILS_H
#define C_UTILS_H

#include <cstring>

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
  static bool isWhiteSpace(char c);
};

#endif  //#ifndef C_UTILS_H
