#include <vector>
#include <string>
#include <iostream>
#include "q_a.h"
#include "user_Cache.h"

using namespace std;


user_Cache::user_Cache()
{
}

void user_Cache::storeToCache(q_a data)
{
    cached_data.push_back(data);
}

q_a user_Cache::SearchCache(string data)
{
    for (int i = 0; i < cached_data.size(); i++)
    {

        if (cached_data[i].isEqualto(data))
        {
            string a = cached_data[i].getAnswer();
            q_a obj(data, a);
            return obj;
        }

    }

    q_a empty;
    return empty;
}

void user_Cache::clearCache()
{
    cached_data.clear();
}

