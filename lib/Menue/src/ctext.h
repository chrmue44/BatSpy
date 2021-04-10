#ifndef CTEXT_H
#define CTEXT_H

#include <cstdint>
#include <cstdlib>

#ifndef size_t
typedef  std::size_t size_t;
#endif

#define CH_AE   "\x80"
#define CH_OE   "\x81"
#define CH_UE   "\x82"
#define CH_AEs  "\x83"
#define CH_OEs  "\x84"
#define CH_UEs  "\x85"
#define CH_SS   "\x86"

enum enLang {
    LANG_GER = 1,
    LANG_EN  = 2
};

typedef uint32_t thText;
typedef const char tText;

struct stTxtList {
  uint32_t id;
  const char* pGeText;
  const char* pEnText;
};

class Txt {
 public:
  static void setResource(stTxtList* p);
  static void setLang(enLang lang);
  static const char* get(thText id);

 private:
  static enLang m_actLang;
  static stTxtList* m_pTextList;
  static size_t m_size;
};
#endif // CTEXT_H
