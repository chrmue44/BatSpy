#include <cstdint>
#include <cstdlib>
#include "ctext.h"
#include <Arduino.h>

#ifndef size_t
typedef std::size_t size_t;
#endif
#ifndef NULL
#define NULL 0
#endif

enLang Txt::m_actLang = LANG_GER;
stTxtList* Txt::m_pTextList = NULL;
size_t Txt::m_size;

void Txt::setLang(enLang lang) {
  m_actLang = lang;
}

void Txt::setResource(stTxtList* p) {
  m_pTextList = p;
}

const char* Txt::get(thText id) {
    const char* p = NULL;
    bool found = false;
    thText i = 0;
    if(m_pTextList == NULL)
      return "set text resource!";
    for (i= 0; ; i++) {
      if (m_pTextList[i].id == id) {
        switch (m_actLang) {
          case LANG_GER:
          default:
            p = m_pTextList[i].pGeText;
            break;
          case LANG_EN:
            p = m_pTextList[i].pEnText;
            break;
        }
        found = true;
        break;
      }
      if(m_pTextList[i].id == 0xFFFF)
        break;
    }
    if(p != NULL)
      return p;
    else if (found)
      return m_pTextList[i].pGeText;
    else
      return "Text not found!";
}
