#ifndef _n3_vis_util_h_
#define _n3_vis_util_h_

#include "struct/point.h"
#include "image_util.h"
#include "itkComposeImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapOverlayImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"

namespace n3 {

  inline Rgb8 rgb (const char* color)
  {
    Rgb8 ret;
    if (strcmp(color, "black") == 0) ret.Set(0, 0, 0);
    else if (strcmp(color, "white") == 0) ret.Set(255, 255, 255);
    else if (strcmp(color, "red") == 0) ret.Set(255, 0, 0);
    else if (strcmp(color, "green") == 0) ret.Set(0, 255, 0);
    else if (strcmp(color, "blue") == 0) ret.Set(0, 0, 255);
    else if (strcmp(color, "yellow") == 0) ret.Set(255, 255, 0);
    else if (strcmp(color, "cyan") == 0) ret.Set(0, 255, 255);
    else if (strcmp(color, "magenta") == 0) ret.Set(255, 0, 255);    
    else if (strcmp(color, "gray") == 0) ret.Set(128, 128, 128);
    else if (strcmp(color, "silver") == 0) ret.Set(192, 192, 192);
    else if (strcmp(color, "maroon") == 0) ret.Set(128, 0, 0);
    else if (strcmp(color, "olive") == 0) ret.Set(128, 128, 0);
    else if (strcmp(color, "darkgreen") == 0) ret.Set(0, 128, 0);
    else if (strcmp(color, "purple") == 0) ret.Set(128, 0, 128);
    else if (strcmp(color, "teal") == 0) ret.Set(0, 128, 128);
    else if (strcmp(color, "navy") == 0) ret.Set(0, 0, 128);
    else perr("Error: unsupported color...");
    return ret;
  }


