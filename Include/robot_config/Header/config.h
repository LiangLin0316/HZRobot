//
// Created by Lin on 2020/9/8.
//
#pragma once
#ifndef ROBOTAI_CONFIG_HPP
#define ROBOTAI_CONFIG_HPP
#include "../../xml/Header/tinyxml.h"
#include "../../util/json.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>

#define CAMTYPE "camtype"

typedef float camtype;

enum watch_type {
    point_watch = 1,
    switch_watch
};

typedef struct {
    std::string ip;
    std::string port;
    std::string user;
    std::string passwd;
    std::string dbname;
} mysql_config;



typedef struct {
    std::string server_ip;
    unsigned short int server_port;
    std::string doc_root;
    int threads;
    std::string robot_ip;
    std::string robot_port;
    std::string ea_ip;
    std::string ea_port;
    int timeout;
    int cn_photo;
}server_config;

struct st_robot_ip
{
    std::string ip;
    SHINE_JSON_MODEL(st_robot_ip,ip);
};

 struct ea_photo_L1
{
    std::vector<std::vector<std::map<std::string,int>>> param;
    SHINE_JSON_MODEL(ea_photo_L1,param);
};

 struct ea_photo_L2{
    int type;
    int tablenum;
    std::map<std::string , ea_photo_L1 >param;
    SHINE_JSON_MODEL(ea_photo_L2, type,tablenum,param);
} ;

class Config {
public:
    mysql_config sql_config;
    server_config ser_config;

private:
    static Config *m_instance;

public:
    ea_photo_L2 photo_config;
    st_robot_ip c_robot_ip;

private:
    Config() {

    }

public:
    static Config *config_instance() {
        if (m_instance == NULL)
            m_instance = new Config();
        return m_instance;
    }

    bool loadmysql_config() {
        TiXmlDocument doc;
        std::string path_xml = "mysql_config.xml";
        if (!doc.LoadFile(path_xml.c_str()))
            return false;
        try {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pElem;
            pElem = hDoc.FirstChildElement().Element();
            TiXmlHandle hRoot(pElem);
            TiXmlElement *Cnode = hRoot.FirstChild("databases").FirstChild("user").Element();
            sql_config.user = (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("databases").FirstChild("password").Element();
            sql_config.passwd = (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("databases").FirstChild("server_ip").Element();
            sql_config.ip = (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("databases").FirstChild("port").Element();
            sql_config.port =  (const char *)Cnode->GetText();
            Cnode = hRoot.FirstChild("databases").FirstChild("dbname").Element();
            sql_config.dbname = (const char *) Cnode->GetText();
        }
        catch (...) {
            return false;
        }
        std::cout<<"load loadmysql_config"<<std::endl;
        return true;
    }

    bool loadphoto_config() {
        TiXmlDocument doc;
        std::string path_xml = "photo_config.xml";
        if (!doc.LoadFile(path_xml.c_str())) {
            return false;
        }
        try {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pElem;
            pElem = hDoc.FirstChildElement().Element();
            TiXmlHandle hRoot(pElem);
            TiXmlElement *CNode = hRoot.FirstChild("table_xx").FirstChild().Element();
            photo_config.tablenum = 0;

            ea_photo_L1 vvm_tmp;
            std::vector<std::map<std::string, int>> vm_tmp;
            std::map<std::string, int> m_tmp;

            for (CNode; CNode; CNode = CNode->NextSiblingElement()) {
                vm_tmp.clear();
                TiXmlHandle curHandle(CNode);
                TiXmlElement *Child_CNode = curHandle.FirstChildElement().Element();
                for (Child_CNode; Child_CNode; Child_CNode = Child_CNode->NextSiblingElement())
                {
                    m_tmp.clear();
                    m_tmp.insert(std::make_pair(CAMTYPE,atoi(Child_CNode->GetText())));
                    photo_config.tablenum++;
                    vm_tmp.push_back(m_tmp);
                }
                vvm_tmp.param.push_back(vm_tmp);
            }
            photo_config.param["row"] = vvm_tmp;

            auto C_str =  photo_config.json_encode();
            std::cout << C_str<< std::endl;
        }
        catch (...) {
            return false;
        }
        return true;
    }


    bool loadserver_config()
    {
        TiXmlDocument doc;
        std::string path_xml = "server_config.xml";
        if (!doc.LoadFile(path_xml.c_str()))
            return false;
        try {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pElem;
            pElem = hDoc.FirstChildElement().Element();
            TiXmlHandle hRoot(pElem);
            TiXmlElement *Cnode = hRoot.FirstChild("server").FirstChild("server_ip").Element();
            ser_config.server_ip = (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("server_port").Element();
            ser_config.server_port = atoi(Cnode->GetText());
            Cnode = hRoot.FirstChild("server").FirstChild("doc_root").Element();
            ser_config.doc_root = (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("threads").Element();
            ser_config.threads = atoi(Cnode->GetText());
            Cnode = hRoot.FirstChild("server").FirstChild("robot_ip").Element();
            ser_config.robot_ip =  (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("ea_ip").Element();
            ser_config.ea_ip =  (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("ea_port").Element();
            ser_config.ea_port =  (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("robot_port").Element();
            ser_config.robot_port =  (const char *) Cnode->GetText();
            Cnode = hRoot.FirstChild("server").FirstChild("timeout").Element();
            ser_config.timeout =  atoi(Cnode->GetText()) ;
            Cnode = hRoot.FirstChild("server").FirstChild("take_photo_times").Element();
            ser_config.cn_photo =  atoi(Cnode->GetText()) ;


        }
        catch (...) {
            return false;
        }
        std::cout<<"load loadserver_config"<<std::endl;
        return true;
    }
};
#endif //ROBOTAI_CONFIG_HPP
