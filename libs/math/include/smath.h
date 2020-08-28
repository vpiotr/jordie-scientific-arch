/////////////////////////////////////////////////////////////////////////////
// Name:        smath.h
// Project:     scLib
// Purpose:     Simple math-related definitions
// Author:      Piotr Likus
// Modified by:
// Created:     23/02/2009
/////////////////////////////////////////////////////////////////////////////

#ifndef _SMATH_H__
#define _SMATH_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/// \file smath.h
///
/// Math-related definitions for scLib

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
//std
#include <cmath>
#include <limits>
//sc
#include "base/bmath.h"

// safe invert - f(x) = 1/x
template<typename T>
inline T fpSafeInv(T value)
{
  const T DIV_HELPER = static_cast<T>(+1.0E-100);
  const T DIV_THRESHOLD_PLUS = static_cast<T>(0.000001);
  const T DIV_THRESHOLD_MINUS = -DIV_THRESHOLD_PLUS;
  const T DIV_ONE_T = static_cast<T>(1.0);
  const T DIV_ZERO_T = static_cast<T>(0.0);
  if (value < DIV_ZERO_T) {
    if (value < DIV_THRESHOLD_MINUS)
      return DIV_ONE_T / value;
    else
      return DIV_ONE_T / (value - DIV_HELPER);
  }
  else {
    if (value > DIV_THRESHOLD_PLUS)
      return DIV_ONE_T / value;
    else
      return DIV_ONE_T / (value + DIV_HELPER);
  }
}

/// calculate minimal distance for two points on looped vector (1D tourus)
int tourus_distance(int pos1, int pos2, int fieldWidth);

#endif // _SMATH_H__
