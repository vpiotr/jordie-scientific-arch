/////////////////////////////////////////////////////////////////////////////
// Name:        PsoOptimizer.h
// Project:     scLib
// Purpose:     PSO optimizer
// Author:      Piotr Likus
// Modified by:
// Created:     10/01/2010
/////////////////////////////////////////////////////////////////////////////


#ifndef _PSOOPTIMIZER_H__
#define _PSOOPTIMIZER_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/// \file PsoOptimizer.h
///
/// 

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
const double PSO_DEF_FACTOR1 = 2.0;
const double PSO_DEF_FACTOR2 = 2.0;
const double PSO_DEF_INERTIA_FACTOR = 0.5;

// ----------------------------------------------------------------------------
// Class definitions
// ----------------------------------------------------------------------------
class scPsoOptimizer: public scObject {
public:
  // -- create
  scPsoOptimizer();
  virtual ~scPsoOptimizer();
  // -- properties
  // for each item: min, max
  void setParamMeta(const scDataNode &meta);
  void setHistoryLength(uint value);
  // set how much history is important in changing speed, values 0..1
  void setInertiaFactor(double value);
  // -- run
  void execute(const scDataNode &itemRating, scDataNode &itemValues);
  void reset();
protected:  
  void initVelocity(const scDataNode &itemValues);
  void updateBestInfo(const scDataNode &itemRating, scDataNode &itemValues);
  void getNeighbours(uint itemId, uint itemCount, scDataNode &output);
  uint findLocalBest(const scDataNode &localItems);
  void updateVelocity(const scDataNode &currPos);
  void updatePosition(const scDataNode &velocity, scDataNode &itemValues);
  virtual void postProcess(scDataNode &itemValues);
  scDataNodeValueType getValueType(int valueIndex);
  double getCurrentScoreForItem(uint itemIndex);
  void getCurrentParamValuesForItem(uint itemIndex, scDataNode &values);
  double getValueMinDouble(uint idx);
  double getValueMaxDouble(uint idx);
  int getValueMinInt(uint idx);
  int getValueMaxInt(uint idx);
protected:  
  // config
  scDataNode m_paramMeta;
  double m_factor1;
  double m_factor2;
  double m_inertiaFactor;
  uint m_historyLength;
  // state
  scDataNode m_bestItemScores;
  scDataNode m_bestItemValues;
  scDataNode m_itemVelocity;
  scDataNode m_valueHistory;
  scDataNode m_scoreHistory;
  uint m_stepNo;
};

#endif // _PSOOPTIMIZER_H__
