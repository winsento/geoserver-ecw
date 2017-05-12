/******************************************************************************
 ** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
 **
 ** FILE: ViewInternal.h
 ** CREATED: <20/08/2013>
 ** PURPOSE: View internal header file of ECW JPEG 2000 SDK
 **
 ******************************************************************************/
#ifndef NCS_API_VIEW_INTERNAL_H
#define NCS_API_VIEW_INTERNAL_H

#include "API/DecoderBase.h"
#include "API/ProgressiveDecoder.h"
#include "NCSFile.h"
#include "NCSRect.h"
#include "NCSEcw/SDK/Resample_T.h"

using namespace NCS;
using namespace API;

class NCS::API::CCurrentProgressiveView 
{
public:
	CCurrentProgressiveView() : m_nCurrentViewBufferSize(0), m_pCurrentViewBuffer(NULL), m_pProgressiveCallback(NULL), m_nBackgroundColour(0)
	{
		memset(&m_CurrentView, 0x0, sizeof(NCSSetViewInfo));
	}
	virtual ~CCurrentProgressiveView()
	{
		if(m_pCurrentViewBuffer) {
			NCSFree(m_pCurrentViewBuffer);
			m_pCurrentViewBuffer = NULL;
		}
		m_nCurrentViewBufferSize = 0;
	}

