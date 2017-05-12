/********************************************************
** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
**
** FILE:     Precinct.h 
** CREATED:  11/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CPrecinct class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPRECINCT_H
#define NCSJPCPRECINCT_H

#ifndef NCSJPCSUBBAND_H
#include "NCSEcw/JPC/Nodes/SubBand.h"
#endif // NCSJPCSUBBAND_H
#ifndef NCSJPCNode2D_H
#include "NCSEcw/SDK/Node2D.h"
#endif // NCSJPCNode2D_H
#include "NCSEcw/JPC/Progression.h"
//#include "NCSEcw/JPC/Types/ProgressionOrderType.h"
#include "NCSEcw/JPC/Markers/POCMarker.h"
#include "NCSPacketIOStream.h"
#include "NCSThreadPool.h"

#include "NCSEcw/JPC/Packet.h" // Against bug EMS-407, added 25/10/2011

#include "NCSEcw/JPC/Nodes/CodeBlock.h" // for definition of NCSJPC_DECODE_CODEBLOCK_ONCE

#define NCSJPC_KEEP_SMALL_PRECINCT_DECODE // used when m_bSmallPrecinct = true and decode whole subband

// define it to force decode whole region of a large precinct, but it is slower. Also calculating large amount of m_RectMap may crash, eg, zooming into pixel of Burrup_LL-GDA94.jp2
//#define FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT

#ifdef DEBUG_MT_SUBBAND_DECODING
// debug for subband multithreading decoding
static INT16 CReadJobCount = 0;
static INT16 CReadJobDoing = 0;
static INT16 CReadJobDone = 0;
#endif

namespace NCS {

namespace SDK {
	class CSpinLockImpl;
	class CSpinRWLockImpl;
}

namespace JPC {
	/**
	 * CPrecinct class - the JPC codestream Precinct.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: #1 $ $Author: ctapley $ $Date: 2014/10/17 $ 
	 */	
class NCSECW_IMPEX CPrecinct: public SDK::CNode2D {
public:
		/** Resolution level for this precinct */
	class CResolution *m_pResolution;
		/** Precinct number within this resolution */
	UINT32	m_nPrecinct;

		/** SubBands in this precinct */
	CSubBand *m_SubBands[4];

		/** Packet number for each layer in this precinct */
	std::vector<UINT32> m_Packets;

		/** Packets for each layer in this precinct */
	std::vector<CPacket*> m_pPackets; // Against bug EMS-407, added 25/10/2011

	UINT16	m_nProgressionLayer;
		/** Precinct has zero width or height */
	bool	m_bZeroSize;		
		/** Flag for header read status */
	INT8	m_nHeaderReadDone; // added on 18/03/2012, changed on 26/02/2013
		/** The first packet number in its own tile */
	UINT32  m_nFirstPacketNrInOwnTile; // added on 29/06/2012

		/** temp stored stream for each packet in this precinct. This is used for ECWP3. */
	std::vector<CMemoryIOStream*> m_pStreams; // added 12/02/2013
		/** the smallest stored layer. This is used with m_pStreams. */
	UINT16 m_nSmallestStoredLayerNr;	// added 12/02/2013
		/** the smallest parsed layer. This is used with m_pStreams. */
	UINT16 m_nSmallestParsedLayerNr;	// added 12/02/2013

	CRect2D mInvalidRect;

		/** 
	     * Constructor 
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 */
	CPrecinct(class CResolution *pResolution, UINT32 nPrecinct, bool bCreateSubBands = false);
		/** Virtual destructor */
	virtual ~CPrecinct();

		/** 
		 * Get X0 of this rect.
		 * @return      INT32		Coordinate value.
		 */
	//virtual NCS_INLINE INT32 GetX0() { return(CRect2D::GetX0()); };
		/** 
		 * Get Y0 of this rect.
		 * @return      INT32		Coordinate value.
		 */
	//virtual NCS_INLINE INT32 GetY0() { return(CRect2D::GetY0()); };
		/** 
		 * Get X1 of this rect.
		 * @return      INT32		Coordinate value.
		 */
	//virtual NCS_INLINE INT32 GetX1() { return(CRect2D::GetX1()); };
		/** 
		 * Get Y1 of this rect.
		 * @return      INT32		Coordinate value.
		 */
	//virtual NCS_INLINE INT32 GetY1() { return(CRect2D::GetY1()); };

