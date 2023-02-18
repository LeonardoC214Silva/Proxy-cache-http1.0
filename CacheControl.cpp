//
// Created by leo on 29/07/22.
//

#include "CacheControl.h"

CacheControl::CacheControl(string request) {
    size_t pos;
    no_cache = if_mod_since = false;
    age = 0;
    expDay = expMonth = expYear = expHour = expMin = expSec = 0;
    pos=request.find("no-cache");
    if (pos != string::npos) no_cache=true;
    pos= request.find("private");
    if (pos != string::npos) no_cache=true;
    pos=request.find("If-Modified-Since");
    if (pos != string::npos)if_mod_since=true;
    pos=request.find("max-age=");
    if (pos != string::npos){
        string txtAux=request.data()+pos+8;
        pos=txtAux.find("\r\n");
        txtAux.erase(pos);
        age=stoi(txtAux);
    }
    pos=request.find("Expires: ");

    if (pos != string::npos){
        string txtAux = request.data() + pos + 8;//pula o 'Expires: '
        string aux2=txtAux;
        pos = request.find("\r\n");
        aux2.erase(pos);

        if(aux2=="-1") {



            string strnum;
            pos = txtAux.find(',');
            pos += 2;//pula ', '
            txtAux.erase(0, pos);

            strnum = txtAux;
            pos = strnum.find(' ');
            strnum.erase(pos);
            expDay = stoi(strnum);
            txtAux = txtAux.data() + pos + 1;//avança proximo campo

            strnum = txtAux;
            pos = strnum.find(' ');
            strnum.erase(pos);
            if (strnum == "Jan") {
                expMonth = 1;
            }
            if (strnum == "Feb") {
                expMonth = 2;
            }
            if (strnum == "Mar") {
                expMonth = 3;
            }
            if (strnum == "Apr") {
                expMonth = 4;
            }
            if (strnum == "May") {
                expMonth = 5;
            }
            if (strnum == "Jun") {
                expMonth = 6;
            }
            if (strnum == "Jul") {
                expMonth = 7;
            }
            if (strnum == "Aug") {
                expMonth = 8;
            }
            if (strnum == "Sep") {
                expMonth = 9;
            }
            if (strnum == "Oct") {
                expMonth = 10;
            }
            if (strnum == "Nov") {
                expMonth = 11;
            }
            if (strnum == "Dec") {
                expMonth = 12;
            }

            txtAux = txtAux.data() + pos + 1;//avança proximo campo
            strnum = txtAux;
            pos = strnum.find(' ');
            strnum.erase(pos);
            expYear = stoi(strnum);
            txtAux = txtAux.data() + pos + 1;//avança proximo campo

            strnum = txtAux;
            pos = strnum.find(':');
            strnum.erase(pos);
            expHour = stoi(strnum);
            txtAux = txtAux.data() + pos + 1;//avança proximo campo

            strnum = txtAux;
            pos = strnum.find(':');
            strnum.erase(pos);
            expMin = stoi(strnum);

            txtAux = txtAux.data() + pos + 1;//avança proximo campo
            strnum = txtAux;
            pos = strnum.find(' ');
            strnum.erase(pos);
            expSec = stoi(strnum);
        }
    }


}

bool CacheControl::isNoCache() const {
    return no_cache;
}

bool CacheControl::isIfModSince() const {
    return if_mod_since;
}

int CacheControl::getAge() const {
    return age;
}

int CacheControl::getExpDay() const {
    return expDay;
}

int CacheControl::getExpMonth() const {
    return expMonth;
}

int CacheControl::getExpYear() const {
    return expYear;
}

int CacheControl::getExpHour() const {
    return expHour;
}

int CacheControl::getExpMin() const {
    return expMin;
}

int CacheControl::getExpSec() const {
    return expSec;
}
