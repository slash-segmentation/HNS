#include "vis_util.h"
using namespace n3;


// Create RGB image from gray image
Rgb8Image::Pointer n3::gray2rgb (UInt8Image::Pointer im)
{      
  typedef itk::ComposeImageFilter<UInt8Image, Rgb8Image> C;
  C::Pointer c = C::New();
  c->SetInput1(im);
  c->SetInput2(im);
  c->SetInput3(im);
  c->Update();
  return c->GetOutput();
}



// Paint points to image with given color and opacity
void n3::paint (Rgb8Image::Pointer im, Points const& p, 
		Rgb8 const& fgPixel, double bgPortion)
{
  double fgPortion = 1.0 - bgPortion;
  for (Points::const_iterator it = p.begin(); it != p.end(); ++it) {
    ImageIndex i = p2i(*it);
    Rgb8 bgPixel = im->GetPixel(i);
    Rgb8 pixel;
    pixel.Set((Rgb8::ComponentType)(bgPixel.GetRed() * bgPortion + 
				    fgPixel.GetRed() * fgPortion), 
	      (Rgb8::ComponentType)(bgPixel.GetGreen() * bgPortion + 
				    fgPixel.GetGreen() * fgPortion), 
	      (Rgb8::ComponentType)(bgPixel.GetBlue() * bgPortion + 
				    fgPixel.GetBlue() * fgPortion));
    im->SetPixel(i, pixel);
  }
}



// Color label image and overlay with gray background image
// If fillBG, fill bg pixel on label image with black on output image
Rgb8Image::Pointer n3::overlay (LabelImage::Pointer labelImage, 
				UInt8Image::Pointer bgImage, 
				double opacity, const char* bgFillColor)
{
  typedef itk::LabelImageToLabelMapFilter<LabelImage> C;
  C::Pointer c = C::New();
  c->SetInput(labelImage);
  c->Update();
  typedef itk::LabelMapOverlayImageFilter<C::OutputImageType, 
					  UInt8Image, Rgb8Image> O;
  O::Pointer o = O::New();
  o->SetInput(c->GetOutput());
  o->SetFeatureImage(bgImage);
  o->SetOpacity(opacity);
  o->Update();
  Rgb8Image::Pointer ret = o->GetOutput();
  if (bgFillColor != NULL) {
    Rgb8 bgPixel = rgb(bgFillColor);
    for (itk::ImageRegionConstIteratorWithIndex<LabelImage> 
	   it(labelImage, labelImage->GetLargestPossibleRegion()); 
	 !it.IsAtEnd(); ++it) 
      if (it.Get() == BGVAL) 
	setv<Rgb8Image>(ret, it.GetIndex()[0], it.GetIndex()[1], 
			bgPixel);
  }
  return ret;
}
