close all;
clear all;
clc;
 
% label = readmeta('~/Workspace/data/ellisman/truth/truth_235.mha');
label = imread('~/Workspace/data/ellisman/truth/truth_269.png');

% bin = single(label);
% bin(label > 0) = 1;
% clr = zeros([size(bin), 3]);
% clr(:, :, 1) = bin;
% clr(:, :, 2) = bin;
% clr(:, :, 3) = bin;
% figure, imshow(clr);
% [j, i, but] = ginput(1);
% while i < 1 || i > 700 || j < 1 || j > 700
%     [j, i, but] = ginput(1);
% end
% j = uint32(j);
% i = uint32(i);
% while but ~= 3
%     tmp = clr(:, :, 1);
%     tmp(tmp == 0.5) = 0.8;
%     clr(:, :, 1) = tmp;
%     l = label(i, j);
%     if l ~= 0
%         disp(l);
%         tmp = clr(:, :, 2);
%         tmp(label == l) = 0.8;
%         clr(:, :, 2) = tmp;
%         tmp = clr(:, :, 1);
%         tmp(label == l) = 0.5;
%         clr(:, :, 1) = tmp;
%         tmp = clr(:, :, 3);
%         tmp(label == l) = 0;
%         clr(:, :, 3) = tmp;
%         imshow(clr);
%     end
%     [j, i, but] = ginput(1);
%     while i < 1 || i > 700 || j < 1 || j > 700
%         [j, i, but] = ginput(1);
%     end
%     j = uint32(j);
%     i = uint32(i);
% end

bin = single(label);
bin(label > 0) = 1;
clr = zeros([size(bin), 3]);
clr(:, :, 1) = bin;
clr(:, :, 2) = bin;
clr(:, :, 3) = bin;
ulabels = setdiff(unique(label(:)), 0);
for l = ulabels(:)'
    tmp = clr(:, :, 1);
    tmp(tmp == 0.5) = 0.8;
    clr(:, :, 1) = tmp;
    disp(l);
    tmp = clr(:, :, 2);
    tmp(label == l) = 0.8;
    clr(:, :, 2) = tmp;
    tmp = clr(:, :, 1);
    tmp(label == l) = 0.5;
    clr(:, :, 1) = tmp;
    tmp = clr(:, :, 3);
    tmp(label == l) = 0;
    clr(:, :, 3) = tmp;
    imshow(clr);
    movegui('northeast');
    pause;
end
