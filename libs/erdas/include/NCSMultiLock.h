/********************************************************
** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
**
** FILE:   	NCSPolygon.h
** CREATED:	29-Dec-2008
** AUTHOR: 	Russell Rogers
** PURPOSE:	Interface for shared locks.
*******************************************************/

#if !defined(NCSMULTILOCK_H)
#define NCSMULTILOCK_H

#include "NCSUtil.h"
#include "NCSMutex.h"
#include "NCSEvent.h"

#include <map>

namespace NCS {

class NCSECW_IMPEX CMultiLock
{
public:
	CMultiLock();
	virtual ~CMultiLock();

	virtual void LockShared();
	virtual bool TryLockShared();
	virtual void UnLockShared();
	virtual bool LockExclusive();
	virtual bool TryLockExclusive();
	virtual void ConvertToExclusive();
	virtual void UnLockExclusive();

protected:
	CMutex m_mSharedCount;
	INT32 m_nSharedCount;

	CMutex m_Mutex;
	CEvent m_OK2WriteEvent;
	bool m_bLockedExclusively;
};

class NCSECW_IMPEX CSafeMultiLock : public CMultiLock
{
public:
	CSafeMultiLock();
	virtual ~CSafeMultiLock();

	virtual void LockShared();
	virtual bool TryLockShared();
	virtual void UnLockShared();
	virtual bool LockExclusive();
	virtual bool TryLockExclusive();
	virtual void ConvertToExclusive();
	virtual void UnLockExclusive();

protected:
	class CSharedTIDMap;

	CSharedTIDMap *m_pSharedTID;
};

class NCSECW_IMPEX CSharedLock
{
public:
	CSharedLock( CMultiLock *pMultiLock )
	{
		m_pMultiLock = pMultiLock;
		m_pMultiLock->LockShared();
	}
	CSharedLock( CSafeMultiLock *pMultiLock )
	{
		m_pMultiLock = static_cast<CMultiLock*>(pMultiLock);
		m_pMultiLock->LockShared();
	}
	CSharedLock( CMultiLock &MultiLock )
	{
		m_pMultiLock = &MultiLock;
		m_pMultiLock->LockShared();
	}
	CSharedLock( CSafeMultiLock &MultiLock )
	{
		m_pMultiLock = static_cast<CMultiLock*>(&MultiLock);
		m_pMultiLock->LockShared();
	}
	virtual ~CSharedLock()
	{
		Release();
	}
	void Release() {
		if(m_pMultiLock) {
			m_pMultiLock->UnLockShared();
			m_pMultiLock = NULL;
		}
	}

private:
	CMultiLock *m_pMultiLock;

};

class NCSECW_IMPEX CExclusiveLock
{
public:
	CExclusiveLock( CMultiLock *pMultiLock )
	{
		m_pMultiLock = pMultiLock;
		m_pMultiLock->LockExclusive();
	}
	CExclusiveLock( CSafeMultiLock *pMultiLock )
	{
		m_pMultiLock = static_cast<CMultiLock*>(pMultiLock);
		m_pMultiLock->LockExclusive();
	}
	CExclusiveLock( CMultiLock &MultiLock )
	{
		m_pMultiLock = &MultiLock;
		m_pMultiLock->LockExclusive();
	}
	CExclusiveLock( CSafeMultiLock &MultiLock )
	{
		m_pMultiLock = static_cast<CMultiLock*>(&MultiLock);
		m_pMultiLock->LockExclusive();
	}
	virtual ~CExclusiveLock()
	{
		Release();
	}
	void Release() {
		if(m_pMultiLock) {
			m_pMultiLock->UnLockExclusive();
			m_pMultiLock = NULL;
		}
	}
private:
	CMultiLock *m_pMultiLock;

};


}

#ifndef NCS_NO_COMPAT_NAMES
typedef NCS::CMultiLock CNCSMultiLock;
typedef NCS::CExclusiveLock CNCSExclusiveLock;
typedef NCS::CSharedLock CNCSSharedLock;
#endif

#endif // !defined(NCSMULTILOCK_H)
