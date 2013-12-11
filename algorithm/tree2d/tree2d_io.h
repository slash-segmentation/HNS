#ifndef _n3_tree2d_io_h_
#define _n3_tree2d_io_h_

#include "n3_io.h"
#include "tree2d.h"

namespace n3 {
  
  namespace tree2d {

    /* Set dataFile to NULL if do not write merge data file */
    template <typename T> void 
      writeMerges (const char* structFile, const char* dataFile, 
		   std::list<Merge<T> > const& merges) 
      {
	std::ofstream fss(structFile);	
	if (fss.is_open()) {
	  for (typename std::list<Merge<T> >::const_iterator it = 
		 merges.begin(); it != merges.end(); ++it)
	    fss << it->from0 << " " << it->from1 << " " 
		<< it->to << "\n";
	  fss.close();
	}
	else perr("Error: cannot create merge structure file...");
	if (dataFile != NULL) {
	  std::ofstream fsd(dataFile);
	  if (fsd.is_open()) {
	    for (typename std::list<Merge<T> >::const_iterator it = 
		   merges.begin(); it != merges.end(); ++it)
	      fsd << it->data << "\n";
	    fsd.close();
	  }
	  else perr("Error: cannot create merge data file");
	}	
      }



    /* Set dataFile to NULL if do not read merge data */
    template <typename T> void 
      readMerges (std::list<Merge<T> >& merges, const char* structFile, 
		  const char* dataFile) 
      {
	std::ifstream fss(structFile);
	if (fss.is_open()) {
	  std::list<double> data;
	  read(data, fss, 3, ' ');
	  std::list<double>::const_iterator it = data.begin();
	  while (it != data.end()) {
	    Label from0 = (Label)(*it + 0.5); ++it;
	    Label from1 = (Label)(*it + 0.5); ++it;
	    Label to = (Label)(*it + 0.5); ++it;
	    merges.push_back(Merge<T>(from0, from1, to));
	  }
	  fss.close();
	}
	else perr("Error: cannot read merge structure file...");
	if (dataFile != NULL) {
	  std::ifstream fsd(dataFile);
	  if (fsd.is_open()) {
	    std::list<T> data;
	    read(data, fsd, 1);
	    if (data.size() != merges.size()) 
	      perr("Error: merge structure/data length disagree...");
	    typename std::list<T>::const_iterator dit = data.begin(); 
	    typename std::list<Merge<T> >::iterator mit = merges.begin();
	    while (dit != data.end()) {
	      mit->data = *dit;
	      ++dit;
	      ++mit;
	    }
	    fsd.close();
	  }
	  else perr("Error: cannot read merge data file...");
	}
      }  
  };

};

#endif
