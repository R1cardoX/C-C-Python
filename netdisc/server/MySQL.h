#ifndef _MYSQL_H_
#define _MYSQL_H_
#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
//g++ -std=c++11 -DSTATIC_CONCPP -I /usr/include  sql.cc  /usr/lib/libmysqlcppconn8-static.a -lssl -lcrypto -lpthread -o app
class MySQL
{
public:
    MySQL(int port,std::string user,std::string pwd):sess(port,user,pwd)
    {
        mysqlx::RowResult res = sess.sql("show variables like 'version'").execute();
        std::stringstream version;
        version << res.fetchOne().get(1).get<std::string>();
        int major_version;
        version >> major_version;
        coll.remove("true").execute();
    }
    ~MySQL()
    {
    }
    void Add(std::string data,bool flag = false)
    {
        add = this->coll.add(data).execute();
        if(flag)
            ids = add.getGeneratedIds();
        if (ids.size() != 0)
          std::cout <<"- added doc with id: " << ids[0] <<std::endl;
        else
          std::cout <<"- added doc" <<std::endl;
    }
    void Find(std::string data)
    {
        int i=0;
        docs = coll.find(data).execute();
        while(true)
        {
            doc = docs.fetchOne();
            if(!doc)
                break;
            std::cout<<"doc#"<<i<<": "<<doc<<std::endl;
            for(auto fld:doc)
            {
                std::cout << " field `" << fld << "`: " <<doc[fld] << std::endl;
            }
            std::string name = doc["name"];
            std::cout<<"name: "<<name<<std::endl;
            using namespace std;
            using namespace mysqlx;
            if (doc.hasField("date") && Value::DOCUMENT == doc.fieldType("date"))
            {
              cout << "- date field" << endl;
              DbDoc date = doc["date"];
              for (Field fld : date)
              {
                cout << "  date `" << fld << "`: " << date[fld] << endl;
              }
              std::string month = doc["date"]["month"];
              int day = date["day"];
              cout << "  month: " << month << endl;
              cout << "  day: " << day << endl;
            }
            if (doc.hasField("toys") && Value::ARRAY == doc.fieldType("toys"))
            {
              cout << "- toys:" << endl;
              for (auto toy : doc["toys"])
              {
                cout << "  " << toy << endl;
              }
            }
            cout << endl;
            i++;
        }
    }
private:
    mysqlx::Session sess;
    std::vector<std::string> ids;
    mysqlx::Result add;
    mysqlx::Schema sch= sess.getSchema("test");
    mysqlx::Collection coll= sch.createCollection("c1", true);
    mysqlx::DocResult docs;
    mysqlx::DbDoc doc;
};
#endif
