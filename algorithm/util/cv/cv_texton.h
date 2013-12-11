#ifndef _n3_cv_texture_h_
#define _n3_cv_texture_h_

#include "n3_io.h"
#include "util/struct_util3.h"

namespace n3 {

  const int TEXTON_PAT_SIZE = 7;
  const int TEXTON_PAT_HALF_SIZE = (TEXTON_PAT_SIZE - 1) / 2;
  const int TEXTON_PAT_LEN = TEXTON_PAT_SIZE * TEXTON_PAT_SIZE;
  const int TEXTON_PAT_LEN3 = TEXTON_PAT_SIZE * TEXTON_PAT_SIZE * 
    TEXTON_PAT_SIZE;
  const int TEXTON_BIN = 100;

  typedef std::vector<float> TextonPatch;
  typedef std::vector<TextonPatch> TextonDict;


  /* Generate texton dictionary */
  void genTextonDict (TextonDict& dict, std::list<TextonPatch> const& pats, 
		      int maxIterationNum, double convergenceThreshold, 
		      int patchLength);
  /* Generate texton patch */
  // For current use, set resetCanvas to false for speed
  // Otherwise, set resetCanvas to true for safety
  void genTextonPatch (std::list<TextonPatch>& pats, 
		       FloatImage::Pointer valImage, 
		       Points const& points, LabelImage::Pointer canvas, 
		       bool resetCanvas);
  // For current use, set resetCanvas to false for speed
  // Otherwise, set resetCanvas to true for safety
  void genTextonPatch (std::list<TextonPatch>& pats, 
		       FloatImage3::Pointer valImage, 
		       Points3 const& points, LabelImage3::Pointer canvas, 
		       bool resetCanvas);
  /* Generate texton histogram */
  void genTextonHist (fvec& hist, TextonDict const& dict, 
		      std::list<TextonPatch> const& pats);
  /* Generate texton histogram */
  void genTextonHist (fvec& hist, TextonDict const& dict, 
		      Points const& points, FloatImage::Pointer valImage, 
		      LabelImage::Pointer canvas, bool resetCanvas);
  void genTextonHist (fvec& hist, TextonDict const& dict, 
		      Points3 const& points, FloatImage3::Pointer valImage, 
		      LabelImage3::Pointer canvas, bool resetCanvas);
  
  void genTextonHist (fvec& hist, TextonDict const& dict, 
		      std::list<TextonPatch const*> const& pats);

  // Generate textons for every point except borders
  void genTextons (std::vector<std::vector<TextonPatch> >& textons, 
		   FloatImage::Pointer valImage);

  // For current use, set resetCanvas to false for speed
  // Otherwise, set resetCanvas to true for safety  
  void genTextonPatch (std::list<TextonPatch const*>& pats, 
		       std::vector<std::vector<TextonPatch> > const& 
		       textons, Points const& points, 
		       UInt8Image::Pointer check, 
		       LabelImage::Pointer canvas, bool resetCanvas);

  // For current use, set resetCanvas to false for speed
  // Otherwise, set resetCanvas to true for safety
  void genTextonHist (fvec& hist, TextonDict const& dict, 
		      std::vector<std::vector<TextonPatch> > const& 
		      textons, Points const& points, 
		      UInt8Image::Pointer check, 
		      LabelImage::Pointer canvas, bool resetCanvas);



  template <typename TContainer> void 
    writeTexton (const char* file, TContainer const& pats)
    {
      std::ofstream fs(file);
      if (fs.is_open()) {
	for (typename TContainer::const_iterator it = pats.begin(); 
	     it != pats.end(); ++it) write(fs, *it, -1, ' ');
	fs.close();
      }
      else perr("Error: cannot create texton file...");
    }



  template <typename TContainer> void 
    readTexton (TContainer& pats, const char* file, int patchLength)
    {
      std::ifstream fs(file);
      if (fs.is_open()) {
	std::list<float> data;
	read(data, fs, -1, ' ');
	std::list<float>::const_iterator it = data.begin(); 
	while (it != data.end()) {
	  TextonPatch pat(patchLength, 0.0);
	  for (int i = 0; i < patchLength; ++i) {
	    pat[i] = *it;
	    ++it;
	  }
	  pats.push_back(pat);
	}
	fs.close();
      }	
      else perr("Error: cannot read texton file...");
    }

};

#endif
