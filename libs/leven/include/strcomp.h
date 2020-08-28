/////////////////////////////////////////////////////////////////////////////
// Name:        strcomp.h
// Project:     dtpLib
// Purpose:     Functions for comparing strings.
// Author:      Piotr Likus
// Modified by:
// Created:     25/04/2012
/////////////////////////////////////////////////////////////////////////////

#ifndef _DTPSTRCOMP_H__
#define _DTPSTRCOMP_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/** \file strcomp.h
\brief Functions for comparing strings.

*/

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
#include <vector>

#include "base/string.h"

// ----------------------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Simple type definitions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Forward class definitions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Class definitions
// ----------------------------------------------------------------------------
// functor for pre-allocation for strDiff algorithm
class strDiffFunctor {
  typedef std::vector<std::vector<unsigned int> > vectorUInt2;
  typedef std::auto_ptr<vectorUInt2> vectorUInt2Guard;
public:
  strDiffFunctor(size_t maxStrLen);
  virtual ~strDiffFunctor() {};
  uint genMaxStrLength() {return m_maxStrLength;};
  uint calc(const dtpString &s1, const dtpString &s2);
protected:
  void prepareForSize(size_t maxStrLen);
protected:
  vectorUInt2Guard m_workMatrix;
  uint m_maxStrLength;
};

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------
/// Calculates Levenshtein distance 
unsigned int strDiffLev(const dtpString &s1, const dtpString &s2);

#endif // _DTPSTRCOMP_H__
