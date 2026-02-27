#include "PolynomialMap.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

constexpr double EPSILON = 1e-12;

PolynomialMap::PolynomialMap(const PolynomialMap &other)
{
    m_Polynomial = other.m_Polynomial;
}

PolynomialMap::PolynomialMap(const string &file)
{
    ReadFromFile(file);
}

PolynomialMap::PolynomialMap(const double *cof, const int *deg, int n)
{
    m_Polynomial.clear();
    for (int i = 0; i < n; ++i)
    {
        m_Polynomial[deg[i]] = cof[i];
    }
}

PolynomialMap::PolynomialMap(const vector<int> &deg, const vector<double> &cof)
{
    assert(deg.size() == cof.size());
    m_Polynomial.clear();
    for (int i = 0; i < deg.size(); ++i)
    {
        m_Polynomial[deg[i]] = cof[i];
    }
}

double PolynomialMap::coff(int i) const
{
    auto it = m_Polynomial.find(i);
    if (it != m_Polynomial.end())
    {
        return it->second;
    }
    return 0.0;
}

double &PolynomialMap::coff(int i)
{
    return m_Polynomial[i];
}

void PolynomialMap::compress()
{
    auto it = m_Polynomial.begin();
    while (it != m_Polynomial.end())
    {
        if (fabs(it->second) < EPSILON)
        {
            it = m_Polynomial.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap &right) const
{
    PolynomialMap rst = *this;
    for (const auto &term : right.m_Polynomial)
    {
        rst.m_Polynomial[term.first] += term.second;
    }
    rst.compress();
    return rst;
}

PolynomialMap PolynomialMap::operator-(const PolynomialMap &right) const
{
    PolynomialMap rst = *this;
    for (const auto &term : right.m_Polynomial)
    {
        rst.m_Polynomial[term.first] -= term.second;
    }
    rst.compress();
    return rst;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap &right) const
{
    PolynomialMap rst = {};
    for (const auto &term1 : m_Polynomial)
    {
        for (const auto &term2 : right.m_Polynomial)
        {
            int deg = term1.first + term2.first;
            double cof = term1.second * term2.second;
            rst.m_Polynomial[deg] += cof;
        }
    }
    rst.compress();
    return rst;
}

PolynomialMap &PolynomialMap::operator=(const PolynomialMap &right)
{
    m_Polynomial = right.m_Polynomial;
    return *this;
}

void PolynomialMap::Print() const
{
    bool first = true;
    for (auto it = m_Polynomial.begin(); it != m_Polynomial.end(); ++it)
    {
        if (fabs(it->second) < EPSILON)
        {
            continue;
        }
        if (!first)
        {
            cout << " ";
            if (it->second > 0)
            {
                cout << "+";
            }
        }
        first = false;
        cout << it->second;
        if (it->first > 0)
        {
            cout << "x^" << it->first;
        }
    }
    if (first)
    {
        cout << "0";
    }
    cout << endl;
}

bool PolynomialMap::ReadFromFile(const string &file)
{
    m_Polynomial.clear();

    ifstream inpt;
    inpt.open(file);
    if (!inpt.is_open())
    {
        return false;
    }
    char ch;
    int n, deg;
    double cof;
    inpt >> ch >> n;
    while (n--)
    {
        inpt >> deg >> cof;
        m_Polynomial[deg] = cof;
    }
    inpt.close();
    return true;
}
