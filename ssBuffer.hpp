//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2020-12-15

#ifndef _SS_BUFFER_H_
#define _SS_BUFFER_H_

#include <cstring>
#include <iostream>
#include <string>
#include <typeinfo>

template <class T>
class ssBuffer
{
public:
    ssBuffer(size_t _bufferLength = 1 * 1024 * 1024);

    ~ssBuffer();

    // Read data from buffer
    //_data : the container of data;
    //_length : the length of data to read;
    //PS : if valid buffer area's length less than _length , _data can't get data and _length is 0 , return OVER_READ
    //如果需要读取的长度，超出缓冲区有效数据长度，那会返回OVER_READ,_data=NULL，_length=0
    int ReadData(T *_data, size_t _length);

    // Write data to buffer
    //_data : the data to be written
    //_length : the length of data to write
    //PS : if data to be written will overwrite the unused buffer area , the data will be written correctly , but return OVER_WRITE
    //如果需要写入的数据长度，会覆盖未读取的区域，那么数据会被正常写入（未读区被覆盖），但是会返回OVER_WRITE作为警告
    int WriteData(T *_data, size_t _length);

    // Reset buffer;
    int Reset();

    std::string GetLastErrorString();

    enum ssBufferCode
    {
        YES,
        ILLEGAL_PARAM,
        NO_INIT,
        NO_START,
        OUT_RANGE,
        OVER_WRITE,
        OVER_READ,
    };

private:
    size_t m_capcity;      // buffer's capcity
    size_t m_pos_write;    // write pointer
    size_t m_pos_read;     // read pointer
    bool m_isStart;        // flag of start write
    bool m_isInit;         // flag of initialize
    T *m_buffer;           // main buffer
    std::string m_err_str; // string of error info
};

template <class T>
ssBuffer<T>::ssBuffer(size_t _bufferLength)
{
    if (_bufferLength <= 0)
        throw "illegal param";

    m_capcity = _bufferLength;
    m_buffer = new T[m_capcity];
    if (typeid(T) == typeid(char) || typeid(T) == typeid(short) ||
        typeid(T) == typeid(int) || typeid(T) == typeid(long) ||
        typeid(T) == typeid(unsigned int) ||
        typeid(T) == typeid(unsigned short) ||
        typeid(T) == typeid(unsigned long))
    {
        for (int i = 0; i < m_capcity; i++)
            m_buffer[i] = 0;
    }
    m_isInit = true;
    m_pos_write = 0;
    m_pos_read = 0;
}

template <class T>
ssBuffer<T>::~ssBuffer()
{
    delete[] m_buffer;
}

template <class T>
int ssBuffer<T>::ReadData(T *_data, size_t _length)
{
    if (!m_isInit)
    {
        m_err_str = "buffer no initialize";
        return NO_INIT;
    }
    if (!m_isStart)
    {
        m_err_str = "buffer no start , because data has not been written";
        return NO_START;
    }
    if (_data == NULL || _length <= 0)
    {
        m_err_str = "params is illegal , such as _data is NULL , _length less than or equal to 0";
        return ILLEGAL_PARAM;
    }
    if (_length > m_capcity)
    {
        m_err_str = "_length[param] is out of range";
        return OUT_RANGE;
    }

    size_t distance;
    if (m_pos_read < m_pos_write)
        distance = m_pos_write - m_pos_read;
    else if (m_pos_read > m_pos_write)
        distance = m_capcity - m_pos_read + m_pos_write;
    else
        distance = 0;
    if (_length > distance)
        return OVER_READ;

    int pos = (m_pos_read + _length) % m_capcity;
    if (m_capcity - m_pos_read >= _length)
    {
        memcpy(_data, m_buffer + m_pos_read, _length * sizeof(T));
    }
    else
    {
        memcpy(_data, m_buffer + m_pos_read, (m_capcity - m_pos_read) * sizeof(T));
        memcpy(_data + (m_capcity - m_pos_read), m_buffer, pos);
    }
    m_pos_read = pos;
    return YES;
}

template <class T>
int ssBuffer<T>::WriteData(T *_data, size_t _length)
{
    if (!m_isInit)
    {
        m_err_str = "buffer no initialize";
        return NO_INIT;
    }
    if (_length > m_capcity)
    {
        m_err_str = "_length[param] is out of range";
        return OUT_RANGE;
    }

    m_isStart = true;

    size_t pos = (m_pos_write + _length) % m_capcity;
    if (m_capcity - m_pos_write >= _length)
    {
        memcpy(m_buffer + m_pos_write, _data, _length * sizeof(T));
    }
    else
    {
        memcpy(m_buffer + m_pos_write, _data,
               (m_capcity - m_pos_write) * sizeof(T));
        memcpy(m_buffer, _data + m_capcity - m_pos_write, pos);
    }

    size_t distance = 0;
    if (m_pos_read > m_pos_write)
        distance = m_pos_read - m_pos_write;
    else
        distance = m_capcity - m_pos_write + m_pos_read;
    m_pos_write = pos;
    if (_length > distance)
        return OVER_WRITE;
    return YES;
}

template <class T>
int ssBuffer<T>::Reset()
{
    if (!m_isInit)
    {
        m_err_str = "buffer no initialize";
        return NO_INIT;
    }
    memset(&m_buffer, 0, m_capcity * sizeof(T));
    m_isStart = false;
    m_pos_read = 0;
    m_pos_write = 0;
    return YES;
}

template<class T>
inline std::string ssBuffer<T>::GetLastErrorString()
{
	return m_err_str;
}

#endif