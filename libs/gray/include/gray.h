/////////////////////////////////////////////////////////////////////////////
// Name:        bit.h
// Project:     dtpLib
// Purpose:     bit handling functions
// Author:      Piotr Likus
// Modified by:
// Created:     16/04/2013
/////////////////////////////////////////////////////////////////////////////

#ifndef _BIT_H__
#define _BIT_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/** \file bit.h
\brief bit handling functions

Long description
*/

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

// base
#include "base/btypes.h"

/// Convert binary value to Gray code
uint64 binToGray(uint64 value, uint aSize);
uint binToGray(uint value, uint aSize);

/// Convert Gray code value to binary value
uint64 grayToBin(uint64 value, uint aSize);
uint grayToBin(uint value, uint aSize);

#endif // _BIT_H__
