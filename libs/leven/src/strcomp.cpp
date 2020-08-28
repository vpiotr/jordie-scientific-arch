/////////////////////////////////////////////////////////////////////////////
// Name:        strcomp.cpp
// Project:     dtpLib
// Purpose:     Functions for comparing strings.
// Author:      Piotr Likus
// Modified by:
// Created:     25/04/2012
/////////////////////////////////////////////////////////////////////////////

#include "base/strcomp.h"
#include "base/details/butils.h"

//Levenshtein distance
unsigned int strDiffLev(const dtpString &s1, const dtpString &s2)
{
  typedef std::vector<std::vector<unsigned int> > vectorUInt2;

  const size_t len1 = s1.size(), len2 = s2.size();
  vectorUInt2 d(len1 + 1, std::vector<unsigned int>(len2 + 1));

  for(uint i = 1; i <= len1; ++i) d[i][0] = i;
  for(uint i = 1; i <= len2; ++i) d[0][i] = i;

  for(uint i = 1; i <= len1; ++i)
	  for(uint j = 1; j <= len2; ++j)

                d[i][j] = BASE_MIN( BASE_MIN(d[i - 1][j] + 1,d[i][j - 1] + 1),
                                    d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
  return d[len1][len2];
}


strDiffFunctor::strDiffFunctor(size_t maxStrLen): m_maxStrLength(0)
{
  prepareForSize(maxStrLen);
}

void strDiffFunctor::prepareForSize(size_t maxStrLen)
{
  if (m_maxStrLength < maxStrLen)
  {
    if (m_maxStrLength > 0)
      m_maxStrLength = (maxStrLen * 15 / 10); // prepare for more
    else
      m_maxStrLength = maxStrLen;
    m_workMatrix.reset(new vectorUInt2(maxStrLen + 1, std::vector<unsigned int>(maxStrLen + 1)));
  }
}

uint strDiffFunctor::calc(const dtpString &s1, const dtpString &s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();

        prepareForSize(BASE_MAX(len1, len2));

	vectorUInt2 *d = m_workMatrix.get();

	for(size_t i = 1; i <= len1; ++i) (*d)[i][0] = uint(i);
	for(size_t i = 1; i <= len2; ++i) (*d)[0][i] = uint(i);

	for(size_t i = 1; i <= len1; ++i)
		for(size_t j = 1; j <= len2; ++j)
       (*d)[i][j] = BASE_MIN( BASE_MIN((*d)[i - 1][j] + 1, (*d)[i][j - 1] + 1),
                                       (*d)[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
	return (*d)[len1][len2];
}

