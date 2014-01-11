Modular Hierarchical EM Segmentation

Contact: 
Ting Liu (ting@sci.utah.edu)


Reference: 
If you use this code, please refer to our paper: 

@inproceedings{liu2012watershed,
  title={Watershed merge tree classification for electron microscopy image segmentation},
  author={Liu, Ting and Jurrus, Elizabeth and Seyedhosseini, Mojtaba and Ellisman, Mark and Tasdizen, Tolga},
  booktitle={Pattern Recognition (ICPR), 2012 21st International Conference on},
  pages={133--137},
  year={2012},
  organization={IEEE}
}


Dependency:
To successfully compile the code, ITK, OpenCV, and GMP are required. CMake is used for configuration. To compile the random forest code, one needs gfortran compiler. We tested our code with:
  [1] OpenSUSE 12.3 64-bit
  [2] ITK 4+
  [3] OpenCV 2.4.2
  [4] GMP 5.1.x
When compiling GMP, one must enable C++ support by specifying '--enable-cxx'. OpenMP is used when supported.


Compilation:
Here is an example:
  [1] cd n3
  [2] ccmake . (Specify dependency paths in cmake. Set 'CMAKE_BUILD_TYPE' to 'Release'.)
  [3] make 
  

Usage: 
There are two main modules, i.e., 2D segmentation and 3D linking. Each program also prints usage information if given no input arguments. The displayed arguments within brackets are with default values if not specified. Arguments within curly brackets can be multiple. 

Here we mean floating point valued images by value images, and unsigned integer valued images by label images. Pixel intensities of a value image should always be normalized to within [0, 1]. Label images always have 0 as background, and positive labels for foreground regions. We always used 'mha' format for value images, and label images with potentially very large integer labels. If the number of labels is guaranteed not to cause a out-of-range problem, 'png' or other ITK supported image formats can be used for label images. 

In the following, each program is introduced with an example. We assume there are three training slices (slice 1 - 3) and three testing slices (slice 4 - 6) (with raw intensity images, membrane detection probability maps, 2D ground truth label images for training slices, and 3D pairwise ground truth connections for testing slices). 

