function write_tiff (name, vol)
if isa(vol, 'single')
    op.SampleFormat = Tiff.SampleFormat.IEEEFP;
    op.BitsPerSample = 32;
    vol = single(vol);
elseif isa(vol, 'double')
    op.SampleFormat = Tiff.SampleFormat.IEEEFP;
    op.BitsPerSample = 64;
    vol = double(vol);
elseif isa(vol, 'uint16')
    op.SampleFormat = Tiff.SampleFormat.UInt;
    op.BitsPerSample = 16;
    vol = uint16(vol);
elseif isa(vol, 'uint8')
    op.SampleFormat = Tiff.SampleFormat.UInt;
    op.BitsPerSample = 8;
    vol = uint8(vol);
elseif isa(vol, 'uint32')
    op.SampleFormat = Tiff.SampleFormat.UInt;
    op.BitsPerSample = 32;
    vol = uint32(vol);
else
    error('Error: unsupported image type...');
end

[w, h, d] = size(vol);
op.Compression = Tiff.Compression.None;
op.Photometric = Tiff.Photometric.MinIsBlack;
op.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
op.SamplesPerPixel = 1;
op.ImageLength = h;
op.ImageWidth = w;

delete(name);
for i = 1: d
    t = Tiff(name, 'a');
    t.setTag(op);
    t.write(vol(:, :, i));
    t.close();
end