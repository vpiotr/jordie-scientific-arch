/////////////////////////////////////////////////////////////////////////////
// Name:        rand.h
// Project:     scLib
// Purpose:     Random number handling
// Author:      Piotr Likus
// Modified by:
// Created:     13/02/2008
/////////////////////////////////////////////////////////////////////////////


#ifndef _SCRAND_H__
#define _SCRAND_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/// \file rand.h
///
/// Random numbers support using Mersene-Twister generator.
/// Note: this module is NOT thread-safe.

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
#include "base/btypes.h"
#include "base/string.h"

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

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
void randomize();
double randomDouble(double a_min, double a_max);
xdouble randomXDouble(xdouble a_min, xdouble a_max);
int randomInt(int a_min, int a_max);
uint randomUInt(uint a_min, uint a_max);
bool randomBool();
bool randomFlip(double aProb);
void randomString(const dtpString &alphabet, uint a_size, dtpString &output);

#endif // _SCRAND_H__
