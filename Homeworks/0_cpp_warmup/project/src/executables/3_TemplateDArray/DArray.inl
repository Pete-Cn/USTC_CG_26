// implementation of class DArray
// #include "DArray.h"

#include <assert.h>
#include <iostream>

// default constructor
template <typename T>
DArray<T>::DArray()
{
    Init();
}

// set an array with default values
template <typename T>
DArray<T>::DArray(int nSize, const T &dValue)
{
    Init();
    SetSize(nSize);
    for (int i = 0; i < nSize; i++)
    {
        SetAt(i, dValue);
    }
}

template <typename T>
DArray<T>::DArray(const DArray<T> &arr)
{
    Init();
    SetSize(arr.GetSize());
    for (int i = 0; i < arr.GetSize(); i++)
    {
        SetAt(i, arr.GetAt(i));
    }
}

// deconstructor
template <typename T>
DArray<T>::~DArray()
{
    Free();
}

// display the elements of the array
template <typename T>
void DArray<T>::Print() const
{
    std::cout << "size = " << m_nSize << "\n";
    for (int i = 0; i < m_nSize; ++i)
    {
        std::cout << " " << m_pData[i];
    }
    std::cout << "\n";
}

// initilize the array
template <typename T>
void DArray<T>::Init()
{
    m_pData = nullptr;
    m_nSize = 0;
    m_nMax = 0;
}

// free the array
template <typename T>
void DArray<T>::Free()
{
    delete[] m_pData;
    m_pData = nullptr;
    m_nSize = 0;
    m_nMax = 0;
}

// allocate enough memory
template <typename T>
void DArray<T>::Reserve(int nSize)
{
    if (m_nMax >= nSize)
    {
        return;
    }

    while (m_nMax < nSize)
    {
        m_nMax = (m_nMax == 0) ? 1 : 2 * m_nMax;
    }

    T *new_pData = new T[m_nMax];
    for (int i = 0; i < m_nSize; i++)
    {
        new_pData[i] = m_pData[i];
    }
    delete[] m_pData;
    m_pData = new_pData;
}

// get the size of the array
template <typename T>
int DArray<T>::GetSize() const
{
    return m_nSize;
}

// set the size of the array
template <typename T>
void DArray<T>::SetSize(int nSize)
{
    if (m_nSize == nSize)
    {
        return;
    }

    Reserve(nSize);
    m_nSize = nSize;
}

// get an element at an index
template <typename T>
const T &DArray<T>::GetAt(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// set the value of an element
template <typename T>
void DArray<T>::SetAt(int nIndex, const T &dValue)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    m_pData[nIndex] = dValue;
}

// overload operator '[]'
template <typename T>
const T &DArray<T>::operator[](int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// overload operator '[]' (non-const)
template <typename T>
T &DArray<T>::operator[](int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex];
}

// add a new element at the end of the array
template <typename T>
void DArray<T>::PushBack(const T &dValue)
{
    Reserve(m_nSize + 1);
    m_pData[m_nSize] = dValue;
    ++m_nSize;
}

// delete an element at some index
template <typename T>
void DArray<T>::DeleteAt(int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nSize);
    for (int i = nIndex; i < m_nSize - 1; i++)
    {
        m_pData[i] = m_pData[i + 1];
    }
    --m_nSize;
}

// insert a new element at some index
template <typename T>
void DArray<T>::InsertAt(int nIndex, const T &dValue)
{
    assert(nIndex >= 0 && nIndex <= m_nSize);
    Reserve(m_nSize + 1);
    for (int i = m_nSize; i > nIndex; i--)
    {
        m_pData[i] = m_pData[i - 1];
    }
    m_pData[nIndex] = dValue;
    ++m_nSize;
}

// overload operator '='
template <typename T>
DArray<T> &DArray<T>::operator=(const DArray<T> &arr)
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