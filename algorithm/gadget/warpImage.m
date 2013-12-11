function warpImage (inputImageName, vxFileName, vyFileName, type, ...
    outputImageName)
% Type should be 'nearest', 'linear', 'spline' or 'cubic'. 
% For label image, use 'nearest'.
vx = load(vxFileName);
vy = load(vyFileName);
[~, ~, ext] = fileparts(inputImageName);
if strcmp(ext, '.mha')
    im = readmeta(inputImageName);
else
    im = imread(inputImageName);
end
im = warp_image(im, vx, vy, type);
[~, ~, ext] = fileparts(outputImageName);
if strcmp(ext, '.mha')
    writemeta(outputImageName, im);
else
    imwrite(im, outputImageName);
end
