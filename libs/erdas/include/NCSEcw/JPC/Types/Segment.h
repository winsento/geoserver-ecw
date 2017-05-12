/********************************************************
** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
**
** FILE:     Segment.h 
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPC Segment class
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCSEGMENT_H
#define NCSJPCSEGMENT_H

#ifndef NCSJPCTYPES_H
#include "NCSEcw/JPC/Types/Types.h"
#endif // NCSJPCTYPES_H

namespace NCS {
namespace JPC {

	// Segment class
class NCSECW_IMPEX CSegment {
public:
		/** Segment length */
	UINT32	m_nLength;
		/** Segment Index */
	UINT16	m_nIndex;
		/** Number of passes in this segment */
	UINT8	m_nPasses;
		/** Segment data */
	UINT8	*m_pData;
	
		/** Default constructor */
	CSegment();
		/** Copy constructor */
	CSegment(const CSegment &s);
		/** Virtual destructor */
	virtual ~CSegment();
};
}
}

#endif // NCSJPCSEGMENT_H
