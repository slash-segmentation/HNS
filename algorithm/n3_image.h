#ifndef _n3_image_h_
#define _n3_image_h_

#include "n3_base.h"
#include "itkImage.h"
#include "itkRGBPixel.h"

namespace n3 {

  const unsigned int IMG_DIM = 2;
  const unsigned int IMG_DIM3 = 3;
  
  typedef itk::RGBPixel<UInt8> Rgb8;
  typedef itk::Image<Rgb8, IMG_DIM> Rgb8Image;
  typedef itk::Image<UInt8, IMG_DIM> UInt8Image;
  typedef itk::Image<UInt16, IMG_DIM> UInt16Image;
  typedef itk::Image<UInt32, IMG_DIM> UInt32Image;
  typedef itk::Image<Label, IMG_DIM> LabelImage;
  typedef itk::Image<Float, IMG_DIM> FloatImage;

  typedef itk::Size<IMG_DIM> ImageSize;
  typedef itk::Index<IMG_DIM> ImageIndex;
  typedef itk::ImageRegion<IMG_DIM> ImageRegion;

  typedef itk::Image<Label, IMG_DIM3> LabelImage3;
  typedef itk::Image<Float, IMG_DIM3> FloatImage3;
  typedef itk::Image<UInt8, IMG_DIM3> UInt8Image3;
  typedef itk::Image<UInt16, IMG_DIM3> UInt16Image3;

  typedef itk::Size<IMG_DIM3> ImageSize3;
  typedef itk::Index<IMG_DIM3> ImageIndex3;
  typedef itk::ImageRegion<IMG_DIM3> ImageRegion3;

};

#endif
