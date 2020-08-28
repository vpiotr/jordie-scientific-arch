/////////////////////////////////////////////////////////////////////////////
// Name:        series.cpp
// Project:     scLib
// Purpose:     Singla processing functions.
// Author:      Piotr Likus
// Modified by:
// Created:     04/06/2011
/////////////////////////////////////////////////////////////////////////////

#define NOMINMAX
#include <set>
#include <algorithm>

#include "base/btypes.h"
#include "base/algorithm.h"

#include "sc/alg/series.h"
#include "sc/utils.h"
#include "sc/vect.h"

using namespace dtp;
using namespace base;

//#undef min
//#undef max

// calculate correlation between two series
// lenght must be > 1
// r(x,y) = (sum(xi * yi) - n * avg(x) * avg(y))/((n-1)*stdDev(x)*stdDev(y)) 
// returns value between <-1,1>
double calcCorrel(const scVectorOfDouble &input1, const scVectorOfDouble &input2)
{
  double res = -1.0;

  if (input1.size() > 1) {
    double outSum = 0.0;
    for(uint i = 0, epos = input1.size(); i < epos; i++) {
      outSum += (input1[i]*input2[i]);
    }

    double denomin = 
       double(input1.size() - 1) * 
       std_dev(input1.begin(), input1.end(), 0.0) * 
       std_dev(input2.begin(), input2.end(), 0.0);

    double nomin = 
       outSum - 
       (input1.size() * 
        avg(input1.begin(), input1.end(), 0.0) * 
        avg(input2.begin(), input2.end(), 0.0)
       );

    if (!equDouble(denomin, 0.0))
      res = nomin / denomin;
  }  
  
  return  res;
}

uint countExtremeValuesWithNoiseFilter(const scVectorOfDouble &input, uint filterStep)
{
  scVectorOfDouble filteredValues;
  calcMaVector(input, filterStep, filteredValues);
  return countExtremeValues(filteredValues);
}

uint countExtremeValues(const scVectorOfDouble &input)
{
  return countExtremeValues(input, 0, input.size());
}

uint countExtremeValues(const scVectorOfDouble &input, int beginPos, int endPos)
{
  uint res = 0;
  double lastVal1, lastVal2;
  double newVal;

  if (endPos - beginPos > 2) {
    lastVal1 = input[beginPos];
    lastVal2 = input[beginPos + 1];

    for(uint i = beginPos + 2, epos = endPos; i != epos; i++) 
    {
      newVal = input[i];
      if (
           (
             (lastVal1 < lastVal2)
             &&
             (lastVal2 > newVal) 
           )
           ||
           (
             (lastVal1 > lastVal2)
             &&
             (lastVal2 < newVal) 
           )
         )  
      {
        res++; 
      } 

      lastVal1 = lastVal2;
      lastVal2 = newVal;
    }
  }
  
  return res;  
}

uint countIncreases(const scVectorOfDouble &input, int beginPos, int endPos)
{
  uint res = 0;
  double lastVal1, newVal;

  if (endPos - beginPos > 1) {
    lastVal1 = input[beginPos];

    for(uint i = beginPos + 1, epos = endPos; i != epos; i++) 
    {
      newVal = input[i];
      if(lastVal1 < newVal)
        res++; 
      lastVal1 = newVal;
    }
  }
  
  return res;  
}

double sumIncreases(const scVectorOfDouble &input, int beginPos, int endPos)
{
  double res = 0.0;
  double lastVal1, newVal;

  if (endPos - beginPos > 1) {
    lastVal1 = input[beginPos];

    for(uint i = beginPos + 1, epos = endPos; i != epos; i++) 
    {
      newVal = input[i];
      if(lastVal1 < newVal)
        res += (newVal - lastVal1); 
      lastVal1 = newVal;
    }
  }
  
  return res;  
}

double sumExtremeDiffs(const scVectorOfDouble &input, int beginPos, int endPos)
{
  double res = 0.0;
  double lastVal1, lastVal2, newVal;
  double lastExtremeValue;

  if (endPos - beginPos > 2) {
    lastVal1 = input[beginPos];
    lastVal2 = input[beginPos + 1];
    lastExtremeValue = lastVal1;

    for(uint i = beginPos + 2, epos = endPos; i != epos; i++) 
    {
      newVal = input[i];
      if (
           (
             (lastVal1 < lastVal2)
             &&
             (lastVal2 > newVal) 
           )
           ||
           (
             (lastVal1 > lastVal2)
             &&
             (lastVal2 < newVal) 
           )
         )  
      {
      // extreme value found
        res += fabs(lastExtremeValue - lastVal2);
        lastExtremeValue = lastVal2;
      } 
      lastVal1 = lastVal2;
      lastVal2 = newVal;
    }
  }
  
  return res;  
}

