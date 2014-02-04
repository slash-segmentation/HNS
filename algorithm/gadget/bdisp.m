function bdisp (im_file_name, node_file_name, edge_file_name)
[~, ~, ext] = fileparts(im_file_name);
if strcmp(ext, '.mha')
    im = readmeta(im_file_name);
elseif strcmp(ext, '.tif') || strcmp(ext, '.tiff')
    im = read_tiff(im_file_name);
else
    im = imread(im_file_name);
end
idx = find(im ~= 0);
[I, J, K] = ind2sub(size(im), idx);
x = J - 1;
y = I - 1;
z = K - 1;
figure, scatter3(x, y, z, 'c.');
hold on;
xlabel('x');
ylabel('y');
zlabel('z');

% [imh, imw, imd] = size(im);
% n = length(x);
% for i = 1: n
%     xc = x(i);
%     yc = y(i);
%     zc = z(i);
%     for dx = -1: 1
%         for dy = -1: 1
%             for dz = -1: 1
%                 xn = xc + dx;
%                 yn = yc + dy;
%                 zn = zc + dz;
%                 if xn >= 0 && xn < imw && yn >= 0 && yn < imh && ...
%                         zn >= 0 && zn < imd && ...
%                         im(yn + 1, xn + 1, zn + 1) ~= 0 && ...
%                         ~(xn == xc && yn == xc && zn == zc)
%                     plot3([xc; xn], [yc; yn], [zc; zn], 'b-');
%                 end
%             end
%         end
%     end
% end

if nargin > 1
    node_xyz = load(node_file_name);
    scatter3(node_xyz(:, 1), node_xyz(:, 2), node_xyz(:, 3), 'r');
    if nargin > 2
        fid = fopen(edge_file_name);
        tl = fgets(fid);
        while ischar(tl)
            edge_xyz = sscanf(tl, '%f');
            edge_x = edge_xyz(1: 3: end);
            edge_y = edge_xyz(2: 3: end);
            edge_z = edge_xyz(3: 3: end);
            plot3(edge_x, edge_y, edge_z, '.b-');
            tl = fgets(fid);
        end
        fclose(fid);
    end
end