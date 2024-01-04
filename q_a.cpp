#include <iostream>
#include <string>
#include "q_a.h"

using namespace std;


q_a::q_a()
{

}
q_a::q_a(string q, string a)
{
    question = q;
    answer = a;
}

string q_a::getQuestion()
{
    return question;
}

string q_a::getAnswer()
{
    return answer;
}

bool q_a::is_empty()
{
    if (question == "")
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool q_a::isEqualto(string comp)
{
    string a = this->getQuestion();
    if (a == comp)
    {
        return true;
    }
    return false;
}

