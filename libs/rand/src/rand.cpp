/////////////////////////////////////////////////////////////////////////////
// Name:        rand.cpp
// Project:     scLib
// Purpose:     Random number handling
// Author:      Piotr Likus
// Modified by:
// Created:     13/02/2008
/////////////////////////////////////////////////////////////////////////////


// comment-out to disable auto-initialization of random numbers
//#define RAND_AUTO_INIT

//#include "sc/defs.h"

// this module is not thread-safe, so:
#pragma warning(disable:4640) // construction of local static object is not thread-safe

// std
#include <ctime>

// boost
#include "boost/random.hpp"
#include "boost/random/uniform_real.hpp"

// dtp
#include "base/date.h"

//sc
#include "base/rand.h"
#include "base/bmath.h"
//#include "sc/utils.h"

#ifdef DEBUG_MEM
#include "sc/dbg/DebugMem.h"
#endif

#ifdef RAND_AUTO_INIT
static bool initRandomDouble = true;
static bool initRandomXDouble = true;
#else
static bool initRandomDouble = false;
static bool initRandomXDouble = false;
#endif

using namespace dtp;

uint64 randomTimeSeed() 
{
  return currentTimeInSecs()+static_cast<uint64>(clock());
}

void randomize()
{
#pragma omp critical(randomize)
{    
  initRandomDouble = initRandomXDouble = true;
}  
}

double randomDouble(double a_min, double a_max)
{
  double nextVal;
#pragma omp critical(random_double)
{    
  static boost::mt19937 backend;
  typedef boost::uniform_real<double> uni_distrib_double;
  static uni_distrib_double distribution(0,1);
  static boost::variate_generator< boost::mt19937 &, uni_distrib_double > frontend(backend, distribution);
  
  if (initRandomDouble)
  {
    backend.seed(static_cast<unsigned> (randomTimeSeed()));
    initRandomDouble = false;
  }  

  nextVal = (frontend()*(a_max-a_min))+a_min;
}  
  return nextVal;
}

xdouble randomXDouble(xdouble a_min, xdouble a_max)
{
  xdouble nextVal;
#pragma omp critical(random_xdouble)
{    
  static boost::mt19937 backend;
  typedef boost::uniform_real<long double> uni_distrib_xdouble;
  static uni_distrib_xdouble distribution(0,1);
  static boost::variate_generator< boost::mt19937 &, uni_distrib_xdouble > frontend(backend, distribution);
  
  if (initRandomXDouble)
  {
    backend.seed(static_cast<unsigned> (randomTimeSeed()));
    initRandomXDouble = false;
  }  

  nextVal = (frontend()*(a_max-a_min))+a_min;
}  
  return nextVal;
}

int randomInt(int a_min, int a_max)
{
  double nextVald = (randomDouble(0.0, 1.0)*(a_max-a_min+1))+a_min;
  
  int nextVal = round<int>(nextVald-0.5);
  if (nextVal < a_min)
    nextVal = a_min;
  else if (nextVal > a_max)
    nextVal = a_max;  
  return nextVal;
}

uint randomUInt(uint a_min, uint a_max)
{
  double nextVald = (randomDouble(0.0, 1.0)*(a_max-a_min+1))+a_min;
  
  uint nextVal = round<uint>(nextVald-0.5);
  if (nextVal < a_min)
    nextVal = a_min;
  else if (nextVal > a_max)
    nextVal = a_max;  
  return nextVal;
}

bool randomBool()
{
  return (randomDouble(0.0, 1.0) < 0.5);
}

bool randomFlip(double aProb)
{
  return (randomDouble(0.0, 1.0) < aProb);
}

void randomString(const dtpString &alphabet, uint a_size, dtpString &output)
{
  dtpString outValue;
  int maxValue = alphabet.length() - 1;
  int charPos; 
  
  if (alphabet.empty()) {
  // random characters 
    char code;
    for(uint i=0; i < a_size; i++) {
      code = char(randomInt(32, 255));    
      outValue += code;
    }    
  } else { 
  // characters from alphabet
    for(uint i=0; i < a_size; i++) {
      charPos = randomInt(0, maxValue);    
      outValue += alphabet[charPos];
    }    
  }
  output = outValue;
}
