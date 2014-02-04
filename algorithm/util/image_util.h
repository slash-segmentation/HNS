#ifndef _n3_image_util_h_
#define _n3_image_util_h_

#include "n3_image.h"
#include "struct/point3.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkTileImageFilter.h"
#include "itkMirrorPadImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCropImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkScalarConnectedComponentImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMorphologicalWatershedImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkGaussianBlurImageFunction.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkGrayscaleFillholeImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace n3 {

  /* Image index to point */
  inline Point i2p (ImageIndex const& i) {return Point(i[0], i[1]);}



  /* Image index to 3d point */
  inline Point3 i2p (ImageIndex3 const& i) {
    return Point3(i[0], i[1], i[2]); 
  }



  /* Point to image index */
  inline ImageIndex p2i (Point const& p) {
    ImageIndex i;
    i[0] = p.x;
    i[1] = p.y;
    return i;
  }



  /* 3d point to image index */
  inline ImageIndex3 p2i (Point3 const& p) {
    ImageIndex3 i;
    i[0] = p.x;
    i[1] = p.y;
    i[2] = p.z;
    return i;
  }



  /* Determine if a point is on image borders */
  inline bool isborder (Point const& p, int width, int height)
  {
    return (p.x <= 0 || p.x >= width - 1 || 
	    p.y <= 0 || p.y >= height - 1);
  }



  /* Determine if a 3d point is on image volume borders */
  inline bool isborder (Point3 const& p, int width, int height, int depth) 
  {
    return (p.x <= 0 || p.x >= width - 1 || 
	    p.y <= 0 || p.y >= height - 1 || 
	    p.z <= 0 || p.z >= depth - 1); 
  }



  template <typename TImage> typename TImage::PixelType 
    getv (typename TImage::Pointer im, float x, float y)
    {
      ImageIndex i;
      i[0] = (int)(x + 0.5);
      i[1] = (int)(y + 0.5);
      return im->GetPixel(i);
    }



  template <typename TImage> typename TImage::PixelType 
    getv (typename TImage::Pointer im, Point const& p)
    {
      ImageIndex i;
      i[0] = (int)(p.x + 0.5);
      i[1] = (int)(p.y + 0.5);
      return im->GetPixel(i);
    }



  template <typename TImage> typename TImage::PixelType 
    getv (typename TImage::Pointer im, float x, float y, float z)
    {
      ImageIndex3 i;
      i[0] = (int)(x + 0.5);
      i[1] = (int)(y + 0.5);
      i[2] = (int)(z + 0.5);
      return im->GetPixel(i);
    }



  template <typename TImage> typename TImage::PixelType 
    getv (typename TImage::Pointer im, Point3 const& p)
    {
      ImageIndex3 i;
      i[0] = (int)(p.x + 0.5);
      i[1] = (int)(p.y + 0.5);
      i[2] = (int)(p.z + 0.5);
      return im->GetPixel(i);
    }



  template <typename TImage> void 
    setv (typename TImage::Pointer im, float x, float y, 
	  typename TImage::PixelType const& v) 
    {
      ImageIndex i;
      i[0] = (int)(x + 0.5);
      i[1] = (int)(y + 0.5);
      im->SetPixel(i, v);
    }



  template <typename TImage> void 
    setv (typename TImage::Pointer im, Point const& p, 
	  typename TImage::PixelType const& v) 
    {
      ImageIndex i;
      i[0] = (int)(p.x + 0.5);
      i[1] = (int)(p.y + 0.5);
      im->SetPixel(i, v);
    }



  template <typename TImage> void 
    setv (typename TImage::Pointer im, float x, float y, float z, 
	  typename TImage::PixelType const& v)
    {
      ImageIndex3 i;
      i[0] = (int)(x + 0.5);
      i[1] = (int)(y + 0.5);
      i[2] = (int)(z + 0.5);
      im->SetPixel(i, v);
    }



  template <typename TImage> void 
    setv (typename TImage::Pointer im, Point3 const& p, 
	  typename TImage::PixelType const& v)
    {
      ImageIndex3 i;
      i[0] = (int)(p.x + 0.5);
      i[1] = (int)(p.y + 0.5);
      i[2] = (int)(p.z + 0.5);
      im->SetPixel(i, v);
    }



  template <typename TImage> void 
    getvs (std::list<typename TImage::PixelType>& vals, 
	   typename TImage::Pointer im, Points const& points)
    {
      for (Points::const_iterator it = points.begin(); it != points.end(); 
	   ++it) vals.push_back(getv<TImage>(im, it->x, it->y));
    }



  template <typename TImage> void 
    getvs (std::list<typename TImage::PixelType>& vals, 
	   typename TImage::Pointer im, Points3 const& points)
    {
      for (Points3::const_iterator it = points.begin(); 
	   it != points.end(); ++it) 
	vals.push_back(getv<TImage>(im, it->x, it->y, it->z));
    }



  template <typename TImage> void 
    setvs (typename TImage::Pointer im, Points const& points, 
	   typename TImage::PixelType val)
    {
      for (Points::const_iterator it = points.begin(); 
	   it != points.end(); ++it) im->SetPixel(p2i(*it), val);
    }



  template <typename TImage> void 
    setvs (typename TImage::Pointer im, Points3 const& points, 
	   typename TImage::PixelType val)
    {
      for (Points3::const_iterator it = points.begin(); 
	   it != points.end(); ++it) im->SetPixel(p2i(*it), val);
    }



  template <typename TImage> int 
    getw (typename TImage::Pointer im)
    {
      return im->GetLargestPossibleRegion().GetSize()[0];
    }



  template <typename TImage> int 
    geth (typename TImage::Pointer im) 
    {
      return im->GetLargestPossibleRegion().GetSize()[1];
    }



  template <typename TImage> int 
    getd (typename TImage::Pointer im) 
    {
      return im->GetLargestPossibleRegion().GetSize()[2];
    }



  template <typename TImage> typename TImage::Pointer
    readImage (const char* name) 
    {
      typedef itk::ImageFileReader<TImage> R;
      typename R::Pointer r = R::New();
      r->SetFileName(name);
      r->Update();
      return r->GetOutput();
    }



  template <typename TImage> typename TImage::Pointer 
    readImage (std::vector<const char*> const& names) 
    {
      std::vector<std::string> ns;
      ns.reserve(names.size());
      for (std::vector<const char*>::const_iterator nit = names.begin(); 
	   nit != names.end(); ++nit) ns.push_back(std::string(*nit));
      typedef itk::ImageSeriesReader<TImage> R;
      typename R::Pointer r = R::New();
      r->SetFileNames(ns);
      r->Update();
      return r->GetOutput();
    }



  template <typename TImage> void 
    writeImage (const char* name, typename TImage::Pointer im) 
    {
      typedef itk::ImageFileWriter<TImage> W;
      typename W::Pointer w = W::New();
      w->SetInput(im);
      w->SetFileName(name);
      w->Update();
    }



  template <typename TImageIn, typename TImageOut> void 
    writeImage (std::vector<const char*> const& names, 
		typename TImageIn::Pointer im) 
  {
    std::vector<std::string> ns;
    ns.reserve(names.size());
    for (std::vector<const char*>::const_iterator nit = names.begin(); 
	 nit != names.end(); ++nit) ns.push_back(std::string(*nit));
    typedef itk::ImageSeriesWriter<TImageIn, TImageOut> W;
    typename W::Pointer w = W::New();
    w->SetFileNames(ns);
    w->SetInput(im);
    w->Update();
  }



  inline ImageIndex getImageIndex (int x, int y)
  {
    ImageIndex i;
    i[0] = x;
    i[1] = y;
    return i;
  }


  
  inline ImageIndex3 getImageIndex (int x, int y, int z)
  {
    ImageIndex3 i;
    i[0] = x;
    i[1] = y;
    i[2] = z;
    return i;
  }



  inline ImageSize getImageSize (int w, int h)
  {
    ImageSize s;
    s[0] = w;
    s[1] = h;
    return s;
  }


  inline ImageSize3 getImageSize (int w, int h, int d)
  {
    ImageSize3 s;
    s[0] = w;
    s[1] = h;
    s[2] = d;
    return s;
  }



  inline ImageRegion getImageRegion (int w, int h, int top, int left)
  {
    ImageIndex i;
    i[0] = top;
    i[1] = left;
    ImageSize s;
    s[0] = w;
    s[1] = h;
    return ImageRegion(i, s);
  }


  inline ImageRegion getImageRegion (Box const& b)
  {
    return getImageRegion(b.width, b.height, b.start.x, b.start.y);
  }


  inline ImageRegion3 getImageRegion (int w0, int w1, int w2, 
				      int x0, int x1, int x2)
  {
    ImageIndex3 i;
    i[0] = x0;
    i[1] = x1;
    i[2] = x2;
    ImageSize3 s;
    s[0] = w0;
    s[1] = w1;
    s[2] = w2;
    return ImageRegion3(i, s);
  }


  inline ImageRegion3 getImageRegion (ImageRegion const& r, 
				      int w2, int x2)
  {
    ImageIndex3 i;
    i[0] = r.GetIndex()[0];
    i[1] = r.GetIndex()[1];
    i[2] = x2;
    ImageSize3 s;
    s[0] = r.GetSize()[0];
    s[1] = r.GetSize()[1];
    s[2] = w2;
    return ImageRegion3(i, s);
  }


  inline ImageRegion3 getImageRegion (Box3 const& b)
  {
    return getImageRegion(b.width, b.height, b.deep, 
			  b.start.x, b.start.y, b.start.z);
  }


  template <typename TImage> typename TImage::Pointer
    createImage (typename TImage::RegionType const& r, 
		 typename TImage::PixelType fill) 
    {
      typename TImage::Pointer ret = TImage::New();
      ret->SetRegions(r);
      ret->Allocate();
      ret->FillBuffer(fill);
      return ret;
    }



  template <typename TImage> typename TImage::Pointer
    createImage (int w, int h, typename TImage::PixelType fill)
    {
      return createImage<TImage>(getImageRegion(w, h, 0, 0), fill);
    }



  template <typename TImage> typename TImage::Pointer
    createImage (int w, int h, int d, typename TImage::PixelType fill)
    {
      return createImage<TImage>(getImageRegion(w, h, d, 0, 0, 0), fill);
    }



  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer
    createImage (typename TImageIn::Pointer im, 
		 typename TImageOut::PixelType fill = BGVAL)
    {
      return 
	createImage<TImageOut>(im->GetLargestPossibleRegion(), fill);
    }



  template <typename TImage> typename TImage::Pointer 
    createImage (typename TImage::Pointer im, 
		 typename TImage::PixelType fill = BGVAL)
    {
      return createImage<TImage>(im->GetLargestPossibleRegion(), fill);
    }



  template <typename TImage> typename TImage::Pointer 
    copyImage (typename TImage::Pointer im)
    {
      typename TImage::Pointer ret = createImage<TImage>(im);
      itk::ImageRegionConstIterator<TImage> 
	iit(im, im->GetLargestPossibleRegion());
      itk::ImageRegionIterator<TImage> 
	rit(ret, ret->GetLargestPossibleRegion());
      while (!iit.IsAtEnd()) {
	rit.Set(iit.Get());
	++iit;
	++rit;
      }
      return ret;
    }



  /* Return image of given slice; read if none */
  template <typename TImage> typename TImage::Pointer 
    getImage (std::map<int, typename TImage::Pointer>& images, 
	      std::map<int, const char*> const& imageNames, int slice)
    {
      typename std::map<int, typename TImage::Pointer>::const_iterator 
	iit = images.find(slice);
      if (iit != images.end()) return iit->second;
      const char* name = imageNames.find(slice)->second;
      typename TImage::Pointer ret = readImage<TImage>(name);
      images[slice] = ret;
      return ret;
    }



  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    stackImages (std::vector<typename TImageIn::Pointer> const& images)
  {
    ImageRegion r2 = images[0]->GetLargestPossibleRegion();
    ImageRegion3 r3 = getImageRegion(r2, images.size(), 0);
    typename TImageOut::Pointer ret = createImage<TImageOut>(r3, BGVAL);
    itk::ImageRegionIterator<TImageOut> 
      oit(ret, ret->GetLargestPossibleRegion());
    for (typename std::vector<typename TImageIn::Pointer>::const_iterator 
	   it = images.begin(); it != images.end(); ++it) {
      for (itk::ImageRegionConstIterator<TImageIn> 
	     iit(*it, (*it)->GetLargestPossibleRegion()); 
	   !iit.IsAtEnd(); ++iit) {
	oit.Set(iit.Get());
	++oit;
      }
    }
    return ret;
  }


  
  template <typename TImageIn, typename TImageOut> void 
    unstackImages (std::vector<typename TImageOut::Pointer>& outImages, 
		   typename TImageIn::Pointer inImage)
  {
    ImageRegion3 r3 = inImage->GetLargestPossibleRegion();
    int w = r3.GetSize()[0], h = r3.GetSize()[1];
    int sliceNum = r3.GetSize()[2];
    outImages.reserve(sliceNum);
    itk::ImageRegionConstIterator<TImageIn> 
      iit(inImage, inImage->GetLargestPossibleRegion());
    for (int i = 0; i < sliceNum; ++i) {
      outImages.insert(outImages.end(), 
		       createImage<TImageOut>(w, h, BGVAL));
      itk::ImageRegionIterator<TImageOut> 
	oit(outImages.back(), 
	    outImages.back()->GetLargestPossibleRegion());
      while (!oit.IsAtEnd()) {
	oit.Set(iit.Get());
	++oit;
	++iit;
      }
    }
  }



  /* Mirror image padding */
  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    padImage (typename TImageIn::Pointer im, 
	      typename TImageIn::SizeType const& upperInc, 
	      typename TImageIn::SizeType const& lowerInc)
  {
    typedef itk::MirrorPadImageFilter<TImageIn, TImageOut> P;
    typename P::Pointer p = P::New();
    p->SetInput(im);
    p->SetPadUpperBound(upperInc);
    p->SetPadLowerBound(lowerInc);
    p->Update();
    return p->GetOutput();
  }



  /* Constant image padding */
  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    padImage (typename TImageIn::Pointer im, 
	      typename TImageIn::SizeType const& upperInc, 
	      typename TImageIn::SizeType const& lowerInc, 
	      typename TImageOut::PixelType const& v)
  {
    typedef itk::ConstantPadImageFilter<TImageIn, TImageOut> P;
    typename P::Pointer p = P::New();
    p->SetInput(im);
    p->SetPadUpperBound(upperInc);
    p->SetPadLowerBound(lowerInc);
    p->SetConstant(v);
    p->Update();
    return p->GetOutput();
  }



  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    cropImage (typename TImageIn::Pointer im, 
	       typename TImageIn::RegionType const& region)
  {
    typedef itk::ExtractImageFilter<TImageIn, TImageOut> C;
    typename C::Pointer c = C::New();
    c->SetInput(im);
    c->SetExtractionRegion(region);
    c->SetDirectionCollapseToIdentity();
    c->Update();
    return c->GetOutput();
  }



  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    relabelImage (typename TImageIn::Pointer im, int minSize = 0) 
  {
    typedef itk::RelabelComponentImageFilter<TImageIn, TImageOut> R;
    typename R::Pointer r = R::New();
    r->SetInput(im);
    if (minSize > 0) r->SetMinimumObjectSize(minSize);
    r->Update();
    return r->GetOutput();
  }



  /* Only when isBinary == false, diffThreshold is used */
  template <typename TImageIn, typename TImageOut>
    typename TImageOut::Pointer 
    labelConnectedComponents (typename TImageIn::Pointer im,
			      typename TImageIn::PixelType bgVal = BGVAL, 
			      bool isFullyConnected = false, 
			      bool isLabelImage = true, 
			      typename TImageIn::PixelType 
			      diffThreshold = 0)
  {
    if (isLabelImage) {
      typedef itk::ConnectedComponentImageFilter<TImageIn, TImageOut> C;
      typename C::Pointer c = C::New();
      c->SetBackgroundValue(bgVal);
      c->SetInput(im);
      c->SetFullyConnected(isFullyConnected);
      c->Update();
      return c->GetOutput();
    }
    else {
      typedef 
	itk::ScalarConnectedComponentImageFilter<TImageIn, TImageOut> C;
      typename C::Pointer c = C::New();
      c->SetDistanceThreshold(diffThreshold);
      c->SetBackgroundValue(bgVal);
      c->SetInput(im);
      c->SetFullyConnected(isFullyConnected);
      c->Update();
      return c->GetOutput();
    }
  }



  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    castImage (typename TImageIn::Pointer im) 
  {
    typedef itk::CastImageFilter<TImageIn, TImageOut> C;
    typename C::Pointer c = C::New();
    c->SetInput(im);
    c->Update();
    return c->GetOutput();
  }


  template <typename TImageIn, typename TImageOut>
    typename TImageOut::Pointer 
    watershed (typename TImageIn::Pointer im, 
	       typename TImageIn::PixelType seedThreshold, 
	       bool keepWatershedLine = true, 
	       bool isFullyConnected = true)
  {
    typedef itk::MorphologicalWatershedImageFilter<TImageIn, TImageOut> W;
    typename W::Pointer w = W::New();
    w->SetLevel(seedThreshold);
    w->SetInput(im);
    w->SetMarkWatershedLine(keepWatershedLine);
    w->SetFullyConnected(isFullyConnected);
    w->Update();
    return w->GetOutput();
  }



  template <typename TImage> typename TImage::Pointer 
    rescaleImage (typename TImage::Pointer im, 
		  typename TImage::PixelType omin, 
		  typename TImage::PixelType omax)
  {
    typedef itk::RescaleIntensityImageFilter<TImage> R;
    typename R::Pointer r = R::New();
    r->SetInput(im);
    r->SetOutputMinimum(omin);
    r->SetOutputMaximum(omax);
    r->Update();
    return r->GetOutput();
  }



  template <typename TImageIn, typename TImageOut> 
    typename TImageIn::Pointer
    multiplyImage (typename TImageIn::Pointer im, Float r)
    {
      typedef itk::MultiplyImageFilter<TImageIn, 
	FloatImage, TImageOut> M;
      typename M::Pointer m = M::New();
      m->SetInput(im);
      m->SetConstant(r);
      m->Update();
      return m->GetOutput();
    }



  /* If kernelWidth < 0 use (int)(3 * sigma + 1) */
  template <typename TImage> typename TImage::Pointer 
    blurImage (typename TImage::Pointer im, double sigma, 
	       int kernelWidth = -1)
    {
      if (kernelWidth < 0) kernelWidth = (int)(3.0 * sigma + 1.0);
      typedef itk::GaussianBlurImageFunction<TImage> G;
      typename G::Pointer g = G::New();
      g->SetInputImage(im);
      g->SetSigma(sigma);
      g->SetMaximumKernelWidth(kernelWidth);
      typename G::ErrorArrayType ea;
      ea.Fill(0.01);
      g->SetMaximumError(ea);
      typename TImage::Pointer ret = createImage<TImage>(im, 0);
      for (itk::ImageRegionIterator<TImage> 
	     it(ret, ret->GetLargestPossibleRegion()); !it.IsAtEnd(); 
	   ++it) it.Set(g->EvaluateAtIndex(it.GetIndex()));
      return ret;
    }


  template <typename TImage> typename TImage::Pointer
    invertImage (typename TImage::Pointer im, 
		 typename TImage::PixelType max)
    {
      typedef itk::InvertIntensityImageFilter<FloatImage> II;
      II::Pointer ii = II::New();
      ii->SetInput(im);
      ii->SetMaximum(max);
      ii->Update();
      return ii->GetOutput();
    }


  template <typename TImage> typename TImage::PixelType 
    getMax (typename TImage::Pointer im, Point* pos = NULL) 
    {
      typedef itk::MinimumMaximumImageCalculator<TImage> C;
      typename C::Pointer c = C::New();
      c->SetImage(im);
      c->ComputeMaximum();
      if (pos != NULL) *pos = i2p(c->GetIndexOfMaximum());
      return c->GetMaximum();
    }


  template <typename TImage> typename TImage::PixelType 
    getMin (typename TImage::Pointer im, Point* pos = NULL) 
    {
      typedef itk::MinimumMaximumImageCalculator<TImage> C;
      typename C::Pointer c = C::New();
      c->SetImage(im);
      c->ComputeMinimum();
      if (pos != NULL) *pos = i2p(c->GetIndexOfMinimum());
      return c->GetMinimum();
    }


  template <typename TImage> int 
    getUniqueValues (std::set<typename TImage::PixelType>& vals, 
		     typename TImage::Pointer im) 
    {
      for (itk::ImageRegionConstIterator<TImage> 
	     iit(im, im->GetLargestPossibleRegion()); !iit.IsAtEnd(); 
	   ++iit)
	vals.insert(iit.Get());
    }


  /* Inside pixel: lowerThreshold <= intensity <= upperThreshold */
  template <typename TImageIn, typename TImageOut> 
    typename TImageOut::Pointer 
    thresholdImage (typename TImageIn::Pointer inputImage, 
		    typename TImageIn::PixelType lowerThreshold, 
		    typename TImageIn::PixelType upperThreshold, 
		    typename TImageOut::PixelType insideValue, 
		    typename TImageOut::PixelType outsideValue)
  {
    typedef itk::BinaryThresholdImageFilter<TImageIn, TImageOut> B;
    typename B::Pointer b = B::New();
    b->SetInput(inputImage);
    b->SetLowerThreshold(lowerThreshold);
    b->SetUpperThreshold(upperThreshold);
    b->SetInsideValue(insideValue);
    b->SetOutsideValue(outsideValue);
    b->Update();
    return b->GetOutput();
  }


  template <typename TImageIn, typename TImageOut>
    typename TImageOut::Pointer
    fillHoles (typename TImageIn::Pointer inputImage,
  	       bool fullyConnected = false)
  {
    typedef itk::GrayscaleFillholeImageFilter<TImageIn, TImageOut> F;
    typename F::Pointer f = F::New();
    f->SetInput(inputImage);
    f->SetFullyConnected(fullyConnected);
    f->Update();
    return f->GetOutput();
  }


  /* Connect should be 4 or 8 */
  /* If returned vector is not full, point must be on image border */
  template <typename TImage> 
    std::vector<Pixel<typename TImage::PixelType> > 
    getNeighbors (Point const& p, typename TImage::Pointer im, 
		  int connect)
    {
      int w = getw<TImage>(im), h = geth<TImage>(im);
      int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
      int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
      typedef Pixel<typename TImage::PixelType> TPixel;
      std::vector<TPixel> ret;
      int inc = (connect == 8? 1: 2);
      ret.reserve(connect == 8? 8: 4);
      for (int i = 0; i < 8; i += inc) {
	int x = p.x + dx[i], y = p.y + dy[i];
	if (x >= 0 && x < w && y >= 0 && y < h) 
	  ret.push_back(TPixel(x, y, getv<TImage>(im, x, y)));
      }
      return ret;
    }



  /* Connect should be 6, 18 or 26 */
  template <typename TImage>
    std::vector<Pixel3<typename TImage::PixelType> >
    getNeighbors (Point3 const& p, typename TImage::Pointer im, 
		  int connect) 
    {
      int w = getw<TImage>(im), h = geth<TImage>(im), d = getd<TImage>(im);
      int dx[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 
		  0, 0, 0, 0, 0, 0, 0, 0, 
		  1, 1, 1, 1, 1, 1, 1, 1, 1};
      int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 0, 1, 1, 1};
      int dz[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, 
		  -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1};
      std::vector<int> indices;
      indices.reserve(connect);
      if (connect == 6) {
	int is[] = {4, 10, 12, 13, 15, 21};
	indices.insert(indices.end(), is, is + 6);
      }
      else if (connect == 18) {
	int is[] = {1, 3, 4, 5, 7, 9, 10, 11, 12, 
		    13, 14, 15, 16, 18, 20, 21, 22, 24};
	indices.insert(indices.end(), is, is + 18);	
      }
      else for (int i = 0; i < 26; ++i) indices.push_back(i);
      typedef Pixel3<typename TImage::PixelType> TPixel;
      std::vector<TPixel> ret;
      ret.reserve(connect);
      for (std::vector<int>::const_iterator iit = indices.begin(); 
	   iit != indices.end(); ++iit) {
	int x = p.x + dx[*iit], y = p.y + dy[*iit], z = p.z + dz[*iit];
	if (x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d) 
	  ret.push_back(TPixel(x, y, z, getv<TImage>(im, x, y, z)));
      }
      return ret;
    }



  /* Get neighbor pixels with certain pixel value */
  /* Connect should be 6, 18 or 26 */
  template <typename TImage> void 
    getNeighbors (Points3& nb, Point3 const& p, 
		  typename TImage::Pointer im, int connect, 
		  typename TImage::PixelType val) 
    {
      int w = getw<TImage>(im), h = geth<TImage>(im), d = getd<TImage>(im);
      int dx[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 
		  0, 0, 0, 0, 0, 0, 0, 0, 
		  1, 1, 1, 1, 1, 1, 1, 1, 1};
      int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 0, 1, 1, 1};
      int dz[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, 
		  -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1};
      std::vector<int> indices;
      indices.reserve(connect);
      if (connect == 6) {
	int is[] = {4, 10, 12, 13, 15, 21};
	indices.insert(indices.end(), is, is + 6);
      }
      else if (connect == 18) {
	int is[] = {1, 3, 4, 5, 7, 9, 10, 11, 12, 
		    13, 14, 15, 16, 18, 20, 21, 22, 24};
	indices.insert(indices.end(), is, is + 18);	
      }
      else for (int i = 0; i < 26; ++i) indices.push_back(i);
      for (std::vector<int>::const_iterator iit = indices.begin(); 
	   iit != indices.end(); ++iit) {
	int x = p.x + dx[*iit], y = p.y + dy[*iit], z = p.z + dz[*iit];
	if (x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d 
	    && getv<TImage>(im, x, y, z) == val) 
	  nb.push_back(Point3(x, y, z));
      }
    }



  /* Connect should be 4 or 8 */
  template <typename TImage> void
    getNeighbors (std::set<typename TImage::PixelType>& nb, 
		  Point const& p, typename TImage::Pointer im, 
		  int connect) 
    {
      int w = getw<TImage>(im), h = geth<TImage>(im);
      int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
      int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
      typedef Pixel<typename TImage::PixelType> TPixel;
      int inc = (connect == 8? 1: 2);
      for (int i = 0; i < 8; i += inc) {
	int x = p.x + dx[i], y = p.y + dy[i];
	if (x >= 0 && x < w && y >= 0 && y < h) 
	  nb.insert(getv<TImage>(im, x, y));
      }
    }



  /* Connect should be 6, 18 or 26 */
  template <typename TImage> void 
    getNeighbors (std::set<typename TImage::PixelType>& nb, 
		  Point3 const& p, typename TImage::Pointer im, 
		  int connect) 
    {
      int w = getw<TImage>(im), h = geth<TImage>(im), 
	d = getd<TImage>(im);
      int dx[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 
		  0, 0, 0, 0, 0, 0, 0, 0, 
		  1, 1, 1, 1, 1, 1, 1, 1, 1};
      int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 1, 1, 1, 
		  -1, -1, -1, 0, 0, 0, 1, 1, 1};
      int dz[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, 
		  -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1};
      std::vector<int> indices;
      indices.reserve(connect);
      if (connect == 6) {
	int is[] = {4, 10, 12, 13, 15, 21};
	indices.insert(indices.end(), is, is + 6);
      }
      else if (connect == 18) {
	int is[] = {1, 3, 4, 5, 7, 9, 10, 11, 12, 
		    13, 14, 15, 16, 18, 20, 21, 22, 24};
	indices.insert(indices.end(), is, is + 18);	
      }
      else for (int i = 0; i < 26; ++i) indices.push_back(i);
      typedef Pixel3<typename TImage::PixelType> TPixel;
      for (std::vector<int>::const_iterator iit = indices.begin(); 
	   iit != indices.end(); ++iit) {
	int x = p.x + dx[*iit], y = p.y + dy[*iit], z = p.z + dz[*iit];
	if (x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d) 
	  nb.insert(getv<TImage>(im, x, y, z));
      }
    }


  
  /* Remove pixels with certain value */
  /* That are exclusively between two regions with same label */
  template <typename TImage> void 
    removeExclusiveGaps (typename TImage::Pointer image, 
			 typename TImage::PixelType bgVal, int connect, 
			 bool recursive = false)
    {
      bool changed = false;
      for (itk::ImageRegionIteratorWithIndex<TImage> 
	     iit(image, image->GetLargestPossibleRegion()); 
	   !iit.IsAtEnd(); ++iit)
	if (iit.Get() == bgVal) {
	  std::set<typename TImage::PixelType> nb;
	  getNeighbors<TImage>(nb, i2p(iit.GetIndex()), image, connect);
	  nb.erase(bgVal);
	  if (nb.size() == 1) {
	    setv<TImage>(image, i2p(iit.GetIndex()), *(nb.begin()));
	    changed = true;
	  }
	}
      if (recursive)
	while (changed) {
	  changed = false;
	  for (itk::ImageRegionIteratorWithIndex<TImage> 
		 iit(image, image->GetLargestPossibleRegion()); 
	       !iit.IsAtEnd(); ++iit)
	    if (iit.Get() == bgVal) {
	      std::set<typename TImage::PixelType> nb;
	      getNeighbors<TImage>(nb, i2p(iit.GetIndex()), 
				   image, connect);
	      nb.erase(bgVal);
	      if (nb.size() == 1) {
		setv<TImage>(image, i2p(iit.GetIndex()), *(nb.begin()));
		changed = true;
	      }
	    }
	}
    }

};

#endif
