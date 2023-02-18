//
// Created by leo on 29/07/22.
//

#ifndef PROXY_CACHECONTROL_H
#define PROXY_CACHECONTROL_H

#include "string"
using namespace std;
class CacheControl {
    bool no_cache, if_mod_since;
    int age;
    int expDay, expMonth, expYear, expHour, expMin, expSec;
public:
    CacheControl(string request);

    bool isNoCache() const;

    bool isIfModSince() const;

    int getAge() const;

    int getExpDay() const;

    int getExpMonth() const;

    int getExpYear() const;

    int getExpHour() const;

    int getExpMin() const;

    int getExpSec() const;
};


#endif //PROXY_CACHECONTROL_H
