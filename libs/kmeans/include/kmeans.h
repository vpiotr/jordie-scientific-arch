/////////////////////////////////////////////////////////////////////////////
// Name:        kmeans.h
// Project:     scLib
// Purpose:     Clustering using k-means.
// Author:      Piotr Likus
// Modified by:
// Created:     20/12/2009
/////////////////////////////////////////////////////////////////////////////


#ifndef _KMEANS_H__
#define _KMEANS_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/// \file kmeans.h
///
/// Assign class to each entry basing on k-means.

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
#include "sc/dtypes.h"

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
class scKMeansCalculator {
public:
  void execute(const scDataNode &inputVector, scDataNode &output, uint classCount = 5, uint stepLimit = 5);
protected:  
  void initAvg(const scDataNode &inputVector, uint classCount, scDataNode &classAvg);
  uint updateAvg(const scDataNode &inputVector, const scDataNode &itemClassList, scDataNode &classAvg);
  void updateItemClass(const scDataNode &inputVector, const scDataNode &classAvg, scDataNode &itemClassList);
  void prepareInputVector(const scDataNode &inputVector, scDataNode &output);
protected:
};

#endif // _KMEANS_H__
