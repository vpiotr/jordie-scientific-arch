/////////////////////////////////////////////////////////////////////////////
// Name:        kmeans.cpp
// Project:     scLib
// Purpose:     Clustering using k-means.
// Author:      Piotr Likus
// Modified by:
// Created:     20/12/2009
/////////////////////////////////////////////////////////////////////////////

#include <set>
#include <cmath>

//base
#include "base/rand.h"

//perf
#include "perf/Log.h"

//sc
//#include "sc/smath.h"
#include "sc/utils.h"
#include "sc/dtypes.h"
#include "sc/alg/kmeans.h"
#include "sc/smath.h"

#ifdef DEBUG_MEM
#include "sc/DebugMem.h"
#endif

#define DEBUG_KMEANS
#define USE_LOG10_FILTER

using namespace dtp;
using namespace perf;

template < class T >
T fpSafeLog10(T arg)
{
  return (fpSign<T>(arg) * log10(fpAbs<T>(arg)));
}

void scKMeansCalculator::execute(const scDataNode &inputVector, scDataNode &output, uint classCount, uint stepLimit)
{
  scDataNode itemClassList;
  scDataNode classAvg;
  uint realClassCount;
  uint changedAvgCount;
  uint step;
  scDataNode newInputVector;
  const scDataNode *useInputVector;

#ifdef USE_LOG10_FILTER
  prepareInputVector(inputVector, newInputVector);
  useInputVector = &newInputVector;
#else
  useInputVector = &inputVector;
#endif
      
  output.clear();
    
  if (!classCount) 
    realClassCount = (*useInputVector).size();
  else  
    realClassCount = std::min<uint>(classCount, (*useInputVector).size());

  if (!realClassCount)
    return;
    
  output.setAsArray(vt_uint);      
  for(uint i=0, epos = (*useInputVector).size(); i != epos; i++)
    output.addItemAsUInt(0);
  
#ifdef DEBUG_KMEANS
  Log::addDebug("kmeans-1");  
#endif  
  initAvg((*useInputVector), realClassCount, classAvg);
  step = 0;
  do {
    updateItemClass((*useInputVector), classAvg, output);
    changedAvgCount = updateAvg((*useInputVector), output, classAvg);
    step++;
  } while((changedAvgCount > 0) && (step < stepLimit));  
#ifdef DEBUG_KMEANS
  Log::addDebug("kmeans-2");  
#endif  
}

void scKMeansCalculator::prepareInputVector(const scDataNode &inputVector, scDataNode &output)
{
  bool oneDim = true;
  if (inputVector.size() > 0)
    oneDim = (inputVector.getElement(0).size() < 2);
  uint dimSize = 0;
  if (!oneDim)
    dimSize = inputVector.getElement(0).size();
  uint j;   
  scDataNode element; 
    
  if (oneDim)
    output.setAsArray(vt_double);
  else  
    output.setAsArray(vt_datanode);
      
  for(uint i=0, epos = inputVector.size(); i != epos; i++)
  {
    if (oneDim) {
      output.addItemAsDouble(fpSafeLog10(inputVector.getDouble(i)));
    } else {
      inputVector.getElement(i, element);
      for(j = 0; j != dimSize; j++)
        element.setDouble(j, fpSafeLog10(element.getDouble(j)));
      output.addItem(element);  
    }  
  }  
}

