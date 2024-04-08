#include <iostream>
#include <string>

class AudioFile
{
public:
    AudioFile(int buf_size=8192);
    ~AudioFile();

    bool Open(const char *path);
    void Close();

    bool IsOpened() const
    { return (m_file != NULL); }

    int ReadFrame(char* in_buf, int in_buf_size);

private:
    FILE *m_file = NULL;
    char *m_buf = NULL;
    int  m_buf_size = 0;
    int  m_bytes_used = 0;
    int  m_count = 0;
};