// calc nth derivate df(x) = dy/dx
// tested up to 2nd level
void calcNthDerives(const scVectorOfDouble &xVector, const scVectorOfDouble &yVector, uint level, scVectorOfDouble &output)
{
  output.clear();
  assert(level > 0);
  assert(xVector.size() == yVector.size());

  scVectorOfDouble dyNth(xVector.size());
  scVectorOfDouble dxNth(xVector.size());

  output.resize(xVector.size() - level);
  
  for(uint j = 0, eposj = xVector.size(); j != eposj; j++) {
    dxNth[j] = xVector[j];
    dyNth[j] = yVector[j];
  }  
  
  for(uint dLevel=1; dLevel <= level; dLevel++) {
    for(uint j = 0, eposj = xVector.size() - dLevel; j != eposj; j++) {
      dyNth[j] = dyNth[j+1] - dyNth[j];
      if ((dLevel % 2) == 0)        
        dxNth[j] = (dxNth[j+1] + dxNth[j])/dLevel;
      else  
        dxNth[j] = (dxNth[j+1] - dxNth[j])/dLevel;
      if (!equDouble(dxNth[j], 0.0))
        dyNth[j] = dyNth[j] / dxNth[j];
      else  
        dyNth[j] = 0.0;
    }
  }    

  for(uint j = 0, eposj = output.size(); j != eposj; j++) 
    output[j] = dyNth[j];
}

// calc output = input1 - input2
void calcVectorDiff(const scVectorOfDouble &input1, const scVectorOfDouble &input2, scVectorOfDouble &output)
{
  assert(input1.size() == input2.size());
  output.clear();
  output.resize(input1.size());
  for(uint i=0,epos=input1.size(); i!=epos; i++)
    output[i] = input1[i] - input2[i];
}

double calcStdErrorDeriveN(const scVectorOfDouble &xVect, const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect, uint level)
{
  scVectorOfDouble derivedY;
  calcNthDerives(xVect, yVect, level, derivedY);
  return calcStdErrorDeriveNPrepared(xVect, derivedY, fxVect, level);
}

double calcStdErrorDeriveNPrepared(const scVectorOfDouble &xVect, const scVectorOfDouble &yVectDerive, const scVectorOfDouble &fxVect, uint level)
{
  scVectorOfDouble derivedFx, diffs;
  calcNthDerives(xVect, fxVect, level, derivedFx);
  calcVectorDiff(derivedFx, yVectDerive, diffs);
  return std_dev(diffs.begin(), diffs.end(), 0.0);
}

double calcFadingMaDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect)
{
  double outSum = 0.0;
  const double fadingFactor = 0.8;
  double r = fadingFactor;
  // from MA(x,i,size - 1) to MA(x,i,2) 
  for(uint i = yVect.size()-1, epos = 0; i > epos; i = i / 2) {
    outSum += (calcMaDiff(yVect, fxVect, i)*r);
    r *= fadingFactor;
  }
  return outSum;
}

double calcMaDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect, uint blockSize)
{
  double outSum = 0.0;
  double partDiff;
  for(uint i = 0, epos = yVect.size() - blockSize + 1; i < epos; i++) {
    partDiff = 1.0 + fabs(calcMa(yVect, blockSize, i) - calcMa(fxVect, blockSize, i));    
    outSum += (partDiff * partDiff);
  }
  return outSum;
}

double calcMa(const scVectorOfDouble &input, uint blockSize, uint offset)
{
  double outSum = 0.0;

  if (!input.empty()) {
    uint epos = std::min<uint>(input.size(), offset + blockSize);
    for(uint i = offset; i != epos; i++) {
      outSum += input[i];
    }
    if (epos != offset)
      outSum = outSum / double(epos - offset);
  }  
  return outSum;  
}

void calcMaVector(const scVectorOfDouble &input, uint blockSize, scVectorOfDouble &output)
{
  uint targetSize;
  if (input.size() >= blockSize)
    targetSize = input.size() - blockSize + 1;
  else
    targetSize = 0;  

  output.resize(targetSize);
  
  for(uint i = 0, epos = output.size(); i < epos; i++) {
    output[i] = calcMa(input, blockSize, i);    
  }
}

