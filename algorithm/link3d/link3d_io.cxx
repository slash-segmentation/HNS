#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

// Do not write region points
void n3::link3d::writeRegionStats (const char* regionStatFileName, 
				   std::map<Label, RegionStat> const& smap)
{
  std::ofstream fs(regionStatFileName);
  if (fs.is_open()) {
    for (std::map<Label, RegionStat>::const_iterator sit = smap.begin(); 
	 sit != smap.end(); ++sit) {
      RegionStat const* rs = &sit->second;
      fs << sit->first << "\n";
      fs << (rs->border? 1: 0) << ' ';
      writePoints(fs, rs->ucontour, true, ' ', ' ');
      fs << rs->ocontour.size() << ' ';
      for (std::vector<Points>::const_iterator cit = rs->ocontour.begin(); 
	   cit != rs->ocontour.end(); ++cit) 
	writePoints(fs, *cit, true, ' ', ' ');
      fs << rs->polygon.size() << ' ';
      for (std::vector<Points>::const_iterator cit = rs->polygon.begin(); 
	   cit != rs->polygon.end(); ++cit) 
	writePoints(fs, *cit, true, ' ', ' ');
      writePoint(fs, rs->centroid, ' ');
      fs << ' ';
      writePoint(fs, rs->box.start, ' ');
      fs << ' ';
      fs << rs->box.width << ' ' << rs->box.height << ' ';
      write2(fs, rs->stats, true, ' ', ' ');
      fs << '\n';
    }
    fs.close();
  }
  else perr("Error: cannot create region stats file...");
}



void n3::link3d::readRegionStats (std::map<Label, RegionStat>& smap, 
				  const char* regionStatFileName)
{
  std::ifstream fs(regionStatFileName);
  if (fs.is_open()) {
    Label r = 0;
    std::string line;
    while (getline(fs, line)) {
      bool success = true;
      do {
	std::istringstream isr(line);
	if (!(success = read(r, isr, (Label)0, ' ') && 
	      getline(fs, line))) break;
	RegionStat* rs = &smap[r];
	std::istringstream isd(line);
	int border = 0;	
	if (!(success = read(border, isd, (int)0, ' '))) break;
	rs->border = (border != 0);
	if (!(success = readPoints(rs->ucontour, isd, 
				   true, -1, ' '))) break;
	int n;
	if (!(success = read(n, isd, (int)0, ' '))) break;
	if (n > 0) {
	  rs->ocontour.resize(n);
	  for (int i = 0; i < n; ++i)
	    if (!(success = readPoints(rs->ocontour[i], isd, 
				       true, -1, ' '))) break;
	  if (!success) break;
	}
	if (!(success = read(n, isd, (int)0, ' '))) break;
	if (n > 0) {
	  rs->polygon.resize(n);
	  for (int i = 0; i < n; ++i)
	    if (!(success = readPoints(rs->polygon[i], isd, 
				       true, -1, ' '))) break;
	  if (!success) break;
	}	
	if (!(success = readPoint(rs->centroid, isd, ' '))) break;
	if (!(success = readPoint(rs->box.start, isd, ' '))) break;
	if (!(success = read(rs->box.width, isd, (float)0, ' '))) break;
	if (!(success = read(rs->box.height, isd, (float)0, ' '))) break;
	read2(rs->stats, isd);
      } while (false);
      if (!success) perr("Error: region stats file may be corrupted...");
    }
    fs.close();
  }
  else perr("Error: cannot read region stats file...");
}



void n3::link3d::readLinks (std::list<fLink>& links, 
			    const char* regionPairFileName, 
			    const char* dataFileName)
{
  int cnt = 0;
  if (regionPairFileName != NULL) {
    std::ifstream fs(regionPairFileName);
    if (fs.is_open()) {
      std::string line;
      bool success = true;
      while (getline(fs, line)) {
	std::istringstream is(line);
	fLink link;
	if (!(success = read(link.s0, is, (int)0, ' '))) break;
	if (!(success = read(link.r0, is, (Label)0, ' '))) break;
	if (!(success = read(link.s1, is, (int)0, ' '))) break;
	if (!(success = read(link.r1, is, (Label)0, ' '))) break;
	links.push_back(link);
	++cnt;
      }
      if (!success) perr("Error: region pair file may be corrupted...");
      fs.close();
    }
    else perr("Error: cannot read region pair file...");
  }
  if (dataFileName != NULL) {
    std::list<double> data;
    read(data, dataFileName, 1, ' ');
    if (cnt == 0) // Only read data
      for (std::list<double>::const_iterator it = data.begin(); 
	   it != data.end(); ++it) 
	links.push_back(fLink(0, 0, -1, -1, *it));
    else if (cnt == data.size()) { // Read region pairs and data
      std::list<fLink>::reverse_iterator lit = links.rbegin();
      for (std::list<double>::const_reverse_iterator wit = 
	     data.rbegin(); wit != data.rend(); ++wit) {
	lit->data = *wit;
	++lit;
      }
    }
    else perr("Error: region pair and weight number disagree...");
  } 
}



