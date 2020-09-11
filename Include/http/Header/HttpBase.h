//
// Created by Lin on 2020/8/20.
//
#ifndef __LIN_HTTPBASE_H__
#define __LIN_HTTPBASE_H__

#include <iostream>

using namespace std;
namespace lin {

    class HttpBase {

    public:

        HttpBase();

        virtual ~HttpBase();

        /*
        * ����Post����
        */
        virtual int post(const std::string &url) = 0;

        /*
        * ����get����
        */
        virtual int get(const std::string &url) = 0;

        virtual std::string getResponse(void) = 0;

    protected:
        typedef int(*pBuildRequest)(const string &, const string &,
                                    std::ostream &);

        /*
        * ����URL
        * parseUrl
        * url: ��������URL
        * out_server: ��������
        * out_port: �˿ں�
        * out_path: ��������ҳ
        */
        static int parseUrl(const std::string &url, std::string &out_server,
                            std::string &out_port, std::string &out_path);

        /*
        * ����Post����
        * buildPostRequest
        */
        static int buildPostRequest(const std::string &server, const std::string &path,
                                    std::ostream &out_request);

        /*
        * ����Get����
        * buildGetRequest
        */
        static int buildGetRequest(const std::string &server, const std::string &path,
                                   std::ostream &out_request);

    };

}

#endif // __LIN_HTTPBASE_H__
