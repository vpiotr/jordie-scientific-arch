/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Project:     scLib
// Purpose:     Utility (general-purpose) functions
// Author:      Piotr Likus
// Modified by:
// Created:     04/10/2008
/////////////////////////////////////////////////////////////////////////////

#define REGEXP_XPRESSIVE
//#define REGEXP_GRETA
//#define REGEXP_BOOST

//std
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sys/timeb.h>
#include <cmath>

bool equFloat(float val1, float val2)
{
  return (std::fabs(val1 - val2) < std::numeric_limits<float>::epsilon());
}

bool equDouble(double val1, double val2)
{
  return (std::fabs(val1 - val2) < std::numeric_limits<double>::epsilon());
}

bool equXDouble(xdouble val1, xdouble val2)
{
  return (std::fabs(val1 - val2) < std::numeric_limits<long double>::epsilon());
}

scString fillTemplateValues(const scString &tpl, const scDataNode &values)
{
  scString res;
  fillTemplateValues(tpl, values, res);
  return res;
}

void fillTemplateValues(const scString &tpl, const scDataNode &values, scString &output)
{
  scDataNode element;
  output = tpl;
  for(int i=0,epos=values.size();i!=epos;i++)
  {
    values.getElement(i, element);
    strReplaceThis(output, "{"+values.getElementName(i)+"}", element.getAsString(), true);
  }
}

#ifdef SC_USE_WX

// Copy a fixed size of data from an input stream to an
// output stream, using a buffer to speed it up.
void wxBufferedCopy(wxInputStream& inStream, wxOutputStream& outStream,
size_t size)
{
unsigned char buf[1024];
size_t bytesLeft = size;
while (bytesLeft > 0)
{
size_t bytesToRead = SC_MIN((size_t) sizeof(buf), bytesLeft);
inStream.Read((void*) buf, bytesToRead);
outStream.Write((void*) buf, bytesToRead);
bytesLeft -= bytesToRead;
}
}

void saveBufferToFile(void *ptr, size_t dataSize, const scString &fname)
{
    wxMemoryInputStream mis(ptr, dataSize);

    wxFileOutputStream file_output( fname );
    wxBufferedCopy(mis, file_output, dataSize);

    file_output.Sync();
}
#else
void stdBufferedCopy(std::istream& inStream, std::ostream& outStream, size_t size)
{
  char buf[1024];
  size_t bytesLeft = size;
  size_t bytesRead;
  while (!inStream.eof() && (bytesLeft > 0))
  {
    size_t bytesToRead = SC_MIN((size_t) sizeof(buf), bytesLeft);
    bytesRead = inStream.readsome(buf, bytesToRead);
    if (bytesRead > 0)
      outStream.write(buf, bytesRead);
    bytesLeft -= bytesRead;
  }
}

void saveBufferToFile(void *ptr, size_t dataSize, const scString &fname)
{
    std::ofstream outfile;
    outfile.open(fname.c_str());

    outfile.write(static_cast<char *>(ptr), dataSize);

    outfile.close();
}
#endif


void readTextFileToString_v2(const scString &fname, scString &output)
{
  std::ifstream infile(stringToStdString(fname).c_str());
  // allocare enough memory in string
  infile.seekg(0, std::ios::end);
  output.reserve(infile.tellg());
  infile.seekg(0, std::ios::beg);

  // return result
  output.assign((std::istreambuf_iterator<char>(infile)),
              std::istreambuf_iterator<char>());
}

// faster than v2
void readTextFileToString(const scString &fname, scString &output)
{
  std::ifstream infile(stringToStdString(fname).c_str());

  infile.seekg(0, std::ios::end);
  output.resize(infile.tellg());
  infile.seekg(0, std::ios::beg);
  infile.read(&output[0], output.size());
  infile.close();
}

double centerDistribFormula(double x, double a)
{
  // rescale x from 0..1 to -1..+1
  double useX = 2.0 * x - 1.0;
  // rescale a from 0..1 to 0,1..3,0
  double useA = 0.1 + (3.0 - 0.1) * a;

  double res = 1.0 / (1.0 + pow(10.0, -useA)*exp(-useX*(1.0/useA)));
  return res;
}

uint getNextGuidChar(uint seed)
{
  // magic number & generation based on Boost UUID header
  uint res = seed ^ (0x9e3779b9 + (seed << 6) + (seed >> 2));
  return res;
}

scString genGuid(uint initValue)
{
  const uint GUID_SIZE = 32;
  const char *digits = "0123456789abcdef";
  const uint alphabetLen = 16;
  uint seed = initValue;
  uint newCharIdx;
  scString res;
  res.resize(GUID_SIZE);
  for(uint i=0; i != GUID_SIZE; i++)
  {
    seed = getNextGuidChar(seed);
    newCharIdx = seed % alphabetLen;
    res[i] = digits[newCharIdx];
  }
  return res;
}

// calculate min((x - y) mod n, (y - x) mod n)
// used to torus-like or sphere-like distance calculation
int tourus_distance(int pos1, int pos2, int fieldWidth)
{
  int dist1 = ((pos1 + fieldWidth) - pos2) % fieldWidth;
  int dist2 = ((pos2 + fieldWidth) - pos1) % fieldWidth;
  if (dist1 < dist2)
    return dist1;
  else
    return dist2;
}

