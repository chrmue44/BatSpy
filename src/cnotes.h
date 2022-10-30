#ifndef CNOTES_H
#define CNOTES_H

#include "my_vector.h"
#include "ctext.h"

class cnotes
{
public:
    cnotes();
    void initNotes(char* fName, thText tHandleStart, thText tHandleEnd);
    int getCount() {return m_count;};
    void getNote(int i, char* buf, size_t bufSize);
private:
    void createFile(thText tHandleStart, thText tHandleEnd);

private:
    const char* m_fName;
    int m_count;
};

#endif // CNOTES_H
