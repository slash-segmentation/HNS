function [vx, vy] = sift_flow_align (im0, im1)

cellSize = 3;
gridSpacing = 1;

sift0 = mexDenseSIFT(im0, cellSize, gridSpacing);
sift1 = mexDenseSIFT(im1, cellSize, gridSpacing);

param.alpha = 2 * 255;
param.d = 40 * 255;
param.gamma = 0.005 * 255;
param.nlevels = 4;
param.wsize = 2;
param.topwsize = 10;
param.nTopIterations = 60;
param.nIterations = 30;

% param.alpha = 0.01;
% param.d = param.alpha * 20;
% param.gamma = 0.001;
% param.nlevels = 4;
% param.wsize = 3;
% param.topwsize = 10;
% param.nTopIterations = 100;
% param.nIterations = 40;

tic;
[vx, vy, ~] = SIFTflowc2f(sift0, sift1, param);
toc;
