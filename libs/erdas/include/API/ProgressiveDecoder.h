/******************************************************************************
 ** Copyright, 1998 - 2014, Intergraph Corporation. All rights reserved.
 **
 ** FILE: ProgressiveDecoder.h
 ** CREATED: <20/08/2013>
 ** PURPOSE: Progressive decoder header file of ECW JPEG 2000 SDK
 **
 ******************************************************************************/
#ifndef NCS_API_PROGRESSIVE_DECODER_H
#define NCS_API_PROGRESSIVE_DECODER_H
#ifndef SWIG
    #include "NCSDefs.h"
#else
    #define NCSECW_IMPEX
#endif
#include "API/DecoderBase.h"

namespace NCS {
namespace API {

#ifndef SWIG
class CCurrentProgressiveView;
#endif

/**
 * \class	NCS::API::CProgressiveDecoder
 *
 * \brief	A decoder that draws all available data into the provided buffer and then notifies the client when there is more data available.
 * \ingroup	simplifiedclasses
 * 	
 * \author	Intergraph Perth
 * \date	8/08/2013
 */
class NCSECW_IMPEX CProgressiveDecoder : public CDecoderBase
{
public:
	CProgressiveDecoder();
	CProgressiveDecoder(const CProgressiveDecoder &s);
	virtual ~CProgressiveDecoder();

#ifndef SWIG
	const CProgressiveDecoder &operator=(const CProgressiveDecoder &s);
#endif

	/**
	 * \fn	virtual NCS::CError CProgressiveDecoder::Open(const char *szUTF8Path);
	 *
	 * \brief	Opens a ECW/JP2 Dataset at the given UTF8 encoded local path or ECWP URL.
	 *
	 * \author	Intergraph Perth
	 * \date	13/08/2013
	 *
	 * \param	szUTF8Path	Full UTF8 encoded local path or ECWP URL.
	 *
	 * \return	Error of the result.
	 */
	virtual NCS::CError Open(const char *szUTF8Path);

	/**
	 * \fn	NCS::CError CProgressiveDecoder::ReadRegion32bpp(std::vector<unsigned int> &Bandlist,
	 * 		NCSViewSize &viewSize, NCSWorldCoordinatesRect &worldRect, unsigned int *pRGBA);
	 *
	 * \brief	Reads a region from an ECW or JP2 dataset in 32bpp.  
	 * 			
	 * 			For regions less than 6000px x 6000px, if you need a region larger than this then you must use the CBlockingDecoder.
	 *
	 * \author	Intergraph Perth
	 * \date	13/08/2013
	 *
	 * \param [in,out]	Bandlist 	The bandlist, size of which determines the band count.
	 * \param [in,out]	viewSize 	Size of the output region in pixels.
	 * \param [in,out]	imageRect	The world coordinates source region of the dataset to read.
	 * \param [in,out]	pRGBA	 	The output buffer.
	 *
	 * \return	Error of the result.
	 */
	NCS::CError ReadRegion32bpp(std::vector<unsigned int> &Bandlist, NCSViewSize &viewSize, NCSWorldCoordinatesRect &worldRect, unsigned int *pRGBA);

	/**
	 * \fn	NCS::CError CProgressiveDecoder::ReadRegion32bpp(std::vector<unsigned int> &Bandlist,
	 * 		NCSViewSize &viewSize, NCSImageCoordinatesRect &imageRect, unsigned int *pRGBA);
	 *
	 * \brief	Reads a region from an ECW or JP2 dataset in 32bpp.  
	 * 			
	 *			For regions less than 6000px x 6000px, if you need a region larger than this then you must use the CBlockingDecoder.
	 *
	 * \author	Intergraph Perth
	 * \date	13/08/2013
	 *
	 * \param [in,out]	Bandlist 	The bandlist, size of which determines the band count.
	 * \param [in,out]	viewSize 	Size of the output region in pixels.
	 * \param [in,out]	imageRect	The image coordinates source region of the dataset to read.
	 * \param [in,out]	pRGBA	 	The output buffer.
	 *
	 * \return	Error of the result.
	 */
	NCS::CError ReadRegion32bpp(std::vector<unsigned int> &Bandlist, NCSViewSize &viewSize, NCSImageCoordinatesRect &imageRect, unsigned int *pRGBA);
	
	/**
	 * \fn	virtual void CProgressiveDecoder::RefreshUpdate(CCurrentView *pCurrentView)
	 *
	 * \brief	Refresh update callback override this function to receive notification when an update for the current view is available.
	 *
	 * \author	Intergraph Perth
	 * \date	13/08/2013
	 *
	 * \param [in,out]	pCurrentView	The current view determined by the last ReadRegion call.
	 */
	virtual void RefreshUpdate(CCurrentView *pCurrentView) {};

	/**
	 * \fn	void CProgressiveDecoder::SetBackgroundColour(unsigned int nRGBABGColour);
	 *
	 * \brief	Sets background colour.  This is the colour that is set on the ReadRegion output buffer, prior to drawing.
	 *
	 * \author	Intergraph Perth
	 * \date	13/08/2013
	 *
	 * \param	nRGBABGColour	The colour.
	 */
	void SetBackgroundColour(unsigned int nRGBABGColour);

private:
#ifndef SWIG
	CCurrentProgressiveView *m_pProgressiveView;
	UINT32 m_nBackgroundColour;
#endif
};

} // API
} // NCS

#endif // NCS_API_PROGRESSIVE_DECODER_H