		/** 
		 * Get X0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 sGetX0(CResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get X0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 sGetX0(CResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get Y0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 sGetY0(CResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Y0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 sGetY0(CResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get X1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 sGetX1(CResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get X1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 sGetX1(CResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get Y1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 sGetY1(CResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Y1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 sGetY1(CResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get Width of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Width.
		 */
//	static INT32 GetWidth(CResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Height of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
//	static INT32 GetHeight(CResolution *pResolution, INT32 nPrecinct);

		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		read context
		 * @param		pDst		Destination buffer.
		 * @param		iBand		Output band
		 * @return      bool		true on succes, else false.
		 */
	virtual bool Read(ContextID nCtx, CBuffer2D *pDst, UINT16 iBand);

	virtual CNode2D::Context *NewContext() {
		return((CNode2D::Context*)new CPrecinct::Context());
	};

	void AddRef();
	void UnRef();
	UINT32 NrRefs();
	static bool HaveZeroRefs();
	static UINT32	ZeroRefs(){ return sm_nZeroRefs;}

		/**
		 * Create the subbands
		 */
	virtual bool CreateSubBands(ContextID nCtx, bool bRead, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType, CRect2D& rectDst);
	/**
		 * Destroy the subbands
		 */
	virtual bool DestroySubBands();

	virtual void Request(CIOStream *pStream);
	virtual void Cancel(CIOStream *pStream);

	void SetContextQualityLevel(ContextID nCtx, UINT32 nLevel);
	virtual bool UnLink(ContextID nCtx, UINT16 nInputs);

	UINT64 GetMemorySize(); // Against bug EMS-407, added 01/11/2011
	bool CreatePackets();	// Against bug EMS-407, added 02/11/2011

	bool CalcFirstPacketNr();	// added on 13/07/2012

	// added on 21/02/2031
	std::vector<UINT32>* GetPacketIDs();

	// added on 25/02/2031
	void FreeCodeBlocks(bool bLock);

	void ReceivedBlock(NCSBlockId &nBlock, CMemoryIOStream &Stream, CProgression &p, CPacket *pHeader);
	void ReceivedBlock(CMemoryIOStream &Stream, CProgression &p, CPacket *pHeader);

	void SetSubbandCreationState(UINT8 nCodeBlockRead); // added on 24/04/2014 for lock
	UINT8 GetSubbandCreationState(); // added on 24/04/2014 for lock
	bool SetSubbandDecodeState(UINT8 nCodeBlockRead); // added on 24/04/2014 for lock
	UINT8 GetSubbandDecodeState(); // added on 24/04/2014 for lock

#ifdef FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT
	// added on 29/04/2014
	bool SetReadRect(ContextID nCtx, CRect2D& Rect);
	CRect2D& GetReadRect(ContextID nCtx);
#endif // FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT

	UINT32 m_nDecodedToLayer;
	friend class CSubBand;

	CPrecinct *m_pNext;
	CPrecinct *m_pPrev;
protected:

	class NCSECW_IMPEX Context : public NCS::SDK::CNode2D::Context
	{
	public:
		UINT32 m_nRequestedQualityLevel;

		Context();

#ifdef FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT
		// added on 29/04/2014
		void SetReadRect(CRect2D& Rect){
			m_Rect = Rect;
		};
		CRect2D& GetReadRect() {
			return m_Rect;
		}

	private:
		// so rect represents the required reading region for the precinct (x, y) in this view
		CRect2D m_Rect;
#endif // FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT
	};

	static UINT32				sm_nZeroRefs;
	
		/** Packet Read error is stored here */
	CError	m_Error;
		/** Reference count */
	UINT32		m_nRefs;
		/**
		 * Read the packets for this precinct
		 */	
	virtual bool ReadPackets(ContextID nCtx);	

