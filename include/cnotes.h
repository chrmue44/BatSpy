#ifndef CNOTES_H
#define CNOTES_H

#include "my_vector.h"
#include "ctext.h"
#include "MenueSystem.h"

class cnotes
{
public:
    cnotes();
    void initNotes(const char* fName, thText tHandleStart, thText tHandleEnd);
    int getCount() {return m_count;};
    void getNote(int i, char* buf, size_t bufSize);
    void initListPar(cParList& list);

private:
    void createFile(const char* fName, thText tHandleStart, thText tHandleEnd);

private:
    const char* m_fName = nullptr;
    int m_count = 0;
};

#endif // CNOTES_H
