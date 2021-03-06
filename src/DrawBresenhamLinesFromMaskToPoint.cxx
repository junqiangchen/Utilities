#include "itkBresenhamLine.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLabelContourImageFilter.h"
#include "itkNeighborhoodIterator.h"
#include "itkVector.h"

#include <string>
#include <vector>

#include "Common.h"

template <unsigned int ImageDimension>
int DrawLines( int argc, char *argv[] )
{
  typedef unsigned int PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  typename ImageType::IndexType targetIndex;

  if( argc > 5 )
    {
    std::vector<int> point = ConvertVector<int>( std::string( argv[5] ) );
    for( unsigned int d = 0; d < ImageDimension; d++ )
      {
      targetIndex[d] = point[d];
      }
    }
  else
    {
    vnl_vector<float> centerOfMass( ImageDimension );
    centerOfMass.fill( 0.0 );
    float N = 0.0;

    itk::ImageRegionIteratorWithIndex<ImageType> It(
      reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion() );
    for( It.GoToBegin(); !It.IsAtEnd(); ++It )
      {
      if( It.Get() == 1 )
        {
        for( unsigned int d = 0; d < ImageDimension; d++ )
          {
          centerOfMass[d] += It.GetIndex()[d];
          }
        N++;
        }
      }
    for( unsigned int d = 0; d < ImageDimension; d++ )
      {
      centerOfMass[d] /= N;
      }
    for( unsigned int d = 0; d < ImageDimension; d++ )
      {
      targetIndex[d] = static_cast<int>( centerOfMass[d] );
      }
    }

  std::cout << "Target index = " << targetIndex << std::endl;

  if( argc > 4 )
    {
    std::fstream str( argv[4] );

    unsigned int numberOfDirections = 0;

    str >> numberOfDirections;

    typedef itk::BresenhamLine<ImageDimension> LinerType;
    LinerType liner;
    typedef typename LinerType::LType VectorType;
    typedef typename LinerType::OffsetType OffsetType;
    typedef typename LinerType::IndexType IndexType;

    std::vector<VectorType> directions;
    VectorType direction;

    float x = 0.0;

    unsigned int count = 0;
    while( str >> x )
      {
      direction[count % ImageDimension] = x;
      ++count;
      if( count % ImageDimension == 0 )
        {
        directions.push_back( direction );
        }
      }

    typename ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    unsigned long maxLength = 0;
    for( unsigned int d = 0; d < ImageDimension; d++ )
      {
      maxLength += size[d] * size[d];
      }
    maxLength = static_cast<unsigned long>( vcl_sqrt( maxLength ) );

    for( unsigned int d = 0; d < directions.size(); d++ )
      {
      IndexType currentIndex = targetIndex;

      typename LinerType::OffsetArray offsets = liner.BuildLine( directions[d], maxLength );
      typename LinerType::OffsetArray::const_iterator it;

      bool isFound = false;
      for( it = offsets.begin(); it != offsets.end(); ++it )
        {
        if( !reader->GetOutput()->GetLargestPossibleRegion().IsInside( currentIndex ) )
          {
          break;
          }
        if( reader->GetOutput()->GetPixel( currentIndex ) == 1 )
          {
          isFound = true;
          break;
          }
        currentIndex = targetIndex + *it;
        }

      if( isFound )
        {
        currentIndex = targetIndex;

        bool inSkull = false;
        for( it = offsets.begin(); it != offsets.end(); ++it )
          {
          if( !reader->GetOutput()->GetLargestPossibleRegion().IsInside( currentIndex ) )
            {
            break;
            }
          if( reader->GetOutput()->GetPixel( currentIndex ) == 1 && inSkull == false )
            {
            reader->GetOutput()->SetPixel( currentIndex, d + 3 );
            inSkull = true;
            }
          else if( reader->GetOutput()->GetPixel( currentIndex ) != 1 && inSkull == true )
            {
            IndexType previousIndex = targetIndex + offsets[it - offsets.begin() - 2];
            reader->GetOutput()->SetPixel( previousIndex, d + 3 );
            inSkull = false;
            }
          else
            {
            reader->GetOutput()->SetPixel( currentIndex, 2 );
            }
          currentIndex = targetIndex + *it;
          }
        }

      currentIndex = targetIndex;

      offsets = liner.BuildLine( -directions[d], maxLength );

      isFound = false;
      for( it = offsets.begin(); it != offsets.end(); ++it )
        {
        if( !reader->GetOutput()->GetLargestPossibleRegion().IsInside( currentIndex ) )
          {
          break;
          }
        if( reader->GetOutput()->GetPixel( currentIndex ) == 1 )
          {
          isFound = true;
          break;
          }
        currentIndex = targetIndex + *it;
        }
      if( isFound )
        {
        currentIndex = targetIndex;

        bool inSkull = false;
        for( it = offsets.begin(); it != offsets.end(); ++it )
          {
          if( !reader->GetOutput()->GetLargestPossibleRegion().IsInside( currentIndex ) )
            {
            break;
            }
          if( reader->GetOutput()->GetPixel( currentIndex ) == 1 && inSkull == false )
            {
            reader->GetOutput()->SetPixel( currentIndex, d + 3 + numberOfDirections );
            inSkull = true;
            }
          else if( reader->GetOutput()->GetPixel( currentIndex ) != 1 && inSkull == true )
            {
            IndexType previousIndex = targetIndex + offsets[it - offsets.begin() - 2];
            reader->GetOutput()->SetPixel( previousIndex, d + 3 + numberOfDirections );
            inSkull = false;
            }
          else
            {
            reader->GetOutput()->SetPixel( currentIndex, 2 );
            }
          currentIndex = targetIndex + *it;
          }
        }
      }
    }
  else
    {
    typedef itk::BresenhamLine<ImageDimension> LinerType;
    LinerType liner;

    itk::ImageRegionIteratorWithIndex<ImageType> It(
      reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion() );
    for( It.GoToBegin(); !It.IsAtEnd(); ++It )
      {
      if( It.Get() == 1 )
        {
        typename ImageType::IndexType startIndex = It.GetIndex();

        typename LinerType::IndexArray indices = liner.BuildLine( startIndex, targetIndex );

        typename LinerType::IndexArray::const_iterator it;
        for( it = indices.begin(); it != indices.end(); it++ )
          {
          if( reader->GetOutput()->GetPixel( *it ) == 0 )
            {
            reader->GetOutput()->SetPixel( *it, 2 );
            }
          }
        }
      }

    typedef itk::LabelContourImageFilter<ImageType, ImageType> ContourFilterType;
    typename ContourFilterType::Pointer contours = ContourFilterType::New();
    contours->SetInput( reader->GetOutput() );
    contours->SetFullyConnected( true );
    contours->SetBackgroundValue( 0 );
    contours->Update();

    typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill( 1 );

    unsigned numberOfPixels = 1;
    for( unsigned int n = 0; n < ImageDimension; n++ )
      {
      numberOfPixels *= 3;
      }

    NeighborhoodIteratorType ItN( radius, contours->GetOutput(),
      contours->GetOutput()->GetLargestPossibleRegion() );
    for( ItN.GoToBegin(); !ItN.IsAtEnd(); ++ItN )
      {
      if( ItN.GetCenterPixel() == 1 )
        {
        for( unsigned int n = 0; n < numberOfPixels; n++ )
          {
          bool isInBounds = false;
          typename ImageType::PixelType neighbor = ItN.GetPixel( n, isInBounds );
          if( isInBounds && neighbor == 2 )
            {
            reader->GetOutput()->SetPixel( ItN.GetIndex(), 3 );
            break;
            }
          }
        }
      }
    }

  reader->GetOutput()->SetPixel( targetIndex, 4 );

  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( reader->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}

int main( int argc, char *argv[] )
{
  if ( argc < 4 )
    {
    std::cout << argv[0] << " imageDimension mask1 outputFile [sampleVectorFile] [centerIndex]" << std::endl;
    std::cout << "     Note:  inputMask is assumed to be 1/0 with mask label = 1." << std::endl;
    std::cout << "     Note2:  sample vector file is like those in PointSets/ in Camino." << std::endl;
    exit( 0 );
    }

  switch( atoi( argv[1] ) )
   {
   case 2:
     DrawLines<2>( argc, argv );
     break;
   case 3:
     DrawLines<3>( argc, argv );
     break;
   default:
      std::cerr << "Unsupported dimension" << std::endl;
      exit( EXIT_FAILURE );
   }
}