	UINT8 m_nSubBandsCreation; // changed on 03/09/2012, changed on 21/04/2014 from m_nSubBandsRead
	UINT8 m_nSubBandsDecode; // added on 21/04/2014 to decode code blocks only when needed
	bool m_bSmallPrecinct;// added on 01/05/2014 to determine decoding by using single thread or multi threads

	NCS::SDK::CSpinLockImpl *m_pMutex_Refs;
	NCS::SDK::CSpinRWLockImpl *m_prwMutex_Refs;
	NCS::SDK::CSpinLockImpl *m_pMutex;
	NCS::SDK::CSpinRWLockImpl *m_prwMutex;
	NCS::SDK::CSpinRWLockImpl *m_prwMutex_subband_creation; // added on 24/04/2014 for m_nSubBandsCreation and m_nSubBandsDecode
	NCS::SDK::CSpinRWLockImpl *m_prwMutex_subband_decode;// for m_nSubBandsDecode

	public:
		//Context *GetContext(ContextID nCtx) {
			//Context *pCtx = (CPrecinct::Context*)CNode2D::GetContext(nCtx, false);
			//return pCtx;
		//};
};

class NCSECW_IMPEX CSPPrecinct: public NCS::JPC::CPrecinct {
public:
	CSPPrecinct(class CResolution *pResolution, UINT32 nPrecinct, bool bCreateSubBands = false) : CPrecinct(pResolution, nPrecinct, bCreateSubBands) {};
	virtual bool CreateSubBands(ContextID nCtx, bool bRead, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType, CRect2D& rectDst);
};

class NCSECW_IMPEX CMTPrecinct: public NCS::JPC::CPrecinct {
public:
	CMTPrecinct(class CResolution *pResolution, UINT32 nPrecinct, bool bCreateSubBands = false);
	virtual ~CMTPrecinct();
	virtual bool CreateSubBands(ContextID nCtx, bool bRead, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType);
	virtual bool DecodeSubBands(ContextID nCtx, NCS::SDK::CBuffer2D::Type eType, CRect2D& rectDst);
	void Release(); // added on 25/04/2014 

	class CReadJob: public NCS::CThreadPool::CJob {
	public:
		CReadJob(NCS::JPC::CSubBand *pSubBand, NCS::SDK::CBuffer2D::Type eType, NCS::CEvent &JobDoneEvent, NCS::SDK::CAtomicInt &JobDoneCount) : m_JobDoneEvent(JobDoneEvent), m_JobDoneCount(JobDoneCount) {
				m_pSubBand = pSubBand;
				m_eType = eType;
				m_bUseRectDst = false;
				m_bDone = false;
				NCS::CThreadPool::CJob::m_bDeleteThisJobOnceDone = true;
				//m_pSubBand->IncreaseNumQueuedJobs();
			};
		CReadJob(NCS::JPC::CSubBand *pSubBand, NCS::SDK::CBuffer2D::Type eType, CRect2D& rectDst, NCS::CEvent &JobDoneEvent, NCS::SDK::CAtomicInt &JobDoneCount) : m_JobDoneEvent(JobDoneEvent), m_JobDoneCount(JobDoneCount) {
			m_pSubBand = pSubBand;
			m_eType = eType;
			m_rectDst = rectDst;
			m_bUseRectDst = true;
			m_bDone = false;
			NCS::CThreadPool::CJob::m_bDeleteThisJobOnceDone = true;
			//m_pSubBand->IncreaseNumQueuedJobs();
		};
		virtual ~CReadJob();
		/*
		virtual ~CReadJob() {
			if (m_pReadJobsMutex) {
				NCS::SDK::CSpinLockImpl::CScopedLock _Lock(*m_pReadJobsMutex);
				m_JobDoneCount++;
			} else
				m_JobDoneCount++;
			m_bDone = true;
			m_JobDoneEvent.Set();
			//m_pSubBand->DecreaseNumQueuedJobs();
		};
		*/
		virtual void Work(void) {
				//this seems to slow it down
				//tbb::task_scheduler_init ts(tbb::task_scheduler_init::automatic);

#ifdef DEBUG_MT_SUBBAND_DECODING
				{				
					CReadJobDoing = CReadJobDoing +1;
					NCS::CLog::Log(MOD_ECWSDK_ECWJP2, NCS_INFO_LEVEL, NCS_T("CMTPrecinct::Work Doing Count = %d"), CReadJobDoing); 
					if (CReadJobDoing > CReadJobCount)
						NCS::CLog::Log(MOD_ECWSDK_ECWJP2, NCS_INFO_LEVEL, NCS_T("CMTPrecinct::Work Doing Count %d > Queued Count %d"), CReadJobDoing, CReadJobCount); 
				}
#endif
			if (m_bUseRectDst)
				m_pSubBand->DecodeCodeBlocks(m_eType, m_rectDst);
			else
				m_pSubBand->DecodeCodeBlocks(m_eType);
			//m_pSubBand->DecreaseNumQueuedJobs();
				

#ifdef DEBUG_MT_SUBBAND_DECODING
				{				
					CReadJobDone = CReadJobDone +1;
					NCS::CLog::Log(MOD_ECWSDK_ECWJP2, NCS_INFO_LEVEL, NCS_T("CMTPrecinct::Work Done Count = %d"), CReadJobDone); 
				}
#endif
			};

