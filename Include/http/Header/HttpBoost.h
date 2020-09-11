//
// Created by Lin on 2020/8/20.
//

#ifndef __LIN_HTTPBOOST_H__
#define __LIN_HTTPBOOST_H__

#include "HttpBase.h"

#include <boost/asio.hpp>

namespace lin {

    class HttpBoost :
            public HttpBase {

    public:

        HttpBoost(boost::asio::io_service &io_service);

        virtual ~HttpBoost();

        /*
        * ����Post����
        */
        virtual int post(const std::string &url);

        /*
        * ����get����
        */
        virtual int get(const std::string &url);

        virtual std::string getResponse(void) {
            return responseData_;
        }

    private:
        // ��������
        void handle_request_resolve(const std::string &url, pBuildRequest func);

        // ������
        void handle_resolve(const boost::system::error_code &err,
                            boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

        // ���Ӻ�
        void handle_connect(const boost::system::error_code &err);

        // ���������
        void handle_write_request(const boost::system::error_code &err);

        // ��ȡ��Ӧ��
        void handle_read_status_line(const boost::system::error_code &err);

        // ��ȡ��Ӧͷ��
        void handle_read_headers(const boost::system::error_code &err);

        // ��ȡ�������ݺ�
        void handle_read_content(const boost::system::error_code &err);

    private:

        // ������
        boost::asio::ip::tcp::resolver resolver_;
        // �׽���
        boost::asio::ip::tcp::socket socket_;
        // ���󻺳���
        boost::asio::streambuf request_;
        // ��Ӧ������
        boost::asio::streambuf response_;
        // ��Ӧ����
        std::string responseData_;

    };

    std::string post(std::string url);


    std::string get(std::string url);


}
#endif // __LIN_HTTPBOOST_H__