Module 1: 2D segmentation

  Step 1: Generate texton dictionary [with training data]. Use 'genTextonDict':
    [1] -v valueImageName: add raw image file names.
    [2] -l labelImageName: add 2D ground truth segmentation image file names.
    [3] maxIteration: maximum k-means iteration number (default: 10000).
    [3] convergeThreshold: k-means convergence change threshold (default: 2e-4).
    [4] textonDictionaryFileName: output texton dictionary file name (space separated text).
  Example: genTextonDict -v raw1.mha -v raw2.mha -v raw3.mha -l truth1.mha -l truth2.mha -l truth3.mha 10000 2e-4 tdict.ssv
    
  Step 2: Generate initial segmentation [with all data]. Note that slightly blurring (e.g., Gaussian filter with sigma = 1) membrane detection probability maps is recommended to efficiently reduce initial regions. Use 'watershed':
    [1] boundaryValueImageName: membrane detection probability map file name.
    [2] seedThreshold: threshold for initial seed dynamics. (Generally, higher threshold reduces number of initial regions.)
    [3] writeToUInt16Image: whether to write the output label image as uint16 label image (otherwise it is uint32) (default: 0, meaning 'no').
    [4] keepWatershedLine: always set to 1 (default: 1).
    [5] isBoundaryFullyConnected: always set to 1 (default: 1).
    [6] resultImageName: output initial segmentation image file name.
  Example: watershed pmap1.mha 0.1 0 1 1 initseg1.mha
  
  Step 3: Pre-merge initial regions (optional but recommended) [with all data]. There are two pre-merging criteria: (1) if a region is smaller than a certain area threshold (ta0), and (2) if a region is smaller than a certain area threshold and (ta1) its average membrane probability is above a certain threshold (tp). A region that satisfies either criterion will be pre-merged with one of its neighbor regions. The output will be used as the initial segmentation in the following. Use 'merge': 
    [1] inputSegImageName: initial segmentation image file name.
    [2] boundaryProbabilityImageName: membrane detection probability map file name.
    [3] areaThreshold0: area threshold ta0 (commonly use 50).
    [4] areaThreshold1: area threshold ta1 (commonly use 200).
    [5] probabilityThreshold: average probability threshold tp (commonly use 0.5).
    [6] writeToUInt16: whether to write the output label image as uint16 label image (otherwise it is uint32) (default: 0, meaning 'no').
    [7] consecutiveOutputLabels: always set to 1.
    [8] outputSegImageName: output initial segmentation after pre-merging.
  Example: merge initseg1.mha pmap1.mha 50 200 0.5 0 1 initseg1.mha
  
  Step 4: Generate merge tree and saliency files [with all data]. Use 'genMerges':
    [1] labelImageNames: initial segmentation (pre-merged version if any) image file name.
    [2] boundaryValueImageName: membrane detection probability map file name.
    [3] treeFileName: output merge tree file name (space separated text).
    [4] saliencyFileName: output merging saliency file name (space separated text).
  Example: genMerges initseg1.mha pb1.mha tree1.ssv saliency1.ssv
    
  Step 5: Generate boundary classification features [with all data]. The output is a text file with each row representing a sample. The order strictly follows that of the merge tree file. Note that skipping texton features will largely accelerate the program, but may worsen the accuracy (usually by a little). Use 'genBoundaryFeatures:
    [1] labelImageName: initial segmentation image file name.
    [2] treeFileName: merge tree file name.
    [3] saliencyFileName: merging saliency file name (Use 'NULL' to skip merging saliency features).
    [4] rawImageName: raw intensity image file name (Use 'NULL' to skip raw intensity features).
    [5] pbImageName: membrane detection probability map file name (Use 'NULL' to skip membrane detection probability features).
    [6] textonDictFileName: texton dictionary file name (Use 'NULL' to skip texton features).
    [7] featureFileName: output boundary classification feature file name.
  Example: genBoundaryFeature initseg1.mha tree1.ssv saliency1.ssv raw1.mha pb1.mha tdict.ssv bcfeat1.ssv
  
  Step 6: Generate boundary classification training labels [with training data]. The output is a text file with a column of labels. The order strictly follows that of the merge tree file. Use 'genBoundaryLabels': 
    [1] labelImageName: initial segmentation image file name.
    [2] treeFileName: merge tree file name.
    [3] truthImageName: 2D ground truth segmentation image file name.
    [4] labelFileName: output boundary classification label file name.
  Example: genBoundaryLabels initseg1.mha tree1.ssv truth1.mha bclabel1.ssv
  
  Step 7: Train random forest boundary classifier [with training data]. We used a MATLAB-wrapped random forest implementation (https://code.google.com/p/randomforest-matlab/), while other standard random forest implementations should also work. Users should train with all the sample feature files and label files generated in Step 5 and 6. We usually use 255 decision trees, square root of the number of all features to exam at each decision tree node split, and 70% of all samples to train each decision tree.
  
  Step 8: Predict with boundary classifier [with all data]. The predicted file should be a text file with a column of probabilities for label -1 (merge). The output order should strictly follows that of the merge tree file.

  Step 9: Generate final segmentation [with all data]. Use 'segment':
    [1] initialSegmentationImageName: initial segmentation image file name.
    [2] treeFileName: merge tree file name.
    [3] mergeProbabilityFileName: predicted merging probability file name.
    [4] labelOutputBinaryImageConnectedComponents: always set to 1.
    [5] writeToUInt16Image: whether to write the output label image as uint16 label image (otherwise it is uint32) (default: 0, meaning 'no').
    [6] finalSegmentationImageName: output final segmentation image file name.
  Example: segment initseg4.mha tree4.ssv bcpred4.ssv 1 0 seg2d4.mha

Module 2: 3D linking

  Step 1: Generate region statistics [with all data]. This is an intermediate step. Use 'genRegionStats':
    [1] labelImageName: 2D final segmentation image file name.
    [2] regionStatFileName: output region statistics file name (space separated text file).
  Example: genRegionStats seg2d1.mha rstat1.ssv

  Step 2: Generate region pair link candidates [with all data]. Region pairs that overlap or are within the input centroid distance threshold are considered as candidates. Use 'genRegionPairs':
    [1] sliceNo0: slice number 0
    [2] sliceNo1: slice number 1
    [3] labelImageName0: 2D final segmentation image 0 file name.
    [4] labelImageName1: 2D final segmentation image 1 file name.
    [5] centroidDistanceThreshold: centroid distance threshold for candidate region pairs (set to 0 to only use overlap).
    [6] regionPairFileName: output region pair link candidate file name.
  Example: genRegionPairs 1 2 seg2d1.mha seg2d2.mha 50 rpair12.ssv
  
  Step 3: Generate section classification features [with all data]. We used SIFT flow code (http://people.csail.mit.edu/celiu/SIFTflow/) for image alignment. Please use gadget/siftFlowAlign.m as the interface along with the SIFT flow authors' code to compute the transformation matrices between two raw intensity images, and use gadget/warpImage.m to warp corresponding 2D segmentation label images. The output is a text file with each row representing a sample. The order strictly follows that of the region pair link candidate file. Use 'genSectionFeatures':
    [1] regionPairFileName: region pair link candidate file name.
    [2] labelImageName0: 2D final segmentation image 0 file name.
    [3] labelImageName1: 2D final segmentation image 1 file name.
    [4] alignImageName1: 2D final segmentation image 1 aligned to image 0 (use 'NULL' to ignore the image alignment related features).
    [5] regionStatFileName0: region statistics for slice 0 file name.
    [6] regionStatFileName1: region statistics for slice 1 file name.
    [7] featFilename: output section classification feature file name.
  Example: genSectionFeatures rpair12.ssv seg2d1.mha seg2d2.mha NULL rstat1.ssv rstat2.ssv scfeat12.ssv
  
  Step 4: Generate section classification training labels [with training data]. The output is a text file with a column of labels. The order strictly follows that of the region pair link candidate file. Use 'genSectionLabels':
    [1] regionPairFileName: region pair link candidate file name.
    [2] truthPairFileName: ground truth region pair link file name. Such should be space separated text files with each line having two region labels. For example, a row '10 12' means region 10 on 2D ground truth segmentation image for slice 0 should be connected in 3D to region 13 on 2D ground truth segmentation image slice 1. 
    [3] labelImageName0: 2D final segmentation image 0 file name. 
    [4] labelImageName1: 2D final segmentation image 1 file name.
    [5] truthImageName0: 2D ground truth segmentation image 0 file name.
    [6] truthImageName1: 2D ground truth segmentation image 1 file name.
    [7] labelFileName: output section classification training label file name.
  Example: genSectionLabels rpair12.ssv truthlink12.ssv seg2d1.mha seg2d2.mha sclabel12.ssv

  Step 5: Train random forest section classifier [with training data]. We used a MATLAB-wrapped random forest implementation (https://code.google.com/p/randomforest-matlab/), while other standard random forest implementations should also work. Users should train with all the sample feature files and label files generated in Step 5 and 6. We usually use 255 decision trees, square root of the number of all features to exam at each decision tree node split, and 70% of all samples to train each decision tree.

  Step 6: Predict with section classifier [with all data]. The predicted file should be a text file with a column of probabilities for label +1 (true link). The output order should strictly follows that of the region pair link candidate file.

  Step 7: Threshold link candidates and generate 3D final segmentations. The outputs are a sequence of label images. Each unique neuron will be labeled consistently across slices. Use 'threshold':
    [1] -i sliceNo inputImageName outputImageName: slice numbers, 2D final segmentation image file names, and output 3D final segmentation image file names.
    [2] -l regionPairFileName weightFileName: region pair link candidate file names, and section classifier prediction file names. If non-adjancet links are considered, add all region pair link candidate files and section classifier prediction files here.  
    [3] directLinkThreshold: threshold for adjancet links (links above the threshold are preserved). 
    [4] skipLinkThreshold: threshold for non-adjacent links (links above the threshold are preserved).
    [5] writeToUInt16: whether to write the output label image as uint16 label image (otherwise it is uint32) (default: 0, meaning 'no').
    [6] forceOneDirectLink: always set to 1.
  Example: threshold -i 4 seg2d4.mha seg3d4.mha -i 5 seg2d5.mha seg3d5.mha -i 6 seg2d6.mha seg3d6.mha -l rpair45.ssv scpred45.ssv -l rpair56.ssv scpred56.ssv -l rpair67.ssv scpre67.ssv 0.8 0.95 0 1 