	bool CalculateDeviceCoords(int nDeviceTLX, int nDeviceTLY, int nDeviceWidth, int nDeviceHeight,
							   double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY,
							   double m_dCurrentWorldTLX, double m_dCurrentWorldTLY, double m_dCurrentWorldBRX, double m_dCurrentWorldBRY,
							   int *outputDeviceCoords)
	{
		double tlx, tly, brx, bry;

		tlx = ((m_dCurrentWorldTLX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(double)(nDeviceWidth)  + (double)nDeviceTLX;
		tly = ((m_dCurrentWorldTLY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(double)(nDeviceHeight) + (double)nDeviceTLY;
		brx = ((m_dCurrentWorldBRX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(double)(nDeviceWidth)  + (double)nDeviceTLX;
		bry = ((m_dCurrentWorldBRY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(double)(nDeviceHeight) + (double)nDeviceTLY;

		// Round - be careful to round in the right direction if the origin is negative.
		outputDeviceCoords[0] = (int)(tlx + (tlx < 0.0 ? -0.5 : 0.5));
		outputDeviceCoords[1] = (int)(tly + (tly < 0.0 ? -0.5 : 0.5));			
		outputDeviceCoords[2] = (int)(brx + (brx < 0.0 ? -0.5 : 0.5));
		outputDeviceCoords[3] = (int)(bry + (bry < 0.0 ? -0.5 : 0.5));
	
		return true;
	}

	bool calculateImageCoords( double dDevice1TLX, double dDevice1TLY, double dDevice1BRX, double dDevice1BRY,
                                double dImageWidth, double dImageHeight,
                                double dDevice2TLX, double dDevice2TLY, double dDevice2BRX, double dDevice2BRY,
								double *pOutputImageCoords)
	{
		double outputImageWidth, outputImageHeight;
	
		outputImageWidth  = ((dDevice2BRX-dDevice2TLX) * (dImageWidth))/(dDevice1BRX-dDevice1TLX)  ;
		outputImageHeight = ((dDevice2BRY-dDevice2TLY) * (dImageHeight))/(dDevice1BRY-dDevice1TLY)  ;
		pOutputImageCoords[0] = ((dDevice2TLX-dDevice1TLX)  * (outputImageWidth))/(dDevice2BRX-dDevice2TLX) ;
		pOutputImageCoords[1] = ((dDevice2TLY-dDevice1TLY)  * (outputImageHeight))/(dDevice2BRY-dDevice2TLY) ;
		pOutputImageCoords[2] = pOutputImageCoords[0] + outputImageWidth;
		pOutputImageCoords[3] = pOutputImageCoords[1] + outputImageHeight;
	
		return true;
	}

	void SetProgressiveCallback(CProgressiveDecoder *pProgressiveCallback)
	{
		m_pProgressiveCallback = pProgressiveCallback;
	}
//
//	void Paint(int g_tlx, int g_tly, int g_width, int g_height, double g_worldtlx, double g_worldtly, double g_worldbrx, double g_worldbry)
//	{
//	int m_nCurrentWidth  = width();
//	int m_nCurrentHeight = height();
//	int dOutputDeviceCoords[4];
//
//	CalculateDeviceCoords(g_tlx, g_tly, g_width, g_height,
//						g_worldtlx, g_worldtly, g_worldbrx, g_worldbry,
//						m_dWorldTLX, m_dWorldTLY, m_dWorldBRX, m_dWorldBRY, 
//						dOutputDeviceCoords);
//
//	if (dOutputDeviceCoords[2] == 0 || dOutputDeviceCoords[3] == 0) return;
//	
//	// If the current width, height and coords match the display area, just blit it to the screen.
//	if (m_nCurrentWidth  == (dOutputDeviceCoords[2] - dOutputDeviceCoords[0]) &&
//		m_nCurrentHeight == (dOutputDeviceCoords[3] - dOutputDeviceCoords[1]) &&
//		g_worldtlx == m_dWorldTLX &&
//		g_worldtly == m_dWorldTLY &&
//		g_worldbrx == m_dWorldBRX &&
//		g_worldbry == m_dWorldBRY) 
//	{
//		painter.drawImage(g_tlx, g_tly, *this);
//	}
//	else
//	{		
//		// Second easiest case, the width and height match, but the coords are offset, just blit to the new location.
//		if (m_nCurrentWidth  == (dOutputDeviceCoords[2] - dOutputDeviceCoords[0]) &&
//			m_nCurrentHeight == (dOutputDeviceCoords[3] - dOutputDeviceCoords[1]) ) 
//		{
//			painter.drawImage(dOutputDeviceCoords[0], dOutputDeviceCoords[1], *this);
//		}
//		else {
//
//			// This is the complex bit, stretching the image. We need to work out:
//			// A) The screen coordinates of where the available imagery maps to
//			// B) Clip these screen coords to the actual screen area.
//			// c) From the screen area, calculate the corresponding coords in the image from which we will do the stretch.
//
//			// Calculate the actual SCREEN area to which the current image can be stretched, then clip it to the device coords.
//			qreal outputScreenTLX = (dOutputDeviceCoords[0] < (qreal)g_tlx)              ? (qreal)g_tlx              : dOutputDeviceCoords[0];
//			qreal outputScreenTLY = (dOutputDeviceCoords[1] < (qreal)g_tly)              ? (qreal)g_tly              : dOutputDeviceCoords[1];
//			qreal outputScreenBRX = (dOutputDeviceCoords[2] > (qreal)(g_tlx + g_width))  ? (qreal)(g_tlx + g_width)  : dOutputDeviceCoords[2];
//			qreal outputScreenBRY = (dOutputDeviceCoords[3] > (qreal)(g_tly + g_height)) ? (qreal)(g_tly + g_height) : dOutputDeviceCoords[3];
//
//			// Calculate the actual IMAGE coords that map to the screen extent area calculated above. 
//			qreal pOutputImageCoords[4];
//			calculateImageCoords(dOutputDeviceCoords[0], dOutputDeviceCoords[1], dOutputDeviceCoords[2], dOutputDeviceCoords[3],
//								 m_nCurrentWidth, m_nCurrentHeight,
//                                 outputScreenTLX, outputScreenTLY, outputScreenBRX, outputScreenBRY,
//								 pOutputImageCoords);
//
//			// Convert the new IMAGE coords back into world coords, from the edges of the image coords.
//			qreal dNewOutputWorldTLX, dNewOutputWorldTLY, dNewOutputWorldBRX, dNewOutputWorldBRY;
//			qreal outputDeviceCoordsTLX, outputDeviceCoordsTLY, outputDeviceCoordsBRX, outputDeviceCoordsBRY;
//
//			dNewOutputWorldTLX = m_dWorldTLX + ((m_dWorldBRX - m_dWorldTLX)/((qreal)m_nCurrentWidth))  * (pOutputImageCoords[0]);
//			dNewOutputWorldTLY = m_dWorldTLY + ((m_dWorldBRY - m_dWorldTLY)/((qreal)m_nCurrentHeight)) * (pOutputImageCoords[1]);
//			dNewOutputWorldBRX = m_dWorldTLX + ((m_dWorldBRX - m_dWorldTLX)/((qreal)m_nCurrentWidth))  * (pOutputImageCoords[2]);
//			dNewOutputWorldBRY = m_dWorldTLY + ((m_dWorldBRY - m_dWorldTLY)/((qreal)m_nCurrentHeight)) * (pOutputImageCoords[3]);
//		
//			// Convert these new world coordinates into device coordinates.
//			outputDeviceCoordsTLX = (((dOutputDeviceCoords[2] - dOutputDeviceCoords[0]) * (dNewOutputWorldTLX-m_dWorldTLX))/(m_dWorldBRX-m_dWorldTLX)) + dOutputDeviceCoords[0];
//			outputDeviceCoordsTLY = (((dOutputDeviceCoords[3] - dOutputDeviceCoords[1]) * (dNewOutputWorldTLY-m_dWorldTLY))/(m_dWorldBRY-m_dWorldTLY)) + dOutputDeviceCoords[1];
//			outputDeviceCoordsBRX = (((dOutputDeviceCoords[2] - dOutputDeviceCoords[0]) * (dNewOutputWorldBRX-m_dWorldTLX))/(m_dWorldBRX-m_dWorldTLX)) + dOutputDeviceCoords[0];
//			outputDeviceCoordsBRY = (((dOutputDeviceCoords[3] - dOutputDeviceCoords[1]) * (dNewOutputWorldBRY-m_dWorldTLY))/(m_dWorldBRY-m_dWorldTLY)) + dOutputDeviceCoords[1];
//
//			QRectF srcRectF(outputDeviceCoordsTLX, 
//							outputDeviceCoordsTLY, 
//							outputDeviceCoordsBRX - outputDeviceCoordsTLX, 
//							outputDeviceCoordsBRY - outputDeviceCoordsTLY);
//			QRectF dstRectF(pOutputImageCoords[0], 
//							pOutputImageCoords[1], 
//							pOutputImageCoords[2] - pOutputImageCoords[0], 
//							pOutputImageCoords[3] - pOutputImageCoords[1]);
//			Q_ASSERT(!isNull());
//			painter.drawImage(srcRectF, *this, dstRectF);
//		}
//	}
//}

	virtual NCS::CError Read(NCS::CView *pView, int nBands, int *pBandlist, int nWidth, int nHeight, double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY, unsigned int *pRGBA)
	{
		NCS::CMutexLock _Lock(m_Mutex);

		bool bNeedSetView = true;
		NCS::CError Error;

		{
			if(nBands == m_CurrentView.nBands && memcmp(pBandlist, m_CurrentView.pBandList, m_CurrentView.nBands*sizeof(INT32)) == 0) {

				if(dWorldBRX == m_CurrentView.fBottomX &&
				   dWorldTLY == m_CurrentView.fLeftY &&
				   dWorldBRY == m_CurrentView.fRightY &&
				   dWorldTLX == m_CurrentView.fTopX &&
				   nWidth == m_CurrentView.nSizeX &&
				   nHeight == m_CurrentView.nSizeY) {

					UINT32 nSize = m_CurrentView.nSizeX * m_CurrentView.nSizeY * sizeof(UINT32);
					memcpy(pRGBA, m_pCurrentViewBuffer, nSize);
					bNeedSetView = false;
				} else {
					CNCSRect_D NewView(dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY);
					NewView.Normalize();
					CNCSRect_D Intersect = m_CurrentViewRect.Common(NewView);
					if(Intersect.IsValid()) {
						double dCurrentViewCellSizeX = m_CurrentViewRect.DeltaX() / m_CurrentView.nSizeX;
						double dCurrentViewCellSizeY = m_CurrentViewRect.DeltaY() / m_CurrentView.nSizeY;

						double dNewViewCellSizeX = NewView.DeltaX() / nWidth;
						double dNewViewCellSizeY = NewView.DeltaY() / nHeight;

						int nSrcOffsetX, nSrcOffsetY, nDstOffsetX, nDstOffsetY, nSrcCopyWidth, nSrcCopyHeight, nDstCopyWidth, nDstCopyHeight;
						nSrcOffsetX = nSrcOffsetY = nDstOffsetX = nDstOffsetY = 0;

						nSrcCopyWidth = m_CurrentView.nSizeX;
						nSrcCopyHeight = m_CurrentView.nSizeY;
						nDstCopyWidth = nWidth;
						nDstCopyHeight = nHeight;

						if(fabs(dCurrentViewCellSizeX - dNewViewCellSizeX) < FLT_EPSILON && fabs(dCurrentViewCellSizeY - dNewViewCellSizeY) < FLT_EPSILON) {
							// pan
							int nCopyWidth, nCopyHeight;
							double dV = (m_CurrentView.fTopX - dWorldTLX) / dCurrentViewCellSizeX;
							int nV = (int)(dV + (dV < 0.0 ? -0.5 : 0.5));
							if(nV < 0) {
								nSrcOffsetX = abs(nV);
								nCopyWidth = nWidth - nSrcOffsetX;
							} else {
								nDstOffsetX = nV;
								nCopyWidth = nWidth - nDstOffsetX;
							}

							if(dWorldTLY < dWorldBRY) {
								dV = (m_CurrentView.fLeftY - dWorldTLY) / dCurrentViewCellSizeY;
							} else {
								dV = (dWorldTLY - m_CurrentView.fLeftY) / dCurrentViewCellSizeY;
							}
							nV = (int)(dV + (dV < 0.0 ? -0.5 : 0.5));
							if(nV < 0) {
								nSrcOffsetY = abs(nV);
								nCopyHeight = nHeight - nSrcOffsetY;
							} else {
								nDstOffsetY = nV;
								nCopyHeight = nHeight - nDstOffsetY;
							}
						
							UINT32 *pDst = pRGBA;
							for(int nL = 0; nL < nDstOffsetY; nL++) {
								for(int nC = 0; nC < nWidth; nC++) {
									*pDst++ = m_nBackgroundColour;
								}
							}
							for(int nL = 0; nL < nCopyHeight; nL++) {
								UINT32 *pSrc = m_pCurrentViewBuffer + ((nL + nSrcOffsetY) * m_CurrentView.nSizeX) + nSrcOffsetX;
								//UINT32 *pDst = pRGBA + ((nL + nDstOffsetY) * nWidth) + nDstOffsetX;

								for(int nC = 0; nC < nDstOffsetX; nC++) {
									*pDst++ = m_nBackgroundColour;
								}

								memcpy(pDst, pSrc, nCopyWidth * sizeof(UINT32));
								pDst += nCopyWidth;

								for(int nC = nDstOffsetX+nCopyWidth; nC < nWidth; nC++) {
									*pDst++ = m_nBackgroundColour;
								}
							}
							for(int nL = nDstOffsetY+nCopyHeight; nL < nHeight; nL++) {
								for(int nC = 0; nC < nWidth; nC++) {
									*pDst++ = m_nBackgroundColour;
								}
							}
						} else {
							// zoom
							
							// This is the complex bit, stretching the image. We need to work out:
							// A) The screen coordinates of where the available imagery maps to
							// B) Clip these screen coords to the actual screen area.
							// c) From the screen area, calculate the corresponding coords in the image from which we will do the stretch.
				
							// Calculate the actual SCREEN area to which the current image can be stretched, then clip it to the device coords.
							
							UINT32 *pDst = pRGBA;
							for(int nL = 0; nL < nHeight; nL++) {
								for(int nC = 0; nC < nWidth; nC++) {
									*pDst++ = m_nBackgroundColour;
								}
							}

							int dOutputDeviceCoords[4];
							CalculateDeviceCoords(0, 0, nWidth, nHeight,
													dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY,
													m_CurrentView.fTopX, m_CurrentView.fLeftY, m_CurrentView.fBottomX, m_CurrentView.fRightY, 
													dOutputDeviceCoords);

							double outputScreenTLX = (dOutputDeviceCoords[0] < (double)0)              ? (double)0              : dOutputDeviceCoords[0];
							double outputScreenTLY = (dOutputDeviceCoords[1] < (double)0)              ? (double)0              : dOutputDeviceCoords[1];
							double outputScreenBRX = (dOutputDeviceCoords[2] > (double)(0 + nWidth))  ? (double)(0 + nWidth)  : dOutputDeviceCoords[2];
							double outputScreenBRY = (dOutputDeviceCoords[3] > (double)(0 + nHeight)) ? (double)(0 + nHeight) : dOutputDeviceCoords[3];
				
							// Calculate the actual IMAGE coords that map to the screen extent area calculated above. 
							double pOutputImageCoords[4];
							calculateImageCoords(dOutputDeviceCoords[0], dOutputDeviceCoords[1], dOutputDeviceCoords[2], dOutputDeviceCoords[3],
												 m_CurrentView.nSizeX, m_CurrentView.nSizeY,
				                                 outputScreenTLX, outputScreenTLY, outputScreenBRX, outputScreenBRY,
												 pOutputImageCoords);
				
							
							int nSrcR = 0;
							double dDstIncX = ((pOutputImageCoords[2]-pOutputImageCoords[0])/(outputScreenBRX-outputScreenTLX));
							double dDstIncY = ((pOutputImageCoords[3]-pOutputImageCoords[1])/(outputScreenBRY-outputScreenTLY));

							for(int nR = (int)outputScreenTLY; nR < (int)outputScreenBRY; nR++) {

								UINT32 *pSrc = m_pCurrentViewBuffer + (((int)(nSrcR*dDstIncY) + (int)pOutputImageCoords[1]) * m_CurrentView.nSizeX) + (int)pOutputImageCoords[0];
								UINT32 *pDst = pRGBA + (nR * nWidth) + (int)outputScreenTLX;
								
								NCS::SDK::Resample1Line_NearestNeighbour_INT<UINT32>(pDst, pSrc, 0, dDstIncX, (UINT32)(outputScreenBRX-outputScreenTLX), 0);
								nSrcR++;
							}
						}
					}
				}
			}
		}
		if(bNeedSetView) {
			Error = pView->SetView(nBands, pBandlist, nWidth, nHeight, dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY);
		}
		return Error;
	}

	virtual NCSReadStatus RefreshUpdateEx(NCS::CView *pView, NCSSetViewInfo *pViewSetInfo)
	{
		NCSReadStatus eReadStatus = NCS_READ_OK;

		UINT32 nSize = pViewSetInfo->nSizeX * pViewSetInfo->nSizeY * sizeof(UINT32);
		UINT32 *pNewViewBuffer = (UINT32 *)NCSMalloc(nSize, FALSE);
		UINT32 *pOldViewBuffer = m_pCurrentViewBuffer;
        
		UINT32 *pLine = pNewViewBuffer;
		for(UINT32 nL = 0; nL < pViewSetInfo->nSizeY && eReadStatus == NCS_READ_OK; nL++) {
			eReadStatus = pView->ReadLineRGBA(pLine);
			pLine += pViewSetInfo->nSizeX;
		}			
		
		if(eReadStatus == NCS_READ_OK) {
			{
				NCS::CMutexLock _Lock(m_Mutex);
				if(m_CurrentView.pBandList) {
					NCSFree(m_CurrentView.pBandList);
				}
				memcpy(&m_CurrentView, pViewSetInfo, sizeof(NCSSetViewInfo));
			
				m_CurrentView.pBandList = (UINT32 *)NCSMalloc(pViewSetInfo->nBands * sizeof(UINT32), FALSE);
				memcpy(m_CurrentView.pBandList, pViewSetInfo->pBandList, pViewSetInfo->nBands * sizeof(UINT32));

				m_CurrentViewRect.Set(m_CurrentView.fTopX, m_CurrentView.fLeftY, m_CurrentView.fBottomX, m_CurrentView.fRightY);
				m_CurrentViewRect.Normalize();
				m_pCurrentViewBuffer = pNewViewBuffer;
				m_nCurrentViewBufferSize = nSize;
			}
       
			NCSFree(pOldViewBuffer);

			if(m_pProgressiveCallback) {
				CCurrentView CurrentView;
				CurrentView = pViewSetInfo;
				m_pProgressiveCallback->RefreshUpdate(&CurrentView);
			}
		}
		return eReadStatus;
	}
	void SetBackgroundColour(UINT32 nBackgroundColour) { m_nBackgroundColour = nBackgroundColour; };
protected:
	NCSSetViewInfo m_CurrentView;
	UINT32 *m_pCurrentViewBuffer;
	UINT32 m_nCurrentViewBufferSize;
	UINT32 m_nBackgroundColour;
	CNCSRect_D m_CurrentViewRect;
	NCS::CMutex m_Mutex;

	CProgressiveDecoder *m_pProgressiveCallback;
};

class CDecoderBase::CView : public NCS::CView
{
public:
	CView() : m_pProgressiveView(NULL), NCS::CView() {};
	virtual ~CView() {};

	CView(const CView &s) 
	{
		*this = s;
	}

	const CView &operator=(const CView &s)
	{
		((NCS::CView &)*this) = (NCS::CView &)s;

		m_pProgressiveView = s.m_pProgressiveView;

		return *this;
	}

	virtual NCSReadStatus RefreshUpdateEx(NCSSetViewInfo *pViewSetInfo)
	{
		if(m_pProgressiveView) {
			return m_pProgressiveView->RefreshUpdateEx(this, pViewSetInfo);
		}
		return NCS_READ_OK;
	}

	void SetProgressiveDecoder(CCurrentProgressiveView *pProgressiveView)
	{
		m_pProgressiveView = pProgressiveView;
	}
protected:
	CCurrentProgressiveView *m_pProgressiveView;
};

#endif // NCS_API_VIEW_INTERNAL_H