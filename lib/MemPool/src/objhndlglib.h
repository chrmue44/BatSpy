#ifndef OBJHNDLGLIB_H
#define OBJHNDLGLIB_H
/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/


// mit diesem DEFINE wird die Allokierung aller Klassen mit create() und
// destroy() auf statische Allokierung umgestellt
//#ifndef UNIT_TEST
#define MEMPOOL
//#endif

#include "clFixMemPool.h"



#ifdef MEMPOOL
// Makros fuer aktiven FixMemPool
//#define NEW(Cl,Str) new(clFixMemPool::allocate(sizeof(Cl),Str,ALLOC_OBJ)) Cl
//#define DELETE(Cl,p,Str)   {(p)->~Cl(); clFixMemPool::deallocate(p,Str,ALLOC_OBJ); p = nullptr;}
/*
#define CREATE_OBJECT_DEFCON(Cl,Str) new(clFixMemPool::allocate(sizeof(Cl),Str,ALLOC_OBJ)) Cl
#define CREATE_OBJECT_CON(Cl,Co,Str) new(clFixMemPool::allocate(sizeof(Cl),Str,ALLOC_OBJ)) Cl Co
#define CREATE_OBJECT_ARR(Cl,Si,Str) reinterpret_cast<Cl*>(clFixMemPool::allocate(sizeof(Cl)*(Si),Str,ALLOC_OBJ))

#define CREATE_OBJECT_ARRCON(Var,Cl,Si,Str) \
  reinterpret_cast<Cl*>(clFixMemPool::allocate(sizeof(Cl)*(Si),Str,ALLOC_OBJ)); \
  for(uint32_t i = 0; i < Si; i++) new((char*) Var + i*sizeof(Cl))Cl;

#define CREATE_STRUCT(Cl,Str) new(clFixMemPool::allocate(sizeof(Cl),Str,ALLOC_OBJ)) Cl

#define DELETE_TYPED_OBJECT(Cl,p,Str)   {(p)->~Cl(); clFixMemPool::deallocate(p,Str,ALLOC_OBJ); p = NULL;}
#define DELETE_OBJECT_ARR(p,Str) {clFixMemPool::deallocate(p,Str,ALLOC_OBJ);(p) = NULL;}
#define DELETE_OBJECT_ARRCON(p,Str) {clFixMemPool::deallocate(p,Str,ALLOC_OBJ);(p) = NULL;}
#define DELETE_STRUCT(p,Str)  {clFixMemPool::deallocate(p,Str,ALLOC_OBJ);(p) = NULL;}
#else
// Makros fuer Standard-Allokierung
#define NEW(Cl,Str) new Cl
#define DELETE(Cl,p,Str)   {if((p)!=nullptr){delete(p); (p) = nullptr;}}
#define CREATE_OBJECT_DEFCON(Cl,Str) new Cl
#define CREATE_OBJECT_CON(Cl,Co,Str) new Cl Co
#define CREATE_OBJECT_ARR(Ob,Siz,Str) new Ob[Siz]
#define CREATE_OBJECT_ARRCON(Var,Cl,Si,Str) new Cl[Si]
#define CREATE_STRUCT(Cl,Str) new Cl

#define DELETE_TYPED_OBJECT(Cl,p,Str)   {if((p)!=NULL){delete(p); (p) = NULL;}}
#define DELETE_OBJECT_ARR(a,Str) {delete [] (a);(a) = NULL;}
#define DELETE_STRUCT(p,Str)  {if((p)!=NULL){delete(p); (p) = NULL;}}
#define DELETE_OBJECT_ARRCON(p,Str) {delete [] (p);(p) = NULL;}
*/
#endif

#include <string>
#ifdef MEMPOOL
void* operator new(std::size_t sz);
void operator delete(void* ptr) noexcept;

#include "myalloc.h"
// String-Klasse, die im Gegensatz zur Standard-Implemetierung den Speicher
// aus dem statischen Memory-Pool allokiert
//typedef std::basic_string<char,std::char_traits<char>,MyLib::MyAlloc<char> >mpString;

class mpString : public std::basic_string<char,std::char_traits<char>,MyLib::MyAlloc<char> >
{

/*public:
  void* operator new(size_t sz)
  {
    return clFixMemPool::allocate(sz, "mpString");
  }

  void operator delete(void* ptr) noexcept{
      clFixMemPool::deallocate(ptr, "mpString");
  }*/
};

// Vector-Klasse, die im Gegensatz zur Standard-Implemetierung den Speicher
// aus dem statischen Memory-Pool allokiert
#include <vector>
template<class T> 
class mpVector : public std::vector<T, MyLib::MyAlloc<T>>
{
    /*
  public:
    void* operator new(size_t sz)
    {
      return clFixMemPool::allocate(sz, "mpVector");
    }

    void operator delete(void* ptr) noexcept{
        clFixMemPool::deallocate(ptr, "mpVector");
    }*/
};

#else
typedef std::string tPM_string;
#define tPM_vector std::vector 
#endif

//#define CREATE_OBJECT(a) new a

// Loescht ein dynamisch erzeugtes Objekt und setzt den Objektzeiger auf NULL 
//#define DELETE_OBJECT(a) {delete(a);(a) = NULL;}
// Functor ruft fuer ein ihm uebergebenes Objekt destroy auf. 
// Benutzt fuer Container der STL, die Ojekzeiger von mit
// new erzeugten Objekten speichern
struct DestroyListObject
{
  template<typename T>
  void operator()(T* ptr) const
  {
    T::destroy(&ptr);
  }
};


// Functor loescht ein ihm uebergebenes Objekt. Benutzt fuer Container der STL, die Ojekzeiger von mit
// new erzeugten Objekten speichern
struct DeleteListObject
{
  template<typename T>
  void operator()(const T* ptr) const
  {
    delete ptr;
  }
};



#endif //OBJHNDLGLIB_H