  template <typename TImage> 
    typename itk::ScalarToRGBColormapImageFilter<TImage, 
    Rgb8Image>::ColormapEnumType colormap (const char* name)
    {
      typename itk::ScalarToRGBColormapImageFilter<TImage, 
	Rgb8Image>::ColormapEnumType ret;
      if (strcmp(name, "overunder") == 0) 
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::OverUnder;
      else if (strcmp(name, "hsv") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::HSV;
      else if (strcmp(name, "jet") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Jet;
      else if (strcmp(name, "copper") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Copper;
      else if (strcmp(name, "winter") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Winter;
      else if (strcmp(name, "autumn") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Autumn;
      else if (strcmp(name, "summer") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Summer;
      else if (strcmp(name, "spring") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Spring;
      else if (strcmp(name, "cool") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Cool;
      else if (strcmp(name, "hot") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Hot;
      else if (strcmp(name, "gray") == 0 || strcmp(name, "grey")) 
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Grey;
      else if (strcmp(name, "blue") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Blue;
      else if (strcmp(name, "green") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Green;
      else if (strcmp(name, "red") == 0)
	ret = itk::ScalarToRGBColormapImageFilter<TImage, 
	  Rgb8Image>::Red;
      else perr("Error: unsupported colormap...");
      return ret;
    }


  // Map v to HSV color space
  // By default v is within [0, 1]
  template <typename T> 
    Rgb8 v2hsv (T v, T vmin = 0.0, T vmax = 1.0)
    {
      if (v < vmin) v = vmin;
      else if (v > vmax ) v = vmax;
      double x = ((double)v - (double)vmin) / 
	((double)vmax - (double)vmin) * 360.0;
      double maxi = 255.0, mini = 0.0;
      double posSlope = (maxi - mini) / 60.0;
      double negSlope = -posSlope;
      double r = maxi, g = maxi, b = maxi;
      if (x < 60.0) {
	g = posSlope * x + mini;
	b = mini;
      }
      else if (x < 120.0) {
	r = negSlope * x + 2.0 * maxi + mini;
	b = mini;
      }
      else if (x < 180.0) {
	r = mini;
	b = posSlope * x - 2.0 * maxi + mini;
      }
      else if (x < 240.0) {
	r = mini;
	g = negSlope * x + 4.0 * maxi + mini;
      }
      else if (x < 300.0) {
	r = posSlope * x - 4.0 * maxi + mini;
	g = mini;
      }
      else {
	g = mini;
	b = negSlope * x + 6.0 * maxi;
      }
      Rgb8 ret;
      ret.Set((Rgb8::ComponentType)r, (Rgb8::ComponentType)g, 
	      (Rgb8::ComponentType)b);
      return ret;
    }


  // Map v to Jet color space
  // By default v is within [0, 1]
  template <typename T>
    Rgb8 v2jet (T v, T vmin = 0.0, T vmax = 1.0)
    {
      if (v < vmin) v = vmin;
      else if (v > vmax) v = vmax;
      double maxi = 255.0, mini = 0.0;
      double x = 2.0 * ((double)v - (double)vmin) / 
	((double)vmax - (double)vmin) - 1.0;
      double xmin = -1.0, xmax = 1.0;
      double dx = xmax - xmin;
      double r = 1.0, g = 1.0, b = 1.0;
      if (x < xmin + 0.25 * dx) {
	r = 0.0;
	g = 4.0 * (x - xmin) / dx;	
      }
      else if (x < xmin + 0.5 * dx) {
	r = 0.0;
	b = 1.0 + 4.0 * (xmin + 0.25 * dx - x) / dx;	
      }
      else if (x < xmin + 0.75 * dx) {
	r = 4.0 * (x - xmin - 0.5 * dx) / dx;
	b = 0.0;	
      }
      else {
	g = 1.0 + 4.0 * (xmin + 0.75 * dx - x) / dx;
	b = 0.0;
      }
      double d = maxi - mini;
      Rgb8 ret;
      ret.Set((Rgb8::ComponentType)(r * d + mini), 
	      (Rgb8::ComponentType)(g * d + mini), 
	      (Rgb8::ComponentType)(b * d + mini));
      return ret;
    }



  template <typename TImage> 
    void copy (typename TImage::Pointer tarImage, 
	       typename TImage::Pointer srcImage, 
	       std::list<Point> const& points)
    {
      for (std::list<Point>::const_iterator it = points.begin(); 
	   it != points.end(); ++it) {
	ImageIndex idx = p2i(*it);
	tarImage->SetPixel(idx, srcImage->GetPixel(idx));
      }
    }


  
  template <typename TImage>
    void drawLine (typename TImage::Pointer canvas, Point const& start, 
		   Point const& end, typename TImage::PixelType val)
    {
      double dx = end.x - start.x, dy = end.y - start.y;      
      if (fabs(dx) < 1.0 && fabs(dy) < 1.0) return;
      if (fabs(dx) >= fabs(dy)) { /* Draw along x axis */
	double k = dy / dx;
	double b = start.y - start.x * k;
	double xinc = dx > 0.0? 1.0: -1.0;
	double x = start.x;
	while ((x - start.x) * (x - end.x) <= 0) {
	  double y = k * x + b;
	  setv<TImage>(canvas, (int)(x + 0.5), (int)(y + 0.5), val);
	  x += xinc;
	}
      }
      else { /* Draw along y axis */
	double k = dx / dy;
	double b = start.x - start.y * k;
	double yinc = dy > 0.0? 1.0: -1.0;
	double y = start.y;
	while ((y - start.y) * (y - end.y) <= 0) {
	  double x = k * y + b;
	  setv<TImage>(canvas, (int)(x + 0.5), (int)(y + 0.5), val);
	  y += yinc;
	}
      }
    }



  template <typename TImage>
    void drawCurve (typename TImage::Pointer canvas, Points const& corners, 
		    bool closed, typename TImage::PixelType val)
    {
      if (corners.size() < 2) return;
      Points::const_iterator pit0 = corners.begin();
      Points::const_iterator pit1 = pit0; ++pit1;
      while (pit1 != corners.end()) {
	drawLine<TImage>(canvas, *pit0, *pit1, val);
	pit0 = pit1;
	++pit1;
      }
      if (closed) drawLine<TImage>(canvas, *pit0, corners.front(), val);
    }


  template <typename TImage>
    Rgb8Image::Pointer applyColormap 
    (typename TImage::Pointer scalarImage, 
     typename itk::ScalarToRGBColormapImageFilter<TImage, 
     Rgb8Image>::ColormapEnumType colormap, bool useExtremaForScaling)
    {
      typedef itk::ScalarToRGBColormapImageFilter<TImage, Rgb8Image> C;
      typename C::Pointer c = C::New();
      c->SetInput(scalarImage);
      c->SetColormap(colormap);
      c->SetUseInputImageExtremaForScaling(useExtremaForScaling);
      c->Update();
      return c->GetOutput();
    }


  template <typename TImage>
    Rgb8Image::Pointer randomColor (typename TImage::Pointer scalarImage, 
				    const char* bgFillColor = NULL)
    {
      Rgb8 bg = bgFillColor == NULL? rgb("black"): rgb(bgFillColor);
      Rgb8Image::Pointer ret = 
	createImage<TImage, Rgb8Image>(scalarImage, bg);
      std::map<typename TImage::PixelType, Rgb8> ctab;
      srand(time(NULL));
      for (itk::ImageRegionConstIteratorWithIndex<TImage> 
	     it(scalarImage, scalarImage->GetLargestPossibleRegion()); 
	   !it.IsAtEnd(); ++it) {
	if (bgFillColor != NULL && it.Get() == BGVAL) 
	  ret->SetPixel(it.GetIndex(), bg);
	else {
	  typename std::map<typename TImage::PixelType, 
	    Rgb8>::const_iterator tit =  ctab.find(it.Get());	
	  if (tit == ctab.end()) {
	    Rgb8 pixel = v2hsv(rand(), 0, RAND_MAX);
	    ctab[it.Get()] = pixel;
	    ret->SetPixel(it.GetIndex(), pixel);	  
	  }
	  else ret->SetPixel(it.GetIndex(), tit->second);
	}
      }
      return ret;
    }



  // Create RGB image from gray image
  Rgb8Image::Pointer gray2rgb (UInt8Image::Pointer im);


  // Paint points to image with given color and opacity
  void paint (Rgb8Image::Pointer im, Points const& p, 
	      Rgb8 const& fgPixel, double bgPortion);


  // Color label image and overlay with gray background image
  // bgFillColor should be a valid color name
  // If bgFillColor == NULL, do not fill bg pixels with any color
  Rgb8Image::Pointer overlay (LabelImage::Pointer labelImage, 
			      UInt8Image::Pointer bgImage, 
			      double opacity, 
			      const char* bgFillColor = NULL);

};

#endif
