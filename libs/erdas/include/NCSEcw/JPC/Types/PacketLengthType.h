/********************************************************
** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
**
** FILE:     PacketLengthType.h 
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CPacketLengthType class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPACKETLENGTHTYPE_H
#define NCSJPCPACKETLENGTHTYPE_H

#include "NCSEcw/JPC/Markers/Marker.h"

	/**
	 * CPacketLengthType class - the JPC Packet Length Type.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: #3 $ $Author: ywang $ $Date: 2015/01/19 $ 
	 */	
class NCSECW_IMPEX CPacketLengthType {
public:
		/** Packet length - Header usually includes Data, except when using PPM or PPT markers
		  *
		  * m_nHeaderLength was changed to a UINT64 instead of UINT32 as it allows
		  * CNCSJPCPacketList::GenDynamicPLTs() to use "CumulativeLengths" which greatly
		  * increases the performance of CNCSJPCTilePartHeader::GetPacketHeader() and hence
		  * the JP2 index file generation for some JP2 files.
		  */
	UINT64	m_nHeaderLength;
		/** Packet data length - Only when using PPM or PPT markers */
	UINT32	m_nDataLength;

		/** Default constructor, initialises members */
	CPacketLengthType();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      bool		true on success.
		 */
	NCS_INLINE bool Parse(NCS::CIOStream &Stream) {
			UINT32 nTmp = 0;
			bool bRet = false;
			UINT8 t8;

			while((bRet = Stream.ReadMSB(t8)) != FALSE) {
				nTmp = (nTmp << 7) | (t8 & 0x7f);
				if((t8 & 0x80) == 0) {
					break;
				}
			}
			m_nHeaderLength = nTmp;
			return(bRet);
		};
		/** 
		 * Parse the fields from a chunk of memory.
		 * @param		UINT8*		Memory buffer to parse length from.
		 * @param		&i			Index into memory buffer, incremented before return
		 * @return      bool		true on success.
		 */
	NCS_INLINE bool Parse(UINT8 *pBuf, INT32 &i) {
			UINT32 nTmp = 0;
			for(; ;) {
				UINT8 t8 = pBuf[i++];
				nTmp = (nTmp << 7) | (t8 & 0x7f);
				if((t8 & 0x80) == 0) {
					break;
				}
			}
			m_nHeaderLength = nTmp;
			return(true);
		};

		/** 
		 * UnParse the fields to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      bool		true on success.
		 */
	NCS_INLINE bool UnParse(NCS::CIOStream &Stream) {
			UINT32 nMask = 0xffffffff;
			INT8 nShift = 0;
			bool bRet = true;

			while(nMask & m_nHeaderLength) {
				nMask <<= 7;
				nShift++;
			}
			if(nShift > 0) {
				nShift--;
				nMask = 0x7f << (nShift * 7);
				while(nShift >= 0 && bRet) {
					bRet &= Stream.WriteMSB((UINT8)(((m_nHeaderLength & nMask) >> (nShift * 7)) | ((nShift > 0) ? 0x80 : 0x00)));
					nShift--;
					nMask >>= 7;
				}
			} else {
				// Zero length
				bRet &= Stream.WriteMSB((UINT8)0x00);
			}
			return(bRet);

		};
};


#endif // !NCSJPCPACKETLENGTHTYPE_H
