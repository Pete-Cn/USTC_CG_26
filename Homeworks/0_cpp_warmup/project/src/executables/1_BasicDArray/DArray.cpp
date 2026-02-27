// implementation of class DArray
#include "DArray.h"
#include <algorithm>
#include <assert.h>
#include <cstdio>

// default constructor
DArray::DArray()
{
    Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue)
{
    Init();
    SetSize(nSize);
    for (int i = 0; i < nSize; i++)
    {
        SetAt(i, dValue);
    }
}

DArray::DArray(const DArray &arr)
{
    Init();
    SetSize(arr.GetSize());
    for (int i = 0; i < arr.GetSize(); i++)
    {
        SetAt(i, arr.GetAt(i));
    }
}

// deconstructor
DArray::~DArray()
{
    Free();
}

// display the elements of the array
void DArray::Print() const
{
    std::printf("size= %d:", m_nSize);
    for (int i = 0; i < m_nSize; ++i)
    {
        std::printf(" %f", m_pData[i]);
    }
    std::printf("\n");
}

// initilize the array
void DArray::Init()
{
    m_pData = nullptr;
    m_nSize = 0;
}

// free the array
void DArray::Free()
{
    delete[] m_pData;
    m_pData = nullptr;
    m_nSize = 0;
}

// get the size of the array
int DArray::GetSize() const
{
    return m_nSize;
}

// set the size of the array
void DArray::SetSize(int nSize)
{
    if (m_nSize == nSize)
    {
        return;
    }

    double *new_pData = new double[nSize];
    int copyCount = m_nSize < nSize ? m_nSize : nSize;
    for (int i = 0; i < copyCount; i++)
    {
        new_pData[i] = m_pData[i];
    }

    delete[] m_pData;
    m_pData = new_pData;
    m_nSize = nSize;
}

// get an element at an index
const double &DArray::GetAt(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// set the value of an element
void DArray::SetAt(int nIndex, double dValue)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    m_pData[nIndex] = dValue;
}

// overload operator '[]'
const double &DArray::operator[](int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// overload operator '[]' (non-const)
double &DArray::operator[](int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// add a new element at the end of the array
void DArray::PushBack(double dValue)
{
    SetSize(m_nSize + 1);
    m_pData[m_nSize - 1] = dValue;
}

// delete an element at some index
void DArray::DeleteAt(int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    for (int i = nIndex; i < m_nSize - 1; i++)
    {
        m_pData[i] = m_pData[i + 1];
    }
    --m_nSize;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue)
{
    assert(nIndex >= 0 && nIndex <= m_nSize); // nIndex == m_nSize is legal
    SetSize(m_nSize + 1);
    for (int i = m_nSize - 1; i > nIndex; i--)
    {
        m_pData[i] = m_pData[i - 1];
    }
    m_pData[nIndex] = dValue;
}

// overload operator '='
DArray &DArray::operator=(const DArray &arr)
{
    if (arr.m_pData == nullptr)
    {
        Init();
    }
    else
    {
        SetSize(arr.GetSize());
        for (int i = 0; i < arr.GetSize(); i++)
        {
            SetAt(i, arr.GetAt(i));
        }
    }
    return *this;
}