	protected:
		NCS::JPC::CSubBand			*m_pSubBand;
		NCS::SDK::CBuffer2D::Type	m_eType;
		NCS::SDK::CRect2D			m_rectDst;
		bool						m_bUseRectDst;
		bool						m_bDone;
		NCS::CEvent					m_Event;
		NCS::CEvent &m_JobDoneEvent;
		NCS::SDK::CAtomicInt &m_JobDoneCount;
	};

	class CReadJobs: public std::vector<CReadJob*> {
	public:
		//CReadJobs();
		CReadJobs(bool bLock);
		//CReadJobs() {};
		virtual ~CReadJobs();
		//virtual ~CReadJobs() { Wait(); };

		void AddJob(CReadJob *pjob);

		void Wait(void);
		/*
		bool Wait(void) {
			for(int i = 0; i < (int)size(); i++) {
				(*this)[i]->Wait();
				delete (*this)[i];
			}
			clear();
		}; */

		//NCS::CEvent &GetEvent() { return m_JobDoneEvent; };
		//NCS::SDK::CAtomicInt &GetJobDoneCount() { return m_JobDoneCount; };
		NCS::CEvent &GetEvent(); // to add lock, changed on 24/04/2014
		NCS::SDK::CAtomicInt &GetJobDoneCount(); // to add lock, changed on 24/04/2014

		private:
			// added on 21/03/2013 to prevent greyness
			NCS::SDK::CSpinLockImpl *m_pReadJobsMutex;
			NCS::CEvent m_JobDoneEvent;
			NCS::SDK::CAtomicInt m_JobDoneCount;
			bool m_bLock;
	};
#if defined(NCSJPC_DECODE_CODEBLOCK_ONCE) || defined(NCSJPC_KEEP_SMALL_PRECINCT_DECODE)
	CReadJobs *m_Jobs;
#endif

#if !defined(NCSJPC_DECODE_CODEBLOCK_ONCE)
	std::vector<CReadJobs*> m_JobMap;
	NCS::SDK::CSpinLockImpl *m_pMutex_JobMap;
#endif
};

