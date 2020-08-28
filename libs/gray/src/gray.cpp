/////////////////////////////////////////////////////////////////////////////
// Name:        bit.cpp
// Project:     dtpLib
// Purpose:     bit handling functions
// Author:      Piotr Likus
// Modified by:
// Created:     16/04/2013
/////////////////////////////////////////////////////////////////////////////

//dtp
#include "base/bit.h"

uint64 binToGray(uint64 value, uint aSize)
{
  uint64 workValue = value;
  uint64 res;
  uint64 andMask;
  uint bitsLeft = aSize;
  uint lastBit;

  if (bitsLeft > 0) {
    andMask = static_cast<uint64>(1) << (aSize - 1);
    res = workValue & andMask;
    bitsLeft--;
  }
  else {
    res = 0;
    andMask = 0;
  }

  while(bitsLeft > 0)
  {
    lastBit = ((workValue & andMask) != 0)?1:0;
    andMask = andMask >> 1;
    if ((lastBit != 0) != ((workValue & andMask) != 0))
      res = res | andMask;
    bitsLeft--;
  }
  return res;
}

uint binToGray(uint value, uint aSize)
{
  uint workValue = value;
  uint res;
  uint andMask;
  uint bitsLeft = aSize;
  uint lastBit;

  if (bitsLeft > 0) {
    andMask = static_cast<uint>(1) << (aSize - 1);
    res = workValue & andMask;
    bitsLeft--;
  }
  else {
    res = 0;
    andMask = 0;
  }

  while(bitsLeft > 0)
  {
    lastBit = ((workValue & andMask) != 0)?1:0;
    andMask = andMask >> 1;
    if ((lastBit != 0) != ((workValue & andMask) != 0))
      res = res | andMask;
    bitsLeft--;
  }
  return res;
}

uint64 grayToBin(uint64 value, uint aSize)
{
  uint bitValue;
  uint bitsLeft = aSize;
  uint64 res;
  uint64 workValue = value;
  uint64 andMask;

  if (bitsLeft > 0) {
    andMask = static_cast<uint64>(1) << (aSize - 1);
    bitValue = ((workValue & andMask) != 0)?1:0;
    res = workValue & andMask;
    bitsLeft--;
  }
  else {
    res = 0;
    bitValue = 0;
    andMask = 0;
  }

  while (bitsLeft > 0)
  {
    andMask = andMask >> 1;
    if ((workValue & andMask) != 0)
      //bitValue = 1 - bitValue;
      bitValue ^= 1;
    if (bitValue != 0)
      res = res | andMask;
    bitsLeft--;
  }
  return res;
}

uint grayToBin(uint value, uint aSize)
{
  uint bitValue;
  uint bitsLeft = aSize;
  uint res;
  uint workValue = value;
  uint andMask;

  if (bitsLeft > 0) {
    andMask = static_cast<uint>(1) << (aSize - 1);
    bitValue = ((workValue & andMask) != 0)?1:0;
    res = workValue & andMask;
    bitsLeft--;
  }
  else {
    res = 0;
    bitValue = 0;
    andMask = 0;
  }

  while (bitsLeft > 0)
  {
    andMask = andMask >> 1;
    if ((workValue & andMask) != 0)
      //bitValue = 1 - bitValue;
      bitValue ^= 1;
    if (bitValue != 0)
      res = res | andMask;
    bitsLeft--;
  }
  return res;
}
