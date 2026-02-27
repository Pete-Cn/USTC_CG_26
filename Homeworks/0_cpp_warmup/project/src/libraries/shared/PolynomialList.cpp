#include "PolynomialList.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include <assert.h>

using namespace std;

constexpr double EPSILON = 1e-12;

PolynomialList::PolynomialList(const PolynomialList &other)
{
    m_Polynomial = other.m_Polynomial;
}

PolynomialList::PolynomialList(const string &file)
{
    ReadFromFile(file);
}

PolynomialList::PolynomialList(const double *cof, const int *deg, int n)
{
    m_Polynomial.clear();
    for (int i = 0; i < n; ++i)
    {
        AddOneTerm(Term(deg[i], cof[i]));
    }
}

PolynomialList::PolynomialList(const vector<int> &deg, const vector<double> &cof)
{
    assert(deg.size() == cof.size());
    m_Polynomial.clear();
    for (int i = 0; i < deg.size(); ++i)
    {
        AddOneTerm(Term(deg[i], cof[i]));
    }
}

double PolynomialList::coff(int i) const
{
    for (auto it = m_Polynomial.begin(); it != m_Polynomial.end(); ++it)
    {
        if (it->deg == i)
        {
            return it->cof;
        }
        if (it->deg > i)
        {
            break;
        }
    }
    return 0.0; // you should return a correct value
}

double &PolynomialList::coff(int i)
{
    static double ERROR = 0.0; // you should delete this line
    for (auto it = m_Polynomial.begin(); it != m_Polynomial.end(); ++it)
    {
        if (it->deg == i)
        {
            return it->cof;
        }
        if (it->deg > i)
        {
            break;
        }
    }
    return ERROR; // you should return a correct value
}

void PolynomialList::compress()
{
    auto it = m_Polynomial.begin();
    while (it != m_Polynomial.end())
    {
        if (fabs(it->cof) < EPSILON)
        {
            it = m_Polynomial.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList &right) const
{
    PolynomialList rst = *this;
    for (auto it = right.m_Polynomial.begin(); it != right.m_Polynomial.end(); ++it)
    {
        rst.AddOneTerm(*it);
    }
    rst.compress();
    return rst;
}

PolynomialList PolynomialList::operator-(const PolynomialList &right) const
{
    PolynomialList rst = *this;
    for (auto it = right.m_Polynomial.begin(); it != right.m_Polynomial.end(); ++it)
    {
        rst.AddOneTerm(Term(it->deg, -it->cof));
    }
    rst.compress();
    return rst;
}

PolynomialList PolynomialList::operator*(const PolynomialList &right) const
{
    PolynomialList rst = {};
    for (auto it1 = m_Polynomial.begin(); it1 != m_Polynomial.end(); ++it1)
    {
        for (auto it2 = right.m_Polynomial.begin(); it2 != right.m_Polynomial.end(); ++it2)
        {
            double new_cof = it1->cof * it2->cof;
            int new_deg = it1->deg + it2->deg;
            rst.AddOneTerm(Term(new_deg, new_cof));
        }
    }
    return rst;
}

PolynomialList &PolynomialList::operator=(const PolynomialList &right)
{
    m_Polynomial = right.m_Polynomial;
    return *this;
}

void PolynomialList::Print() const
{
    for (auto it = m_Polynomial.begin(); it != m_Polynomial.end(); ++it)
    {
        if (fabs(it->cof) < EPSILON)
        {
            continue;
        }
        if (it != m_Polynomial.begin())
        {
            cout << " ";
            if (it->cof > 0)
            {
                cout << "+";
            }
        }
        cout << it->cof;
        if (it->deg > 0)
        {
            cout << "x^" << it->deg;
        }
    }
    cout << endl;
}

bool PolynomialList::ReadFromFile(const string &file)
{
    m_Polynomial.clear();

    ifstream inpt;
    inpt.open(file);
    if (!inpt.is_open())
    {
        cout << "ERROR::PolynomialList::ReadFromFile:" << endl
             << "\t" << "file [" << file << "] opens failed" << endl;
        return false;
    }
    char ch;
    int n, deg;
    double cof;
    inpt >> ch >> n;
    while (n--)
    {
        inpt >> deg >> cof;
        AddOneTerm(Term(deg, cof));
    }
    inpt.close();
    return true;
}

PolynomialList::Term &PolynomialList::AddOneTerm(const Term &term)
{
    auto it = m_Polynomial.begin();
    for (; it != m_Polynomial.end(); ++it)
    {
        if (it->deg == term.deg)
        {
            it->cof += term.cof;
            return *it;
        }
        if (it->deg > term.deg)
        {
            break;
        }
    }
    return *m_Polynomial.insert(it, term);
}