// calculate frequence vector for objective
void calcFreqVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output)
{
  output.resize(valueVect.size());
  int minIdx, maxIdx, gap;
  int extremeCount;
  double freqTotal, freqGap;
  double step;
  for(int i=0, epos = valueVect.size(); i != epos; i++)
  {
    gap = 1;
    step = 1.0;
    freqTotal = 0.0;
    
    while(((i + gap) < epos) || (i - gap) >= 0)
    {
      minIdx = std::max(i - gap, 0);
      maxIdx = std::min(i + gap, epos - 1);
      extremeCount = countExtremeValues(valueVect, minIdx, maxIdx + 1);
      freqGap = static_cast<double>(extremeCount)*(1.0/pow(2.0, step));
      freqTotal += freqGap;
      gap *= 2;
      step += 1.0; 
    }
    output[i] = freqTotal;
  }
}

// calculate frequence objective
double calcFreqDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect)
{
  scVectorOfDouble freqForY;  
  calcFreqVector(yVect, freqForY);  
  return calcFreqDiffPrepared(freqForY, fxVect);
}

double calcFreqDiffPrepared(const scVectorOfDouble &yVectFreq, const scVectorOfDouble &fxVect)
{
  double res = 0.0;
  scVectorOfDouble freqForFx;
  
  calcFreqVector(fxVect, freqForFx);
  
  for(uint i=0, epos = yVectFreq.size(); i != epos; i++)
  {
    res += relativeErrorMinMax(yVectFreq[i], freqForFx[i]);  
  }
  return res;
}

// calculate amplitude vector for objective
void calcAmplitudeVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output)
{
  output.resize(valueVect.size());
  int minIdx, maxIdx, gap;
  int extremeCount;
  double freqTotal, freqGap;
  double step;
  for(int i=0, epos = valueVect.size(); i != epos; i++)
  {
    gap = 1;
    step = 1.0;
    freqTotal = 0.0;
    
    while(((i + gap) < epos) || (i - gap) >= 0)
    {
      minIdx = std::max(i - gap, 0);
      maxIdx = std::min(i + gap, epos - 1);
      extremeCount = static_cast<int>(sumExtremeDiffs(valueVect, minIdx, maxIdx + 1));
      freqGap = static_cast<double>(extremeCount)*(1.0/pow(2.0, step));
      freqTotal += freqGap;
      gap *= 2;
      step += 1.0; 
    }
    output[i] = freqTotal;
  }
}

// calculate frequence objective
double calcAmplitudeDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect)
{
  scVectorOfDouble ampliForY;
  calcAmplitudeVector(yVect, ampliForY);
  return calcAmplitudeDiffPrepared(ampliForY, fxVect);
}

double calcAmplitudeDiffPrepared(const scVectorOfDouble &yVectAmpli, const scVectorOfDouble &fxVect)
{
  double res = 0.0;
  scVectorOfDouble ampliForFx;
  
  calcAmplitudeVector(fxVect, ampliForFx);
  
  for(uint i=0, epos = yVectAmpli.size(); i != epos; i++)
  {
    res += relativeErrorMinMax(yVectAmpli[i], ampliForFx[i]);  
  }
  return res;
}

void calcIncreasesVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output)
{
  output.resize(valueVect.size());
  int minIdx, maxIdx, gap;
  double valTotal, valGap;
  double step, incSum;
  for(int i=0, epos = valueVect.size(); i != epos; i++)
  {
    gap = 1;
    step = 1.0;
    valTotal = 0.0;
    
    while(((i + gap) < epos) || (i - gap) >= 0)
    {
      minIdx = std::max(i - gap, 0);
      maxIdx = std::min(i + gap, epos - 1);
      incSum = sumIncreases(valueVect, minIdx, maxIdx + 1);
      valGap = incSum*(1.0/pow(2.0, step));      
      valTotal += valGap;
      gap *= 2;
      step += 1.0; 
    }
    output[i] = valTotal;
  }
}

// calculate increases objective
double calcIncreasesDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect)
{
  double res = 0.0;
  scVectorOfDouble vectForY;
  scVectorOfDouble vectForFx;
  
  calcIncreasesVector(yVect, vectForY);
  calcIncreasesVector(fxVect, vectForFx);
  
  for(uint i=0, epos = vectForY.size(); i != epos; i++)
  {
    res += relativeErrorMinMax(vectForY[i], vectForFx[i]);  
  }
  return res;
}

uint calcDistinctCount(const scVectorOfDouble &input)
{
  std::set<double> valueSet;
  for(uint i = 0; i < input.size(); i++) {
    valueSet.insert(input[i]);
  }
  return (valueSet.size());
}
