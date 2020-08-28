/////////////////////////////////////////////////////////////////////////////
// Name:        bmath.h
// Project:     dtpLib
// Purpose:     Simple math-related definitions
// Author:      Piotr Likus
// Modified by:
// Created:     23/02/2009
/////////////////////////////////////////////////////////////////////////////

#ifndef _BMATH_H__
#define _BMATH_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/// \file bmath.h
///
/// Math-related definitions for scLib

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
//std
#include <cmath>
#include <limits>
//sc
#include "base/btypes.h"

// ----------------------------------------------------------------------------
// Simple type definitions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Forward class definitions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
const long double smath_const_pi = 3.141592653589793238462643383279502884197L; // pi
const long double smath_const_e = 2.718281828459045235360287471352662497757L; // e

// ----------------------------------------------------------------------------
// Template functions
// ----------------------------------------------------------------------------

inline float logn(float base, float x) {
  return log(x) / log(base);
}

inline double logn(double base, double x) {
  return log(x) / log(base);
}

inline xdouble logn(xdouble base, xdouble x) {
  return log(x) / log(base);
}

// round value - template
template<typename T, typename V>
inline T round(V value) {
  const V zeroVal = static_cast<V>(0.0);
  const V halfVal = static_cast<V>(0.5);

  V val1;

  if (value < zeroVal)
    val1 = value - halfVal;
  else
    val1 = value + halfVal;

  T intPart = static_cast<T>(val1);
  return intPart;
}

// check if value is NaN
template<typename T>
inline bool isnan(T value)
{
  return value != value;
}

// check if value is infinity, requires #include <limits>
template<typename T>
inline bool isinf(T value)
{
  return std::numeric_limits<T>::has_infinity &&
       (value == std::numeric_limits<T>::infinity());
}

template<typename T>
inline T check_nan(T value, T defValue)
{
  if (!isnan(value))
    return value;
  else
    return defValue;
}

template<typename T>
inline T check_inf(T value, T defValue)
{
  if (!isinf(value))
    return value;
  else
    return defValue;
}

template < class T >
T fsqr(const T &arg)
{
  return arg * arg;
}

/// Returns -1 for value < 0, +1 for value > 0, otherwise returns 0
template < class T >
int sign(const T &arg)
{
  if (arg > 0)
    return 1;
  else if (arg == 0)
    return 0;
  else
    return -1;
}

/// Returns -1 for value < 0, otherwise returns +1
template < class T >
int sign_nonzero(const T &arg)
{
  if (arg < 0)
    return -1;
  else
    return 1;
}

template < class T >
T fpAbs(T arg)
{
  if (arg < static_cast<T>(0))
    return -arg;
  else
    return arg;
}

template < class T, class U >
int fpComp(T arg1, U arg2)
{
  if (arg1 < arg2)
    return -1;
  else if (arg1 > arg2)
    return 1;
  else
    return 0;
}

/// calculate minimal distance for two points on looped vector (1D tourus)
int tourus_distance(int pos1, int pos2, int fieldWidth);

#endif // _BMATH_H__
