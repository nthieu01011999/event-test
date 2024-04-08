#include "AudioFile.h"
#include <cstring>
#include <cstdio>
#include <fcntl.h>

AudioFile::AudioFile(int buf_size): m_buf_size(buf_size)
{
    m_buf = new char[m_buf_size];
}

AudioFile::~AudioFile()
{
    delete [] m_buf;
}

bool AudioFile::Open(const char *path)
{
    m_file = fopen(path, "rb");
    if(m_file == NULL) {
        return false;
    }

    if (fcntl(fileno(m_file), F_SETFL, O_NONBLOCK) != 0) {
        printf("Cannot set non-block to audio fifo\n");
        fclose(m_file);
        return false;
    };

    return true;
}

void AudioFile::Close()
{
    if(m_file) {
        fclose(m_file);
        m_file = NULL;
        m_count = 0;
        m_bytes_used = 0;
    }
}

int AudioFile::ReadFrame(char* in_buf, int in_buf_size)
{
    if(m_file == NULL) {
        return -1;
    }

    // in_buf size must be greater than m_buf_size
    int bytes_read = (int) fread(m_buf, 1, m_buf_size, m_file);
    if (bytes_read > 0) {
        memcpy(in_buf, m_buf, bytes_read);
    }
//    printf("refill %d\n", bytes_read);
    return bytes_read;
}
