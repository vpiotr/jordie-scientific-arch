/////////////////////////////////////////////////////////////////////////////
// Name:        PsoOptimizer.cpp
// Project:     scLib
// Purpose:     PSO optimizer
// Author:      Piotr Likus
// Modified by:
// Created:     10/01/2010
/////////////////////////////////////////////////////////////////////////////

#include "base/rand.h"

#include "sc/alg/PsoOptimizer.h"
#include "sc/utils.h"
#include "sc/smath.h"

#ifdef DEBUG_MEM
#include "sc/DebugMem.h"
#endif

const uint META_IDX_MIN = 0;
const uint META_IDX_MAX = 1;

using namespace dtp;

scPsoOptimizer::scPsoOptimizer()
{
  m_factor1 = PSO_DEF_FACTOR1;
  m_factor2 = PSO_DEF_FACTOR2;
  m_inertiaFactor = PSO_DEF_INERTIA_FACTOR;
  m_historyLength = 0;
  m_stepNo = 0;
}

scPsoOptimizer::~scPsoOptimizer()
{
}

// properties
void scPsoOptimizer::setParamMeta(const scDataNode &meta)
{
  m_paramMeta = meta;
}

void scPsoOptimizer::setHistoryLength(uint value)
{
  m_historyLength = value;
}

// run
void scPsoOptimizer::reset()
{
  m_bestItemScores.clear();
  m_bestItemValues.clear();
  m_itemVelocity.clear();
  m_valueHistory.clear();
  m_scoreHistory.clear();
  m_stepNo = 0;
}

void scPsoOptimizer::execute(const scDataNode &itemRating, scDataNode &itemValues)
{
  if (m_itemVelocity.empty())
  { // init velocity with pos
    initVelocity(itemValues);
  }  
  updateBestInfo(itemRating, itemValues);
  updateVelocity(itemValues);
  updatePosition(m_itemVelocity, itemValues);
  postProcess(itemValues);
  m_stepNo++;
}

// fill m_itemVelocity with random values based on itemValues;
void scPsoOptimizer::initVelocity(const scDataNode &itemValues)
{
  const scDataNode *itemPtr;
  double v;
  scString itemName;

  m_itemVelocity.clear();
  for(uint i=0, epos = itemValues.size(); i != epos; i++)
  {
    itemName = itemValues.getElementName(i);
    m_itemVelocity.addChild(itemName, new scDataNode());
    for(uint j = 0, eposj = itemValues[i].size(); j != eposj; j++)
    {
      itemPtr = &(itemValues[i]);
      if (getValueType(j) == vt_int)
      {
        v = static_cast<double>(itemPtr->getInt(j)) * randomDouble(0.1, 1.0);
        m_itemVelocity[itemName].addChild(new scDataNode(round<int64>(v)));
      } else {
        v = itemPtr->getDouble(j) * randomDouble(0.1, 1.0);
        m_itemVelocity[itemName].addChild(new scDataNode(v));
      }  
    } // for j
  } // for i         
}  

