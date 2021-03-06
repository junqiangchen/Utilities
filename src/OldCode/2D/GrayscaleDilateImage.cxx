#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"

#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 
#include "itkBinaryBoxStructuringElement.h" 
#include "itkBinaryDiamondStructuringElement.h" 

#include "vnl/vnl_math.h"
#include "global.h"

int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile outputImageFile radius  [type: box == 0, ball = 1, diamond = 2]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<PixelType, ImageDimension> ImageType;

  typedef itk::ImageFileReader<ImageType>  ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  if ( argc < 4 || atoi( argv[4] ) == 1 )
    {
    typedef itk::BinaryBallStructuringElement< 
                        PixelType,
                        ImageDimension>             StructuringElementType;
  
    typedef itk::GrayscaleDilateImageFilter<
                              ImageType, 
                              ImageType,
                              StructuringElementType >  DilateFilterType;
    DilateFilterType::Pointer  grayscaleDilate = DilateFilterType::New();
    StructuringElementType  structuringElement;
    structuringElement.SetRadius( atoi( argv[3] ) ); 
    structuringElement.CreateStructuringElement();
  
    grayscaleDilate->SetKernel( structuringElement );
    grayscaleDilate->SetInput( reader->GetOutput() );
    grayscaleDilate->Update();
  
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( grayscaleDilate->GetOutput() );
    writer->SetFileName( argv[2] );
    writer->Update();
    }
  else if ( atoi( argv[4] ) == 0 )
    {
    typedef itk::BinaryBoxStructuringElement< 
                        PixelType,
                        ImageDimension>             StructuringElementType;
  
    typedef itk::GrayscaleDilateImageFilter<
                              ImageType, 
                              ImageType,
                              StructuringElementType >  DilateFilterType;
    DilateFilterType::Pointer  grayscaleDilate = DilateFilterType::New();
    StructuringElementType  structuringElement;
    structuringElement.SetRadius( atoi( argv[3] ) ); 
    structuringElement.CreateStructuringElement();
  
    grayscaleDilate->SetKernel( structuringElement );
    grayscaleDilate->SetInput( reader->GetOutput() );
    grayscaleDilate->Update();
  
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( grayscaleDilate->GetOutput() );
    writer->SetFileName( argv[2] );
    writer->Update();
    }
  else
    {
    typedef itk::BinaryDiamondStructuringElement< 
                        PixelType,
                        ImageDimension>             StructuringElementType;
  
    typedef itk::GrayscaleDilateImageFilter<
                              ImageType, 
                              ImageType,
                              StructuringElementType >  DilateFilterType;
    DilateFilterType::Pointer  grayscaleDilate = DilateFilterType::New();
    StructuringElementType  structuringElement;
    structuringElement.SetRadius( atoi( argv[3] ) ); 
    structuringElement.CreateStructuringElement();
  
    grayscaleDilate->SetKernel( structuringElement );
    grayscaleDilate->SetInput( reader->GetOutput() );
    grayscaleDilate->Update();
  
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( grayscaleDilate->GetOutput() );
    writer->SetFileName( argv[2] );
    writer->Update();
    }
  

  return EXIT_SUCCESS;
}