void n3::link3d::readBodies (std::list<fBody>& bodies, 
			     const char* bodyFileName, 
			     const char* dataFileName) 
{
  int cnt = 0;
  if (bodyFileName != NULL) {
    std::ifstream fs(bodyFileName);
    if (fs.is_open()) {
      std::string line;
      while (getline(fs, line)) {
	std::istringstream is(line);
	bodies.push_back(fBody());
	is >> bodies.back();
      }
      fs.close();
    }
    else perr("Error: cannot read body file...");
  }  
  if (dataFileName != NULL) {
    std::list<double> data;
    read(data, dataFileName, 1, ' ');
    if (cnt == 0) {// Only read data
      for (std::list<double>::const_iterator dit = data.begin(); 
	   dit != data.end(); ++dit)
	bodies.push_back(fBody(0, 0, *dit));
    }
    else if (cnt == data.size()) {
      std::list<fBody>::iterator bit = bodies.begin();
      for (std::list<double>::const_iterator dit = data.begin(); 
	   dit != data.end(); ++dit) {
	bit->data = *dit;
	++bit;
      }
    }
    else perr("Error: body and data number disagree...");
  }
}



void n3::link3d::readBodyLinks (std::list<fBodyLink>& links, 
				const char* bodyPairFileName, 
				const char* dataFileName)
{
  int cnt = 0;
  if (bodyPairFileName != NULL) {
    std::ifstream fs(bodyPairFileName);
    if (fs.is_open()) {
      std::string line;
      bool success = true;
      while (getline(fs, line)) {
	std::istringstream is(line);
	links.push_back(fBodyLink());
	if (!(success = read(links.back().nodes[0].first, is, 
			     (Label)0, ' '))) break;
	if (!(success = read(links.back().nodes[0].second, is, 
			     (int)0, ' '))) break;
	if (!(success = read(links.back().nodes[1].first, is, 
			     (Label)0, ' '))) break;
	if (!(success = read(links.back().nodes[1].second, is, 
			     (int)0, ' '))) break;
	++cnt;
      }
      if (!success) perr("Error: body pair file may be corrupted...");
      fs.close();
    }
    else perr("Error: cannot read body pair file...");
  }
  if (dataFileName != NULL) {
    std::ifstream fs(dataFileName);
    if (fs.is_open()) {
      std::list<double> data;
      read(data, dataFileName, 1, ' ');
      if (cnt == 0) // Only read data
	for (std::list<double>::const_iterator it = data.begin(); 
	     it != data.end(); ++it) 
	  links.push_back(fBodyLink(0, -1, 0, -1, *it));
      else if (cnt == data.size()) {
	std::list<fBodyLink>::iterator lit = links.begin(); 
	for (std::list<double>::const_iterator dit = data.begin(); 
	     dit != data.end(); ++dit) {
	  lit->data = *dit;
	  ++lit;
	}
      }
      fs.close();
    }
    else perr("Error: cannot read body link data file...");
  }
}



void n3::link3d::writeBodyStats (const char* bodyStatFileName, 
				 std::map<Label, BodyStat> const& smap)
{
  std::ofstream fs(bodyStatFileName);
  if (fs.is_open()) {
    for (std::map<Label, BodyStat>::const_iterator it = smap.begin(); 
	 it != smap.end(); ++it) 
      fs << it->first << " " << it->second << "\n";
    fs.close();
  }
  else perr("Error: cannot create body stats file...");
}



void n3::link3d::readBodyStats (std::map<Label, BodyStat>& smap, 
				const char* bodyStatFileName)
{
  std::ifstream fs(bodyStatFileName);
  if (fs.is_open()) {
    std::string line;
    while (getline(fs, line)) {
      std::istringstream is(line);
      Label key;
      is >> key;
      is >> smap[key];
    }
    fs.close();
  }
  else perr("Error: cannot read body stats file...");
}