class NCSECW_IMPEX CPrecinctMap : public NCS::SDK::CNode2D {
public:
/*	const CPrecinct *&operator[](const UINT& _Key) {
		std::map<UINT32, CPrecinct*>::iterator it = find(_Key);
		if(it == end()) {
			static CPrecinct *NP = NULL;
			return(NP);
		} else {
			return(it->second);
		}
	}*/
	CPrecinctMap();
	virtual ~CPrecinctMap();
	void Init(class CResolution *pResolution);
	void ResetProgressionLayer(void);
	CPrecinct *find_or_create(UINT32 x, UINT32 y, bool *bCreated = NULL);
	CPrecinct *find(UINT32 nPrecinct, bool bRef = false);
	CPrecinct *find(UINT32 x, UINT32 y, bool bRef = false);
	CPrecinct *findPacketPrecinct(UINT32 nPacket, bool bRef = false);
	bool empty(UINT32 nY);
	void Clear(bool bLock = true); // Shao changed on 08/11/2011 to speed up when lock is not needed
	UINT8 ClearIfZero();
	UINT32 GetNodeWidth() {
		return(m_nWidth);
	}
	UINT32 GetNodeHeight() {
		return(m_nHeight);
	}
	UINT32 GetMaxSize() {
		return(m_nWidth * m_nHeight);
	}
	__inline UINT32 size(void) {
		return(m_nItems);
	}	
	bool remove(UINT32 nPrecinct, bool bMustRemove = true);
	bool remove(UINT32 x, UINT32 y, bool bMustRemove = true);
	bool remove(CPrecinct *p, bool bMustRemove = true);
	// added on 30/08/2012
	bool removeIfZero(UINT32 x, UINT32 y);
	// added on 07/09/2012
	bool PurgePrecincts();

