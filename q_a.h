#pragma once
#ifndef Q_A_H
#define Q_A_H

#include <iostream>
#include <string>

using namespace std;

class q_a
{
private:
	string question = "";
	string answer = "";


public:
	q_a();

	q_a(string q, string a);

	string getQuestion();

	string getAnswer();

	bool is_empty();

	bool isEqualto(string comp);

};

#endif