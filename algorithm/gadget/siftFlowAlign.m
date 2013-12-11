function siftFlowAlign (imageName0, imageName1, vxFileName, vyFileName)
% Compute transformation that can be applied to align image1 to image0
[~, ~, ext0] = fileparts(imageName0);
[~, ~, ext1] = fileparts(imageName1);
if strcmp(ext0, '.mha')
    im0 = double(readmeta(imageName0));
else
    im0 = imread(imageName0);
    if isa(im0, 'uint16')
        im0 = double(im0) / 65535.0;
    else
        im0 = double(im0) / 255.0;
    end    
end
if strcmp(ext1, '.mha')
    im1 = double(readmeta(imageName1));
else
    im1 = imread(imageName1);
    if isa(im1, 'uint16')
        im1 = double(im1) / 65535.0;
    else
        im1 = double(im1) / 255.0;
    end    
end
[vx, vy] = sift_flow_align(im0, im1);
dlmwrite(vxFileName, vx, 'delimiter', ' ');
dlmwrite(vyFileName, vy, 'delimiter', ' ');
