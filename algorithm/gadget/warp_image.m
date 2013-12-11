function im_out = warp_image (im_in, vx, vy, type)
% Warp image with vx and vy. Type should be 'nearest', 'linear', 'spline'
% 'bicubic', or 'cubic'. For label image, use 'nearest'.
[h2, w2] = size(im_in);
[h1, w1] = size(vx);
[xx, yy] = meshgrid(1: w2, 1: h2);
[XX, YY] = meshgrid(1: w1, 1: h1);
XX = XX + vx;
YY = YY + vy;
XX = min(max(XX, 1), w2);
YY = min(max(YY, 1), h2);
im_out = interp2(xx, yy, im_in, XX, YY, type);
