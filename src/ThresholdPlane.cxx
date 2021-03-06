#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "vnl/vnl_math.h"

#include "Common.h"

template <unsigned int ImageDimension>
int ThresholdPlane( unsigned int argc, char *argv[] )
{
  typedef float RealType;
  typedef itk::Image<RealType, ImageDimension> ImageType;
  typedef itk::Vector<RealType, ImageDimension> VectorType;
  typedef itk::Image<VectorType, ImageDimension> DeformationFieldType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  std::vector<RealType> nor = ConvertVector<RealType>( argv[5] );
  std::vector<RealType> org = ConvertVector<RealType>( argv[4] );

  typename ImageType::PointType origin;
  VectorType normal;
  for( unsigned int d = 0; d < ImageDimension; d++ )
    {
    normal[d] = nor[d];
    origin[d] = org[d];
    }

  itk::ImageRegionIteratorWithIndex<ImageType> It( reader->GetOutput(),
    reader->GetOutput()->GetLargestPossibleRegion() );
  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
    if( It.Get() == 0 )
      {
      continue;
      }

    typename ImageType::PointType point;
    reader->GetOutput()->TransformIndexToPhysicalPoint( It.GetIndex(),
      point );

    VectorType vector = point - origin;

    RealType dot = 0.0;
    for( unsigned int d = 0; d < ImageDimension; d++ )
      {
      dot += vector[d] * normal[d];
      }
    RealType vectorNorm = vector.GetNorm();
    RealType normalNorm = normal.GetNorm();

    RealType angle = vcl_acos( dot / ( vectorNorm * normalNorm ) );

    if( angle > 0.5 * vnl_math::pi )
      {
      It.Set( 0 );
      }
    }

  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput( reader->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();

  return 0;
}

int main( int argc, char *argv[] )
{
   if ( argc < 6 )
     {
     std::cout << argv[0] << " imageDimension inputImage outputImage origin normal" << std::endl;
     exit( 1 );
     }

   switch( atoi( argv[1] ) )
    {
    case 2:
      ThresholdPlane<2>( argc, argv );
      break;
    case 3:
      ThresholdPlane<3>( argc, argv );
      break;
    default:
       std::cerr << "Unsupported dimension" << std::endl;
       exit( EXIT_FAILURE );
   }
}