void scKMeansCalculator::initAvg(const scDataNode &inputVector, uint classCount, scDataNode &classAvg)
{
  std::set<uint> centroids;
  uint randomPoint, inputSize;
  inputSize = inputVector.size();
  std::auto_ptr<scDataNode> dataNodeGuard;
  const scDataNode *dataNodePtr;

  do {
    randomPoint = randomUInt(0, inputVector.size() - 1);
    centroids.insert(randomPoint);
  } while ((centroids.size() < classCount) && (centroids.size() < inputSize));

  bool oneDim = true;
  if (inputSize > 0)
    oneDim = (inputVector.getElement(0).size() < 2);
    
  for(std::set<uint>::const_iterator it = centroids.begin(), epos = centroids.end(); it != epos; ++it)
  {
    if (oneDim)
      classAvg.addChild(new scDataNode(inputVector.getDouble(*it)));    
    else {  
      dataNodeGuard.reset(new scDataNode());
      dataNodePtr = &inputVector[*it];
      for(uint i=0, epos = inputVector[*it].size(); i != epos; i++)
        dataNodeGuard->addChild(new scDataNode(dataNodePtr->getDouble(i)));    
      classAvg.addChild(dataNodeGuard.release());
    }  
  }  
}

void scKMeansCalculator::updateItemClass(const scDataNode &inputVector, const scDataNode &classAvg, scDataNode &itemClassList)
{
  scDataNode element;

  uint bestClassIdx;
  double bestClassDist;
  double distance;
  uint classCnt = classAvg.size();
  scDataNode classMin;
  scDataNode classMax;
  scDataNode classSpace;

  if (!inputVector.size())
    return;

  uint dimCount = inputVector.getElement(0).size();
  double value;
    
  if (dimCount > 0)   
  {
    // find dim min and max
    for(uint j=0, eposj = classCnt; j != eposj; j++)
    {
      for(uint k=0, eposk = classAvg[j].size(); k != eposk; k++)
      {
        value = classAvg[j].getDouble(k);
        if (j == 0)
        {
          classMin.addItemAsDouble(value);
          classMax.addItemAsDouble(value);
        } else {
          if (value < classMin.getDouble(k))
            classMin.setDouble(k, value);
          if (value > classMax.getDouble(k))
            classMax.setDouble(k, value);            
        }  
      } // for k  
    } // for j

    // find dim space    
    for(uint k=0, eposk = dimCount; k != eposk; k++)
    {
      if (
          classMin.getDouble(k) ==
          classMax.getDouble(k)
         )
      {    
        classSpace.addItemAsDouble(1.0);
      } else {  
        classSpace.addItemAsDouble(
          fpAbs(
            classMin.getDouble(k) - 
            classMax.getDouble(k)
          )
        );
      }
    } 
    
  } else { // dim = 1
    // find min and max
    for(uint k=0, eposk = classAvg.size(); k != eposk; k++)
    {
      value = classAvg.getDouble(k);
      if (k == 0) {
        classMin.setAsDouble(value);
        classMax.setAsDouble(value);
      } else {
        if (value < classMin.getAsDouble())
          classMin.setAsDouble(value);
        if (value > classMax.getAsDouble())
          classMax.setAsDouble(value);    
      }  
    }    
    // find dim space
    if (
          classMin.getAsDouble() ==
          classMax.getAsDouble()
       )
    {   
      classSpace.setAsDouble(1.0);
    } else {      
      double a = classMin.getAsDouble();
      double b = classMax.getAsDouble();
      classSpace.setAsDouble(
          fpAbs(a - b)
      );    
    }
  }
    
  std::set<uint> itemClassSet;  
  std::set<uint>::const_iterator class_it;
  double a, b;
  
  // assign items to classes  
  for(uint i=0, epos = inputVector.size(); i != epos; i++)
  {
    inputVector.getElement(i, element);
    bestClassIdx = classCnt;
    bestClassDist = 0.0;

    for(uint j=0, eposj = classCnt; j != eposj; j++)
    {
      distance = 0.0;
      if (element.size())
      {
        for(uint k=0, eposk = element.size(); k != eposk; k++)
        {
          distance += 
            fsqr(
              fpAbs(element.getDouble(k) - classAvg[j].getDouble(k))
              /
              classSpace.getDouble(k)
            );  
        }        
      } else {
        a = element.getAsDouble();
        b = classAvg[j].getAsDouble();      

        distance += 
         fsqr(
          fpAbs(a - b)
           /
          classSpace.getAsDouble()
          );
      }      
      distance = sqrt(distance);
      if ((bestClassIdx == classCnt) || (distance < bestClassDist))
      {
        bestClassIdx = j;
        bestClassDist = distance;
        itemClassSet.clear();
      } else if (distance == bestClassDist) 
      {
        if (itemClassSet.empty())
        {
          itemClassSet.insert(bestClassIdx);
        }  
        itemClassSet.insert(j);          
      } 
    } // for j
    
    if (!itemClassSet.empty())
    {
      bestClassIdx = randomUInt(0, itemClassSet.size() - 1);
      class_it = itemClassSet.begin();
      while(bestClassIdx > 0)
      {
        class_it++;
        bestClassIdx--;
      }
      bestClassIdx = *class_it;       
    }
    
    itemClassList.setUInt(i, bestClassIdx);
  } // for i
}

