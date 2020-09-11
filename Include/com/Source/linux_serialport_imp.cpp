#include "../Header/linux_serialport_imp.h"
#include<time.h>

namespace cms {
    CSerialPortImp::CSerialPortImp() {
        m_b_open = false;
    }

    CSerialPortImp::~CSerialPortImp() {
    }

    int CSerialPortImp::open(int nPort) {
        m_nPort = nPort;
        if (SERIAL_ERROR_OPEN == SerialOpen(nPort))
            return FAIL;
        else {
            printf("CSerialPortImp::open %d success\n", nPort);
            m_b_open = true;
            return SUCCEED;
        }
    }

    int CSerialPortImp::set_para(int baud, enuParity parity, int databits, enuStopbits stopbit, int timeout) {
        //printf("CSerialPortImp::set_para\n");
        SerialSetSpeed(m_nPort, baud);
        SerialSetParam(m_nPort, parity, databits, stopbit);
        set_timeout(timeout);
        return SUCCEED;
    }

    int CSerialPortImp::write(unsigned char *buf, int len) {
        // printf("CSerialPortImp::write port=%d,len=%d\n",m_nPort,len);
        SerialWrite(m_nPort, buf, len);
        return SUCCEED;
    }

    int CSerialPortImp::read(unsigned char *buf, int len) {
        // printf("enter CSerialPortImp::read port=%d,len=%d\n",m_nPort,len);
        int total_read_size = 0;
        int once_read_size = 0;
        unsigned char once_read_buff[1024] = {0};
        unsigned long long time1 = hn_setstarttime();

        bool b_start_read = false;
        while (true) {
            if (SerialDataInInputQueue(m_nPort) > 0) {
                if (total_read_size >= 1024)
                    break;

                b_start_read = true;
                once_read_size = SerialNonBlockRead(m_nPort, once_read_buff, 1024);
                memcpy(buf + total_read_size, once_read_buff, once_read_size);

                //printf("port=%d,total_read_size=%d,once_read_size=%d\n",m_nPort,total_read_size,once_read_size);
                total_read_size += once_read_size;

                usleep(10 * 1000);//10ms
            } else {
                if (b_start_read)
                    break;
                else {
                    usleep(100 * 1000);
                    if (hn_difftime(time1) > m_nTimeOut * 1000)
                        break;
                }
            }
        }

        // printf("leave CSerialPortImp::read port=%d,total_read_size=%d\n",m_nPort,total_read_size);
        return total_read_size;
    }

    int CSerialPortImp::clear_read_buff() {
        // printf("clear_read_buff\n");
        if (SERIAL_OK == SerialCleanBuff(m_nPort))
            return SUCCEED;
        else
            return FAIL;
    }

    int CSerialPortImp::close() {
        if (!m_b_open)
            return SUCCEED;

        // printf("CSerialPortImp::close\n");
        if (SERIAL_OK == SerialClose(m_nPort)) {
            m_b_open = false;
            return SUCCEED;
        } else
            return FAIL;
    }

    void CSerialPortImp::set_timeout(int timeout) {
        //printf("CSerialPortImp::set_timeout\n");
        m_nTimeOut = timeout;
    }

    unsigned long long CSerialPortImp::hn_setstarttime() {
        struct timespec time = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &time);
        return (unsigned long long) (time.tv_sec * 1000000 +
                                     time.tv_nsec / 1000);
    }

    unsigned long long CSerialPortImp::hn_difftime(unsigned long long starttime) {
        struct timespec time = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &time);
        //
        unsigned long long now = (unsigned long long) (time.tv_sec * 1000000 + time.tv_nsec / 1000);
        return (now - starttime);
    }
}



