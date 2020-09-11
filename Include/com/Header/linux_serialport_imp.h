#ifndef _BOOST_SERIAL_IMP_H_
#define _BOOST_SERIAL_IMP_H_

#include <iostream>
#include "linux_serial.h"

using namespace std;

enum {
    default_time_out = 200
};
#define SUCCEED 0
#define FAIL -1

namespace cms {
    enum enuParity {
        none = 0, odd, even
    };
    enum enuStopbits {
        one = 1, two = 2
    };

    class CSerialPortImp {
    public:
        CSerialPortImp();

        ~CSerialPortImp();

        int open(int nPort);//�򿪴��ڣ��ɹ�����SUCCEED��ʧ�ܷ���FAIL��
        int set_para(int baud = 9600, enuParity parity = none, int databits = 8, enuStopbits stopbit = one,
                     int timeout = default_time_out);

        void set_timeout(int timeout);//�޸ĳ�ʱʱ��
        int write(unsigned char *buf, int len);//������д��ͬ����,�ɹ�����д����ֽ�����ʧ�ܷ���FAIL��
        int clear_read_buff();

        int read(unsigned char *buf, int len); //
        int close();//�رմ���

    private:
        int m_nPort;//���ں�
        int m_nTimeOut;//��ʱʱ��
        bool m_b_open;

        unsigned long long hn_difftime(unsigned long long starttime);

        unsigned long long hn_setstarttime();
    };
}

#endif