void scPsoOptimizer::updateBestInfo(const scDataNode &itemRating, scDataNode &itemValues)
{
  scString itemName;
  // update history
  if (m_historyLength > 0)
  {
    uint idx;
    for(uint i=0, epos = itemValues.size(); i != epos; i++)
    {
      itemName = itemValues.getElementName(i);
      if (!m_valueHistory.hasChild(itemName))
      {
        m_valueHistory.addChild(itemName, new scDataNode());
        m_scoreHistory.addChild(itemName, new scDataNode());
      }  
      if (m_valueHistory[itemName].size() < m_historyLength)
      {
        m_valueHistory[itemName].addChild(new scDataNode(itemValues[i]));
        m_scoreHistory[itemName].addChild(new scDataNode(itemRating[i]));
      }  
      else { 
        idx = m_stepNo % m_historyLength;   
        m_valueHistory[itemName].setElement(idx, itemValues[i]);
        m_scoreHistory[itemName].setDouble(idx, itemRating.getDouble(itemName));
      }  
    } 

    // update best from history
    double bestScore = 0.0;
    uint bestPos;
    for(uint i=0, epos = m_scoreHistory.size(); i != epos; i++)
    {
      itemName = m_scoreHistory.getElementName(i);
      bestScore = 0.0;
      bestPos = 0;
      for(uint j=0, eposj = m_scoreHistory[itemName].size(); j != eposj; j++)
      {
        if ((j == 0) || (m_scoreHistory[itemName].getDouble(j) > bestScore))
        {
          bestScore = m_scoreHistory[itemName].getDouble(j);
          bestPos = j;
        }  
      }
      
      if (!m_bestItemScores.hasChild(itemName))
        m_bestItemScores.addChild(itemName, new scDataNode()); 
      if (!m_bestItemValues.hasChild(itemName))
        m_bestItemValues.addChild(itemName, new scDataNode()); 

      m_bestItemScores.setDouble(itemName, bestScore);
      m_bestItemValues.setElement(itemName, m_valueHistory[itemName][bestPos]);
    }
  } else {
    // no history - update best using present value
    for(uint i=0, epos = itemRating.size(); i != epos; i++)
    {
      itemName = itemRating.getElementName(i);
      if (!m_bestItemValues.hasChild(itemName))
      {
        m_bestItemValues = itemValues;
        m_bestItemScores = itemRating;
      } else {
        if (itemRating.getDouble(i) > m_bestItemScores.getDouble(itemName))
        {
          m_bestItemScores.setDouble(itemName, itemRating.getDouble(i));
          m_bestItemValues[i] = itemValues[itemName];
        }
      }
    }    
  }
}

void scPsoOptimizer::getNeighbours(uint itemId, uint itemCount, scDataNode &output)
{
  output.clear();
  output.addChild(new scDataNode(itemId));
  if (itemId == 0)
  {
    output.addChild(new scDataNode(itemCount - 1));    
  } else {
    output.addChild(new scDataNode(itemId - 1));    
  }   
  if (itemId == itemCount - 1)
  {
    output.addChild(new scDataNode(0));      
  } else {
    output.addChild(new scDataNode(itemId + 1));    
  }   
}

void scPsoOptimizer::updateVelocity(const scDataNode &currPos)
{  
  scString itemName, localBestName;
  uint itemId, localBestId;
  scDataNode localItems;
  double velocity;
  int intVelocity;
  double currPosX, oldVelocity;
  scDataNode localBestValues;
  
  for(uint i=0, epos = m_bestItemValues.size(); i != epos; i++)
  {
    itemName = m_bestItemValues.getElementName(i);
    itemId = stringToUIntDef(itemName, 0);
    localItems.clear();
    getNeighbours(itemId, epos, localItems);
    localBestId = findLocalBest(localItems);
    localBestName = toString(localBestId);
    getCurrentParamValuesForItem(localBestId, localBestValues);
    for(uint j=0, eposj = currPos[itemName].size(); j != eposj; j++)
    {
      currPosX = currPos[itemName].getDouble(j);
      oldVelocity = m_itemVelocity[itemName].getDouble(j);
      velocity = 
        oldVelocity + 
        m_factor1 * randomDouble(0.0, 1.0) * (m_bestItemValues[itemName].getDouble(j) - currPosX);
        
      if (localBestId != itemId)
        velocity += m_factor2 * randomDouble(0.0, 1.0) * 
          (
           localBestValues.getDouble(j)
            - 
           currPosX
          );

      // update velocity with inertia
      velocity = (m_inertiaFactor * oldVelocity) + (1.0 - m_inertiaFactor) * velocity;
      
      // verify limits 
      if (getValueType(j) == vt_int)
      {  
        currPosX += velocity;
        intVelocity = round<int64>(currPosX);  
        if (intVelocity < getValueMinInt(j))
          velocity += (1.0 + randomDouble(0.0, 0.5))*static_cast<double>(getValueMinInt(j) - intVelocity);
        else if (intVelocity > getValueMaxInt(j))
          velocity -= (1.0 + randomDouble(0.0, 0.5))*static_cast<double>(intVelocity - getValueMaxInt(j));            
      } else {
        currPosX += velocity;
        if (currPosX < getValueMinDouble(j))
          velocity += (1.0 + randomDouble(0.0, 0.5))*(getValueMinDouble(j) - currPosX);
        else if (currPosX > getValueMaxDouble(j))
          velocity -= (1.0 + randomDouble(0.0, 0.5))*(currPosX - getValueMaxDouble(j));            
      } 

      // update
      m_itemVelocity[itemName].setDouble(j, velocity);  
    }
  }
}

