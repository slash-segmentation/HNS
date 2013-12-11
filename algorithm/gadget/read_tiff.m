function im = read_tiff (name)
iinfo = imfinfo(name);
n = numel(iinfo);
im0 = imread(name, 1);
im = zeros([size(im0), n], class(im0));
im(:, :, 1) = im0;
for i = 2: n
    im(:, :, i) = imread(name, i);
end