	UINT32 remove_front();
#ifdef NOT_DEF
	void insert(ContextID nCtx, UINT32 nPrecinct, CPrecinct *p);
	void insert(ContextID nCtx, UINT32 x, UINT32 y, CPrecinct *p);
	void insert(UINT32 nPrecinct, CPrecinct *p);
	void insert(UINT32 x, UINT32 y, CPrecinct *p);
#endif
		/** 
		 * UnLink all precincts.
		 * @param		nCtx		Read context
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool UnLink(SDK::CNode2D::ContextID nCtx, UINT16 nInputs = 0);

#ifdef FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT
	// added on 29/04/2014 to store rect for intdividual precinct
	class NCSECW_IMPEX Context : public NCS::SDK::CNode2D::Context
	{
	public:
		Context();
		virtual ~Context();
	
		void Init(UINT32 nWidth, UINT32 nHeight);

		bool SetReadRect(UINT32 nNodeX, UINT32 nNodeY, CRect2D& Rect);
		CRect2D& GetReadRect(UINT32 nNodeX, UINT32 nNodeY);

	private:
		// rect[y][x], y = 0-CPrecinctMap::m_nHeight, x = 0-CPrecinctMap::m_nWidth
		// so rect represents the required reading region for the precinct (x, y) in this view
		typedef std::vector<CRect2D> CRect2DVector;
		std::vector< CRect2DVector > m_RectMap;
		CRect2D mInvalidRect;
		UINT32 m_nWidth;
		UINT32 m_nHeight;
	};

	CNode2D::Context *NewContext() {
		return((CNode2D::Context*)new Context());
	};

	Context *GetContext(ContextID nCtx) {
		Context *pCtx = (CPrecinctMap::Context*)CNode2D::GetContext(nCtx, true);
		return pCtx;
	};
#endif //FORCE_DECODE_WHOLE_REGION_IN_LARGE_PRECINCT	

	UINT64 GetMemorySize(); // Against bug EMS-407, added 01/11/2011
	bool CreatePackets();	// Against bug EMS-407, added 02/11/2011
	bool HasRefs();	// Against bug EMS-407, added 01/12/2011
	void AddRefs();	// Against bug EMS-407, added 29/11/2011

	NCS::SDK::CSpinLockImpl *m_pMutex_Map; // Against bug EMS-407, added 29/11/2011
	NCS::SDK::CSpinRWLockImpl *m_prwMutex_Map; 

private:
	class NCSECW_IMPEX CPrecinctMapRow {
	public:
		typedef std::map<UINT32, CPrecinct*>::iterator CPrecinctMapColumnIterator;
		NCS::SDK::CSpinLockImpl *m_pMutex;
		NCS::SDK::CSpinRWLockImpl *m_prwMutex;
		std::map<UINT32, CPrecinct*> m_Columns;

		CPrecinctMapRow();
		virtual ~CPrecinctMapRow();

		void Clear(bool bLock = true); // Shao changed on 08/11/2011 to speed up when lock is not needed
		
		void MoveBack(CPrecinct *pPrecinct);

		CPrecinct *m_pFront;
		CPrecinct *m_pBack;
	};
	typedef std::vector<CPrecinctMapRow>::iterator CPrecinctMapRowIterator;

	class NCSECW_IMPEX ICProgressionOrderPacketCalculator
	{
	public:
		ICProgressionOrderPacketCalculator() {
			m_pPrecinctMap = NULL;
			m_pResolution = NULL;
			m_nComponents = 0;
			m_nWidth = 0;
			m_nHeight = 0;
			m_nLayers = 0;
		};
		virtual ~ICProgressionOrderPacketCalculator() {
			m_pPrecinctMap = NULL; // just in case re-visit it, added on 01/12/2011
			m_pResolution = NULL; // just in case re-visit it, added on 01/12/2011
		};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p) = 0;
	protected:
		class CPrecinctMap *m_pPrecinctMap;
		class CResolution *m_pResolution;
		UINT32 m_nComponents;
		UINT32 m_nWidth;
		UINT32 m_nHeight;
		INT32 m_nLayers;
	};

	class NCSECW_IMPEX CPacketCalculatorLRCP : public ICProgressionOrderPacketCalculator
	{
	public:
		CPacketCalculatorLRCP() : ICProgressionOrderPacketCalculator() {
			m_nLayerFactor = 0;
			m_nResolutionOffset = 0;
		};
		virtual ~CPacketCalculatorLRCP() {};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p);
	private:
		UINT32 m_nLayerFactor;
		UINT32 m_nResolutionOffset;
	};

	class NCSECW_IMPEX CPacketCalculatorRLCP : public ICProgressionOrderPacketCalculator
	{
	public:
		CPacketCalculatorRLCP() : ICProgressionOrderPacketCalculator() {
			m_nStartPrecinctNumber = 0;
		};
		virtual ~CPacketCalculatorRLCP() {};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p);
	private:
		UINT32 m_nStartPrecinctNumber;
	};

	class NCSECW_IMPEX CPacketCalculatorRPCL : public ICProgressionOrderPacketCalculator
	{
	public:
		CPacketCalculatorRPCL() : ICProgressionOrderPacketCalculator() {
			m_nStartPrecinctNumber = 0;
		};
		virtual ~CPacketCalculatorRPCL() {};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p);
	private:
		UINT32 m_nStartPrecinctNumber;
	};

	class NCSECW_IMPEX CPacketCalculatorPCRL : public ICProgressionOrderPacketCalculator
	{
	public:
		CPacketCalculatorPCRL() : ICProgressionOrderPacketCalculator(){
			m_nResolutions = 0;
			m_XOffset.resize(0);
			m_Widths.resize(0);
			m_Heights.resize(0);
		};
		virtual ~CPacketCalculatorPCRL() {
			m_XOffset.clear();
			m_Widths.clear();
			m_Heights.clear();
		};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p);
	private:
		UINT32 m_nResolutions;
		std::vector<UINT32> m_XOffset;
		std::vector<UINT32> m_Widths;
		std::vector<UINT32> m_Heights;
	};

	class NCSECW_IMPEX CPacketCalculatorCPRL : public ICProgressionOrderPacketCalculator
	{
	public:
		CPacketCalculatorCPRL() {
			m_nComponentOffset = 0;
			m_nResolutionOffset = 0;
		};
		virtual ~CPacketCalculatorCPRL() {};

		virtual void Init(CPrecinctMap *pPrecinctMap);
		virtual void Set(UINT32 x, UINT32 y, CPrecinct *p);
	private:
		UINT32 m_nComponentOffset;
		UINT32 m_nResolutionOffset;
	};

	UINT32 m_nWidth;
	UINT32 m_nHeight;
	class CResolution *m_pResolution;
	
	UINT32 m_nItems;

	std::vector<CPrecinctMapRow *> m_Rows;
	ICProgressionOrderPacketCalculator *m_pProgressionOrderCalculator;

};
}
}

#endif // !NCSJPCPRECINCT_H
