#ifndef _n3_link3d_io_h_
#define _n3_link3d_io_h_

#include "util/io_util.h"
#include "link3d.h"
#include "rs.h"
#include "bs.h"

namespace n3 {

  namespace link3d {

    void writeRegionStats (const char* regionStatFileName, 
			   std::map<Label, RegionStat> const& smap);

    void readRegionStats (std::map<Label, RegionStat>& smap, 
			  const char* regionStatFileName);

    void readLinks (std::list<fLink>& links, 
		    const char* regionPairFileName, 
		    const char* dataFileName);

    void readBodies (std::list<fBody>& bodies, 
		     const char* bodyFileName, 
		     const char* dataFileName);

    void readBodyLinks (std::list<fBodyLink>& links, 
			const char* bodyPairFileName, 
			const char* dataFileName);

    void writeBodyStats (const char* bodyStatFileName, 
			 std::map<Label, BodyStat> const& smap);

    void readBodyStats (std::map<Label, BodyStat>& smap, 
			const char* bodyStatFileName);

    template <typename TContainer> void 
      writeLinks (const char* regionPairFileName, 
		  const char* dataFileName, TContainer const& links)
      {  
	if (regionPairFileName != NULL) {
	  std::ofstream fs(regionPairFileName);
	  if (fs.is_open()) {
	    for (typename TContainer::const_iterator it = links.begin(); 
		 it != links.end(); ++it) 
	      fs << it->s0 << " " << it->r0 << " " 
		 << it->s1 << " " << it->r1 << "\n";
	    fs.close();
	  }
	  else perr("Error: cannot create region pair file...");
	}  
	if (dataFileName != NULL) {
	  std::ofstream fs(dataFileName);
	  if (fs.is_open()) {
	    for (typename TContainer::const_iterator it = links.begin(); 
		 it != links.end(); ++it) {
	      write(fs, it->data);
	      fs << "\n";
	    }
	    fs.close();
	  }
	  else perr("Error: cannot create region pair data file...");
	}
      }


    template <typename TBodyData> void 
      writeBodies (const char* bodyFileName, const char* dataFileName, 
		   std::list<Body<TBodyData> > const& bodies)
      {
	if (bodyFileName != NULL) {
	  std::ofstream fs(bodyFileName);
	  if (fs.is_open()) {
	    for (typename std::list<Body<TBodyData> >::const_iterator 
		   bit = bodies.begin(); bit != bodies.end(); ++bit) 
	      fs << *bit << "\n";
	    fs.close();
	  }
	  else perr("Error: cannot create body file...");
	}
	if (dataFileName != NULL) {
	  std::ofstream fs(dataFileName);
	  if (fs.is_open()) {
	    for (typename std::list<Body<TBodyData> >::const_iterator 
		   bit = bodies.begin(); bit != bodies.end(); ++bit) 
	      fs << bit->data << "\n";
	    fs.close();
	  }
	  else perr("Error: cannot create body data file...");
	}
      }

    
    template <typename TContainer> void 
      writeBodyLinks (const char* bodyPairFileName, 
		      const char* dataFileName, TContainer const& links)
      {
	if (bodyPairFileName != NULL) {
	  std::ofstream fs(bodyPairFileName);
	  if (fs.is_open()) {
	    for (typename TContainer::const_iterator lit = links.begin(); 
		 lit != links.end(); ++lit) fs << *lit << "\n";
	    fs.close();
	  }
	  else perr("Error: cannot create body pair file...");
	}
	if (dataFileName != NULL) {
	  std::ofstream fs(dataFileName);
	  if (fs.is_open()) {
	    fs << links.size() << "\n";
	    for (typename TContainer::const_iterator lit = links.begin(); 
		 lit != links.end(); ++lit) fs << lit->data << "\n";
	    fs.close();
	  }
	  else perr("Error: cannot creat body pair data file...");
	}
      }
    
  };

};

#endif
