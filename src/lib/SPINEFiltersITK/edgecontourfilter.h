#ifndef EDGECONTOURFILTER_HXX
#define EDGECONTOURFILTER_HXX

#include "itkThresholdImageFilter.h"

#include "itkImageToImageFilter.h"

#include "itkNeighborhoodIterator.h"

using namespace itk;

template< typename TInputImage, typename TOutputImage >
class EdgeContourFilter : public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    /** Standard class typedefs. */
    typedef EdgeContourFilter          Self;
    typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef SmartPointer< Self >        Pointer;
    typedef SmartPointer< const Self >  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(EdgeContourFilter, ImageToImageFilter);

    /** Some additional typedefs.  */
    typedef TInputImage                                InputImageType;
    typedef typename InputImageType::ConstPointer InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::PixelType    InputImagePixelType;

    typedef ConstNeighborhoodIterator< InputImageType > InputIterator;

    /** Some additional typedefs.  */
    typedef TOutputImage                               OutputImageType;
    typedef typename OutputImageType::Pointer    OutputImagePointer;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef typename OutputImageType::PixelType  OutputImagePixelType;
    typedef NeighborhoodIterator< OutputImageType > OutputIterator;


    void PrintSelf(std::ostream & os, Indent indent) const;

    /**  */
    void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                              ThreadIdType threadId);

protected:
    EdgeContourFilter();

};

#ifndef EDGECONTOURFILTER_TXX
#include <edgecontourfilter.txx>
#endif

#endif // EDGECONTOURFILTER_HXX
