#pragma once
#ifndef USER_CACHE_H
#define USER_CACHE_H

#include <vector>
#include <string>
#include <iostream>
#include "q_a.h"

using namespace std;

class user_Cache {

private:

	//parallel vectors
	vector<q_a> cached_data;


public:
	user_Cache();

	void storeToCache(q_a data);

	q_a SearchCache(string data);

	void clearCache();

};

#endif