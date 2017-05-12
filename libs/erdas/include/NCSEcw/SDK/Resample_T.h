/********************************************************** 
** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
** 
** FILE:   	Resample_T.h
** CREATED:	01/03/2012
** PURPOSE:	Consolidated resampling for ECW and JP2 decoders
**
********************************************************/

#ifndef NCS_SDK_RESAMPLE_T
#define NCS_SDK_RESAMPLE_T

#if defined(POSIX) && !defined(MACOSX) && !defined(NCS_ANDROID)
#include <fpu_control.h>
#endif

#include "NCSEcw/SDK/Buffer2D.h"

namespace NCS {
namespace SDK {

template <class BT>
static bool Resample1Line_NearestNeighbour_INT(BT *pDst, BT *pSrc, double dStartX, double dIncX, UINT32 nWidth, int nShift)
{
	register UINT32 number_x = nWidth;
	register UINT64 x_offset = ((UINT64) (dStartX * 0x10000) << 16);
	// This gives us 1/(2^16) accuracy per line, which should be fine
	register UINT64 increment_x = ((UINT64) (dIncX * 0x10000) << 16);
	register BT *p_in = pSrc;
	register BT *p_out = pDst;

	FLT_TO_INT_INIT();
	for(UINT32 x = 0; x < number_x; x++) {
		((BT*)p_out)[x] = (BT)(*(p_in + (UINT32) (x_offset >> 32))) >> nShift;
		x_offset += increment_x;
	}
	FLT_TO_INT_FINI();
	return(true);
}

template <class BT>
static bool Resample1Line_NearestNeighbour_FP(BT *pDst, BT *pSrc, double dStartX, double dIncX, UINT32 nWidth, int nShift)
{
	register UINT32 number_x = nWidth;
	register UINT64 x_offset = ((UINT64) (dStartX * 0x10000) << 16);
	// This gives us 1/(2^16) accuracy per line, which should be fine
	register UINT64 increment_x = ((UINT64) (dIncX * 0x10000) << 16);
	register BT *p_in = pSrc;
	register BT *p_out = pDst;
	register INT32 nDiv = NCS2Pow(nShift);

	FLT_TO_INT_INIT();
	for(UINT32 x = 0; x < number_x; x++) {
		((BT*)p_out)[x] = (BT)(*(p_in + (UINT32) (x_offset >> 32))) / nDiv;
		x_offset += increment_x;
	}
	FLT_TO_INT_FINI();
	return(true);
}

#define BILINEAR_INTERP_2x2_FLT(o, v00, v01, v10, v11, dx, dy) \
									o = (IEEE4)((v00 + (v01 - v00) * (dx)) + \
											((v10 + (v11 - v10) * (dx)) - \
											 (v00 + (v01 - v00) * (dx))) * (dy))
#define BILINEAR_INTERP_2x2_DBL(o, v00, v01, v10, v11, dx, dy) \
									o = (IEEE8)((v00 + (v01 - v00) * (dx)) + \
											((v10 + (v11 - v10) * (dx)) - \
											 (v00 + (v01 - v00) * (dx))) * (dy))
#define BILINEAR_INTERP_2x2_INT(o, v00, v01, v10, v11, dx, dy) \
								{ \
									float f;\
									BILINEAR_INTERP_2x2_FLT(f, v00, v01, v10, v11, dx, dy);\
									o = NCSfloatToInt32_RM(f); \
								}
#define BILINEAR_INTERP_2x2_INT64(o, v00, v01, v10, v11, dx, dy) \
								{ \
									double f;\
									BILINEAR_INTERP_2x2_DBL(f, v00, v01, v10, v11, dx, dy);\
									o = NCSfloatToInt64_RM(f); \
								}
#define BILINEAR_INTERP_2(v0, v1, d)	((v0) + ((v1) - (v0)) * (d))

template<class BT>
static bool Resample1Line_Bilinear_INT(BT *pDst, BT *pSrc1, BT *pSrc2, int nShift, double dStartX, double dIncX, double dY, INT32 nWidth, unsigned int ROUND_MODE)
{
	register INT32 number_x = nWidth;
	register BT *p_out = pDst;
	register BT *p_in = pSrc1;
	register BT *p_in2 = pSrc2;

#if defined(_WIN32)
#ifdef _WIN32_WCE
	unsigned int old_controlfp_val = _controlfp(ROUND_MODE, _MCW_RC);
#else
	unsigned int old_controlfp_val, old_controlfp_val_2;
	_controlfp_s(&old_controlfp_val, ROUND_MODE, _MCW_RC);
#endif
#elif defined (MACOSX) || defined(NCS_ANDROID)
    // dont need to do anything, as MACOSX only runs on newer x86-64 CPUs,and GCC takes care of this automatically.
#elif defined (POSIX)
	fpu_control_t fpu_oldcw, fpu_newcw;
	_FPU_GETCW(fpu_oldcw); //Store the old cw
	_FPU_SETCW(ROUND_MODE);
#else
	#error Not implemented
#endif

	double dX = dStartX;
	dIncX -= 0.5;
	for(register INT32 x = 0; x < number_x; x++) {
		//dX = dStartX + x * dIncX;
		dX += 0.5;
		double dXF = dX - (INT32)dX;
		INT32 nX1 = NCSFloor(dX);
		INT32 nX0 = nX1-1;
		
		BILINEAR_INTERP_2x2_INT(((BT*)p_out)[x], ((BT*)p_in)[nX0], ((BT*)p_in)[nX1], ((BT*)p_in2)[nX0], ((BT*)p_in2)[nX1], dXF, dY);
		((BT*)p_out)[x] >>= nShift;
		
		dX += dIncX;
	}
#if defined(_WIN32)
#ifdef _WIN32_WCE
	_controlfp(old_controlfp_val, _MCW_RC);
#else
	_controlfp_s(&old_controlfp_val_2, old_controlfp_val, _MCW_RC);
#endif
#elif defined (MACOSX) || defined(NCS_ANDROID)
    // dont need to do anything, as MACOSX only runs on newer x86 CPUs,and GCC takes care of this automatically.
#elif defined (POSIX)
	_FPU_SETCW(fpu_oldcw);
#else
	#error Not implemented
#endif
	return true;
}

template<class BT>
static bool Resample1Line_Bilinear_FP(BT *pDst, BT *pSrc1, BT *pSrc2, int nShift, double dStartX, double dIncX, double dY, INT32 nWidth)
{
	register INT32 number_x = nWidth;
	register BT *p_out = pDst;
	register BT *p_in = pSrc1;
	register BT *p_in2 = pSrc2;
	register INT32 nDiv = NCS2Pow(nShift);

#if defined(_WIN32)
#ifdef _WIN32_WCE
	unsigned int old_controlfp_val = _controlfp(_RC_NEAR, _MCW_RC);
#else
	unsigned int old_controlfp_val = _RC_NEAR;
	unsigned int old_controlfp_val_2 = 0;
	_controlfp_s(&old_controlfp_val, _RC_NEAR, _MCW_RC);
#endif
#elif defined (MACOSX) || defined(NCS_ANDROID)	
    // dont need to do anything, as MACOSX only runs on newer x86-64 CPUs,and GCC takes care of this automatically.
#elif defined (POSIX)
	fpu_control_t fpu_oldcw, fpu_newcw;
	_FPU_GETCW(fpu_oldcw); //Store the old cw
	fpu_newcw = (fpu_oldcw & ~_FPU_RC_DOWN & ~_FPU_RC_UP & ~_FPU_RC_ZERO) | _FPU_RC_NEAREST;
	_FPU_SETCW(fpu_newcw);
#else
	#error Not implemented
#endif

	double dX = dStartX;
	dIncX -= 0.5;
	for(register INT32 x = 0; x < number_x; x++) {
		//dX = dStartX + x * dIncX;
		dX += 0.5;
		double dXF = dX - (INT32)dX;
		INT32 nX1 = NCSFloor(dX);
		INT32 nX0 = nX1-1;

		BILINEAR_INTERP_2x2_FLT(((IEEE4*)p_out)[x], ((IEEE4*)p_in)[nX0], ((IEEE4*)p_in)[nX1], ((IEEE4*)p_in2)[nX0], ((IEEE4*)p_in2)[nX1], dXF, dY);
		((IEEE4*)p_out)[x] /= nDiv;
		
		dX += dIncX;
	}
#if defined(_WIN32)
#ifdef _WIN32_WCE
	_controlfp(old_controlfp_val, _MCW_RC);
#else
	_controlfp_s(&old_controlfp_val_2, old_controlfp_val, _MCW_RC);
#endif
#elif defined (MACOSX) || defined(NCS_ANDROID)	
    // dont need to do anything, as MACOSX only runs on newer x86 CPUs,and GCC takes care of this automatically.
#elif defined (POSIX)
	_FPU_SETCW(fpu_oldcw);
#else
	#error Not implemented
#endif
	return true;
}

} // NCS
} // SDK

#endif // NCS_SDK_RESAMPLE_T