double scPsoOptimizer::getValueMinDouble(uint idx)
{
  return m_paramMeta.getElement(idx).getDouble(META_IDX_MIN);
}

double scPsoOptimizer::getValueMaxDouble(uint idx)
{
  return m_paramMeta.getElement(idx).getDouble(META_IDX_MAX);
}

int scPsoOptimizer::getValueMinInt(uint idx)
{
  return m_paramMeta.getElement(idx).getInt(META_IDX_MIN);
}

int scPsoOptimizer::getValueMaxInt(uint idx)
{
  return m_paramMeta.getElement(idx).getInt(META_IDX_MAX);
}

uint scPsoOptimizer::findLocalBest(const scDataNode &localItems)
{
  uint bestPos = localItems.size();
  double bestScore = 0.0;
  double itemScore;
  if (!localItems.empty())  
  {
    for(uint i=0, epos = localItems.size(); i != epos; i++)
    {
      //itemScore = m_bestItemScores.getDouble(localItems.getUInt(i));
      itemScore = getCurrentScoreForItem(localItems.getUInt(i));
      if ((i == 0) || (itemScore > bestScore))
      {
        bestPos = localItems.getUInt(i);
        bestScore = itemScore;
      }
    } // for    
  }
  return bestPos;
}

scDataNodeValueType scPsoOptimizer::getValueType(int valueIndex)
{
  return m_paramMeta.getElement(valueIndex)[META_IDX_MIN].getValueType();
}

void scPsoOptimizer::updatePosition(const scDataNode &velocity, scDataNode &itemValues)
{
  scString itemName;
  for(uint i=0, epos = itemValues.size(); i != epos; i++)
  {
    itemName = itemValues.getElementName(i);
    for(uint j=0, eposj = itemValues[itemName].size(); j != eposj; j++)
    {
      if (getValueType(j) == vt_int)
      {
        itemValues[itemName].setInt(j, 
          round<int>(
            static_cast<double>(itemValues[itemName].getInt(j)) +
            velocity[itemName].getDouble(j)
          )
        );  
      } else {
      // double
        itemValues[itemName].setDouble(j, 
          itemValues[itemName].getDouble(j) +
            velocity[itemName].getDouble(j)
        );    
      }
    } // for j
  } // for i  
} // func

void scPsoOptimizer::postProcess(scDataNode &itemValues)
{ //empty
}

double scPsoOptimizer::getCurrentScoreForItem(uint itemIndex)
{
  double res;
  scString itemName(toString(itemIndex));
  
  if (m_historyLength > 0)
  {
    uint idx = m_stepNo % m_historyLength;   
    res = m_scoreHistory[itemIndex].getDouble(idx);
  } else {
    res = m_bestItemScores.getDouble(itemName);
  }
  return res;  
}

void scPsoOptimizer::getCurrentParamValuesForItem(uint itemIndex, scDataNode &values)
{
  scString itemName(toString(itemIndex));
  
  values.clear();
  
  if (m_historyLength > 0)
  {
    uint idx = m_stepNo % m_historyLength;   
    m_valueHistory[itemName].getElement(idx, values);
  } else {
    values = m_bestItemValues[itemName];
  }
}