uint scKMeansCalculator::updateAvg(const scDataNode &inputVector, const scDataNode &itemClassList, scDataNode &classAvg)
{
  uint res = 0;
  scDataNode sums;
  scDataNode counts;
  
  if (!inputVector.size())
    return res;
    
  uint dimCount = inputVector.getElement(0).size();
  uint classIdx;
  double oldValue, newValue;
  bool classChanged;

  if (!dimCount) {
    // init values
    counts.setAsArray(vt_int);
    for(uint i=0, epos = classAvg.size(); i != epos; i++)
    {
      sums.addItemAsDouble(0.0);
      counts.addItemAsInt(0);
    }  

    // count + sum
    for(uint i=0, epos = inputVector.size(); i != epos; i++)
    {
      classIdx = itemClassList.getUInt(i);
      sums.setDouble(classIdx, sums.getDouble(classIdx) + inputVector.getDouble(i));
      counts.setInt(classIdx, counts.getUInt(classIdx) + 1);
    }      

    // calc avg
    for(uint i=0, epos = classAvg.size(); i != epos; i++)
    {
      if (counts.getUInt(i) > 0)
      {
        newValue = sums.getDouble(i) / static_cast<double>(counts.getUInt(i));
      } else {
        newValue = 0.0;
      }
      oldValue = classAvg.getDouble(i);
      if (oldValue != newValue)
        res++;
      classAvg.setDouble(i, newValue);
    } // for i   
  } else {
    // init values
    for(uint i=0, epos = classAvg.size(); i != epos; i++)
    {
      sums.addChild(new scDataNode());
      counts.addChild(new scDataNode());
      counts[i].setAsArray(vt_int);
      for(uint j=0; j != dimCount; j++)
      {
        sums[i].addItemAsDouble(0.0);
        counts[i].addItemAsInt(0);
      }  
    }    
    
    // count + sum
    for(uint i=0, epos = inputVector.size(); i != epos; i++)
    {
      classIdx = itemClassList.getUInt(i);
      for(uint j=0; j != dimCount; j++)
      {
        sums[classIdx].setDouble(j, sums[classIdx].getDouble(j) + inputVector[i].getDouble(j));
        counts[classIdx].setInt(j, counts[classIdx].getUInt(j) + 1);
      }  
    }      
    
    // calc avg
    for(uint i=0, epos = classAvg.size(); i != epos; i++)
    {
      classChanged = false;
      
      for(uint j=0; j != dimCount; j++)
      {
        if (counts[i].getUInt(j) > 0)
        {
          newValue = sums[i].getDouble(j) / static_cast<double>(counts[i].getUInt(j));
        } else {
          newValue = 0.0;
        }
        oldValue = classAvg[i].getDouble(j);
        if (oldValue != newValue)
        {
          classAvg[i].setDouble(j, newValue);
          classChanged = true;
        }
      } // for j

      if (classChanged)
        res++;
    } // for i       
  } // if dim > 1   
    
  return res;
}

