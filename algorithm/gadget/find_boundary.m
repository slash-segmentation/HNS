function vol_out = find_boundary (vol_in)

vol_out = vol_in;
maxi = max(vol_out(:));
for i = 1: size(vol_in, 3)
    slice = vol_out(:, :, i);
    right = circshift(slice, [0, 1]);
    right(:, 1) = right(:, 2);
    left = circshift(slice, [0, -1]);
    left(:, end) = left(:, end - 1);    
    up = circshift(slice, [1, 0]);
    up(end, :) = up(end - 1, :);
    down = circshift(slice, [-1, 0]);
    down(1, :) = down(2, :);
    comp = (slice ~= right | slice ~= left | slice ~= up | slice ~= down);
    slice(comp == 1) = maxi + 1;
    vol_out(:, :, i) = slice;
end