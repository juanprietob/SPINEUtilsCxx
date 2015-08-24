
#ifndef EDGECONTOURFILTER_TXX
#define EDGECONTOURFILTER_TXX

#include "edgecontourfilter.h"

template< typename TInputImage, typename TOutputImage >
EdgeContourFilter< TInputImage, TOutputImage >::EdgeContourFilter()
{
}

template< typename TInputImage, typename TOutputImage >
void EdgeContourFilter< TInputImage, TOutputImage >::PrintSelf(std::ostream & os, Indent indent) const{
    os << indent << "EdgeContourFilter"<< endl;
    Superclass::PrintSelf(os, indent);
}

/** ThresholdImageFilter can be implemented as a multithreaded filter.
 * Therefore, this implementation provides a ThreadedGenerateData() routine
 * which is called for each processing thread. The output image data is
 * allocated automatically by the superclass prior to calling
 * ThreadedGenerateData().  ThreadedGenerateData can only write to the
 * portion of the output image specified by the parameter
 * "outputRegionForThread"
 *
 * \sa ImageToImageFilter::ThreadedGenerateData(),
 *     ImageToImageFilter::GenerateData()  */
template< typename TInputImage, typename TOutputImage >
void EdgeContourFilter< TInputImage, TOutputImage >::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                          ThreadIdType threadId){


    const SizeValueType size0 = outputRegionForThread.GetSize(0);
    if( size0 == 0)
      {
      return;
      }
    itkDebugMacro(<< "Actually executing");

    // Get the input and output pointers
    InputImagePointer  inputPtr  = this->GetInput();
    OutputImagePointer outputPtr = this->GetOutput(0);

    typename InputImageType::SizeType radius;
    radius.Fill(1);

    typename InputImageType::SizeType size = inputPtr->GetLargestPossibleRegion().GetSize();
    for(int i = 0; i < 3; i++){
        if(size[i] == 1){
            radius[i] = 0;
        }
    }


    InputIterator  inIt(radius, inputPtr, outputRegionForThread);
    OutputIterator outIt(radius, outputPtr, outputRegionForThread);


    const size_t numberOfLinesToProcess = outputRegionForThread.GetNumberOfPixels() / size0;
    ProgressReporter progress( this, threadId, numberOfLinesToProcess );

    inIt.GoToBegin();
    outIt.GoToBegin();
    while(!inIt.IsAtEnd() && !outIt.IsAtEnd()){
        bool border = false;

        outIt.SetCenterPixel(0);

        if(inIt.GetCenterPixel() != 0){

            for(int i = 0; i < inIt.Size() && !border; i++){
                bool IsInBounds = false;
                inIt.GetPixel(i, IsInBounds);
                if(!IsInBounds){
                    border = true;
                }
            }

            for(int i = 0; i < inIt.Size() && !border; i++){
                bool IsInBounds = false;
                inIt.GetPixel(i, IsInBounds);
                if(IsInBounds && inIt.GetPixel(i) == 0){
                    border = true;
                }
            }
            if(border){
                outIt.SetCenterPixel(inIt.GetCenterPixel());
            }
        }

        ++inIt;
        ++outIt;

        progress.CompletedPixel();
    }
}

#endif
