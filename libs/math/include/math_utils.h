/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Project:     scLib
// Purpose:     Utility, general-purpose functions
// Author:      Piotr Likus
// Modified by:
// Created:     04/10/2008
/////////////////////////////////////////////////////////////////////////////


#ifndef _UTILS_H__
#define _UTILS_H__

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
//std
#include <sstream>
#include <iomanip>
#include <vector>

//base
#include "base/bit.h"
#include "base/bmath.h"
#include "base/strcomp.h"

//sc
#include "sc/strings.h"
#include "sc/defs.h"
#include "sc/dtypes.h"

#define SC_CHECK_NOT_NULL(a) (scCheck(a != SC_NULL)?a:SC_NULL)

// symbols required because Boost includes WinDefs.h with min/max incompatible with std versions
#define SC_MIN(a,b) (std::min)(a,b)
#define SC_MAX(a,b) (std::max)(a,b)

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

// use this class to perform to-string conversion in loop
template < class T >
class toStringGate {
  std::ostringstream	m_out;
public:
  scString &toString(const T &arg, scString &output) {
    std::ostringstream tmp;
    //m_out.clear();
#if defined (_MSC_VER)
    m_out.swap(tmp);
#else
    m_out = tmp;
#endif
    m_out << arg;
    output = scString(m_out.str());
    return output;
  }
};


// logical xor
template < class T, class Y >
bool log_xor(const T &arg1, const Y &arg2)
{
  bool flag1 = bool(arg1);
  bool flag2 = bool(arg2);
	return(flag1 != flag2);
}

bool equFloat(float val1, float val2);
bool equDouble(double val1, double val2);
bool equXDouble(xdouble val1, xdouble val2);

// ----------------------------------------------------------------------------
// date & time handling
// ----------------------------------------------------------------------------

double cpu_timef();


// ----------------------------------------------------------------------------
// calculators
// ----------------------------------------------------------------------------

template < class T >
T fpSign(T arg)
{
  if (arg < static_cast<T>(0.0))
    return static_cast<T>(-1.0);
  else
    return static_cast<T>(1.0);
}

// Calculate relative error using min/max formula
// Another way could be: abs(a1 - a2)/a2
// Returns value 0..1, 0 -> no difference, 1 - maximum error / difference
template < class T >
T relativeErrorMinMax(T arg1, T arg2)
{
  const T DIV_HELPER = 1E-100;
  T absArg1 = fpAbs<T>(arg1);
  T absArg2 = fpAbs<T>(arg2);
  T minValue = std::min<T>(absArg1, absArg2);
  T maxValue = std::max<T>(absArg1, absArg2);
  if (log_xor((arg1 < 0.0), (arg2 < 0.0)))
    maxValue = maxValue + (2.0 * minValue);

  return (static_cast<T>(1.0) - (DIV_HELPER + minValue) / (DIV_HELPER + maxValue));
}

// Returns sigmoidal-like value between 0..1, where
// x - value between 0..1 (0 - result = min, 1 - result = max)
// a - value between 0..1 (0 - sharp, 1 - flat)
// result - value between 0..1
double centerDistribFormula(double x, double a);

scString genGuid(uint initValue);

#include "base/object.h"
#include "base/file_utils.h"
#include "perf/time_utils.h"

#endif // _UTILS_H__
