/////////////////////////////////////////////////////////////////////////////
// Name:        series.h
// Project:     scLib
// Purpose:     Signal processing functions
// Author:      Piotr Likus
// Modified by:
// Created:     04/06/2011
/////////////////////////////////////////////////////////////////////////////

#ifndef _SCSERIES_H__
#define _SCSERIES_H__

// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------
/** \file series.h
\brief Short file description

Long description
*/

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
typedef std::vector<double> scVectorOfDouble;

// ----------------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------------
// calculate number of distinct values
uint calcDistinctCount(const scVectorOfDouble &input);

// calculate correlation between two series
// lenght must be > 1
// r(x,y) = (sum(xi * yi) - n * avg(x) * avg(y))/((n-1)*stdDev(x)*stdDev(y)) 
// returns value between <-1,1>
double calcCorrel(const scVectorOfDouble &input1, const scVectorOfDouble &input2);

// calculate number of minimas & maximas
uint countExtremeValues(const scVectorOfDouble &input);
uint countExtremeValues(const scVectorOfDouble &input, int beginPos, int endPos);
double sumExtremeDiffs(const scVectorOfDouble &input, int beginPos, int endPos);
uint countIncreases(const scVectorOfDouble &input, int beginPos, int endPos);
double sumIncreases(const scVectorOfDouble &input, int beginPos, int endPos);
  
// count extreme values with noise filter
uint countExtremeValuesWithNoiseFilter(const scVectorOfDouble &input, uint filterStep);
  
// calc nth derivate df(x) = dy/dx
// tested up to 2nd level
void calcNthDerives(const scVectorOfDouble &xVector, const scVectorOfDouble &yVector, uint level, scVectorOfDouble &output);

// calc output = input1 - input2
void calcVectorDiff(const scVectorOfDouble &input1, const scVectorOfDouble &input2, scVectorOfDouble &output);

// calculate StdDev(vector_diff(nth-derive(yVect), nth-derive(fxVect)))
// metric used to calculate difference between functions in nth derive space
double calcStdErrorDeriveN(const scVectorOfDouble &xVect, const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect, uint level);
double calcStdErrorDeriveNPrepared(const scVectorOfDouble &xVect, const scVectorOfDouble &yVectDerive, const scVectorOfDouble &fxVect, uint level);

// calculate fading moving average difference
double calcFadingMaDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect);
// calculate moving average difference
double calcMaDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect, uint blockSize);
// calculate moving average for k samples starting from offset 
double calcMa(const scVectorOfDouble &input, uint blockSize, uint offset);
// calculate MA vector
void calcMaVector(const scVectorOfDouble &input, uint blockSize, scVectorOfDouble &output);

// calculate frequence vector for objective
void calcFreqVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output);
// calculate frequence objective
double calcFreqDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect);
double calcFreqDiffPrepared(const scVectorOfDouble &yVectFreq, const scVectorOfDouble &fxVect);

// calculate amplitude vector for objective
void calcAmplitudeVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output);
// calculate frequence objective
double calcAmplitudeDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect);
double calcAmplitudeDiffPrepared(const scVectorOfDouble &yVectAmpli, const scVectorOfDouble &fxVect);
  
void calcIncreasesVector(const scVectorOfDouble &valueVect, scVectorOfDouble &output);
double calcIncreasesDiff(const scVectorOfDouble &yVect, const scVectorOfDouble &fxVect);  

#endif // _SCSERIES_H__
