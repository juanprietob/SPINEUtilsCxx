/*
  Author: Juan Carlos Prieto, SPINE execution of Weighted functional box plots
  Date: 07/10/14
*/

#include <stdlib.h>
#include "wfbplot.h"
#include "gaussweight.h"

#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"

#include "itkImageFileWriter.h"

using namespace std;

void help(char* exec){
    cerr<<exec<<endl;
    cerr<<"Usage options: "<<endl;
    cerr<<"-i <imageFilename>"<<endl;
    cerr<<"-l <labelValue>"<<endl;
    cerr<<"-o <outputDirectory>"<<endl;
    cerr<<"-of <outputFileCompound>"<<endl;
}

int main(int argv, char **argc)
{

    matrix* data;
    int pDim = 0;
    int nData = 0;
    string outDirectory = "";
    string outFile = "";
    vector< string > inputImages;

    double labelValue = -1;

    for(unsigned i = 1; i < argv; i++){
        string inputstring = argc[i];

        if(inputstring.compare("-i")==0){
            i++;
            inputstring = argc[i];
            inputImages.push_back(inputstring.c_str());
        }else if(inputstring.compare("-o")==0){
            i++;
            outDirectory = argc[i];
        }else if(inputstring.compare("-of")==0){
            i++;
            outFile = argc[i];
        }else if(inputstring.compare("-l") == 0){
            i++;
            labelValue = atof(argc[i]);
        }
    }

    if(inputImages.size() == 0 || labelValue == -1){
        help(argc[0]);
        return -1;
    }

    typedef double InputPixelType;
    static const int dimension = 3;
    typedef itk::Image< InputPixelType, dimension > InputImageType;
    typedef InputImageType::Pointer InputImagePointerType;

    typedef unsigned short OutputPixelType;
    typedef itk::Image< OutputPixelType, dimension > OutputImageType;
    typedef OutputImageType::Pointer OutputImagePointerType;

    typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
    typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;

    typedef itk::ImageFileWriter< OutputImageType > OutputImageFileWriterType;
    typedef OutputImageFileWriterType::Pointer OutputImageFileWriterPointerType;

    typedef itk::ImageRegionIterator< InputImageType > InputImageRegionIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputImageRegionIteratorType;

    InputImageType::RegionType region;
    InputImageType::RegionType imageRegion;
    InputImageType::SpacingType imageSpacing;
    InputImageType::PointType imageOrigin;
    InputImageType::DirectionType imageDirection;
    InputImageType::IndexType minIndex;
    minIndex.Fill(LLONG_MAX);
    InputImageType::IndexType maxIndex;
    maxIndex.Fill(LLONG_MIN);

    for(unsigned i = 0; i < inputImages.size(); i++){
        InputImageFileReaderPointerType reader = InputImageFileReaderType::New();
        reader->SetFileName(inputImages[i].c_str());
        reader->Update();
        InputImagePointerType img = reader->GetOutput();

        if(i == 0){
            imageRegion = img->GetLargestPossibleRegion();
            imageSpacing = img->GetSpacing();
            imageOrigin = img->GetOrigin();
            imageDirection = img->GetDirection();
        }

        InputImageRegionIteratorType it(img, img->GetLargestPossibleRegion());
        it.GoToBegin();
        while(!it.IsAtEnd()){
            if(it.Get() == (InputPixelType)labelValue){
                InputImageType::IndexType index = it.GetIndex();
                for(unsigned j = 0; j < dimension; j++){
                    if(index[j] < minIndex[j]){
                        minIndex[j] = index[j];
                    }
                    if(index[j] > maxIndex[j]){
                        maxIndex[j] = index[j];
                    }
                }
            }
            ++it;
        }

    }

    InputImageType::SizeType size;
    pDim = 1;
    nData = inputImages.size();

    for(unsigned j = 0; j < dimension; j++){
        size[j] = maxIndex[j] - minIndex[j];
        if(size[j] > 0){
            pDim *= size[j];
        }
    }

    if(pDim <= 0){
        cerr<<"Dimension is negative. There is no data to compute the Boxplot. Did you set the right label?"<<endl;
        return EXIT_FAILURE;
    }

    region.SetIndex(minIndex);
    region.SetSize(size);

    data = new matrix(nData, pDim);

    for(unsigned i = 0; i < inputImages.size(); i++){
        InputImageFileReaderPointerType reader = InputImageFileReaderType::New();
        reader->SetFileName(inputImages[i].c_str());
        reader->Update();
        InputImagePointerType img = reader->GetOutput();

        InputImageRegionIteratorType it(img, region);
        unsigned j = 0;
        for(j = 0, it.GoToBegin(); !it.IsAtEnd(); j++, ++it){
            if(it.Get() == labelValue && it.Get() != 0){
                data->setElement(i, j, 1);
            }else{
                data->setElement(i, j, 0);
            }

        }
    }

    wfbplot wfbTest(*data);
    //wfbTest.print();

    double *med = new double[pDim], *inf = new double[pDim], *sup = new double[pDim], *minBd = new double[pDim], *maxBd = new double[pDim], *depth = new double[nData];
    wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);

    vector< double* > bplotdata;
    bplotdata.push_back(minBd);
    bplotdata.push_back(inf);
    bplotdata.push_back(med);
    bplotdata.push_back(sup);
    bplotdata.push_back(maxBd);
    //bplotdata.push_back(depth);

    vector< string > bplotdatanames;
    bplotdatanames.push_back("minBd");
    bplotdatanames.push_back("inf");
    bplotdatanames.push_back("med");
    bplotdatanames.push_back("sup");
    bplotdatanames.push_back("maxBd");
    //bplotdatanames.push_back("depth");

    OutputImagePointerType outimgcompound = OutputImageType::New();
    outimgcompound->SetRegions(imageRegion);
    outimgcompound->SetSpacing(imageSpacing);
    outimgcompound->SetOrigin(imageOrigin);
    outimgcompound->SetDirection(imageDirection);
    outimgcompound->Allocate(true);


    for(unsigned i=0; i < bplotdata.size(); i++){
        OutputImagePointerType outimg = OutputImageType::New();
        outimg->SetRegions(imageRegion);
        outimg->SetSpacing(imageSpacing);
        outimg->SetOrigin(imageOrigin);
        outimg->SetDirection(imageDirection);
        outimg->Allocate(true);

        OutputImageRegionIteratorType outit(outimg, region);
        unsigned j = 0;
        for(outit.GoToBegin(), j = 0; !outit.IsAtEnd(); ++outit, j++){
            double value = bplotdata[i][j];
            outit.Set((OutputPixelType)value);
            if(value == 1 && outimgcompound->GetPixel(outit.GetIndex()) == 0){
                outimgcompound->SetPixel(outit.GetIndex(), i + 1);
            }
        }

        if(outDirectory.compare("") != 0){
            OutputImageFileWriterPointerType writer = OutputImageFileWriterType::New();
            writer->SetFileName(outDirectory + "/" + bplotdatanames[i] + ".nii.gz");
            writer->SetInput(outimg);
            writer->Update();
        }

    }

    if(outFile.compare("") != 0){
        OutputImageFileWriterPointerType writer = OutputImageFileWriterType::New();
        writer->SetFileName(outFile.c_str());
        writer->SetInput(outimgcompound);
        writer->Update();
    }

    /*if(outfilename.compare("") != 0){
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
        wfbTest.printVector(nData, depth);
    }*/





    delete data;
    delete med;
    delete inf;
    delete sup;
    delete minBd;
    delete maxBd;
    delete depth;



    // built atlas using weighted functional boxplot
    //cout<<" --double sigma = 30;
    /*for(int id=0; id < nData; id++)
    {
        //double weight[nData];
        // compute weight
        //gaussweight::computeModifiedGaussian( ages, nData, ages[id], sigma, ageRange[0], ageRange[1], weight);
        //wfbTest.setWeight(weight);
        //wfbTest.printVector(nData, weight);
        double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
        // call weight functional boxplot function
        wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
        wfbTest.printVector(nData, depth);
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
    }*/

    return EXIT_SUCCESS;
}




