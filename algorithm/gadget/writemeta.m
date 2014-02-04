function writemeta(filename,vol,meta)

dims = ndims(vol);

% Transpose input vol to output correct image
% Only account for 2d and 3d images
% Unclear about higher dimensional images
if dims == 2
    vol = permute(vol, [2 1]);
elseif dims == 3
    vol = permute(vol, [2 1 3]);
end

sze = size(vol);

[str,maxsize,endian] = computer;
if endian == 'L'
    byteorder = 'ieee-le';
else
    byteorder = 'ieee-be';
end

if strcmp(class(vol),'uint32')
    elementtype = 'MET_UINT';
elseif strcmp(class(vol),'int32')
    elementtype = 'MET_INT';
elseif strcmp(class(vol),'uint16')
    elementtype = 'MET_USHORT';
elseif strcmp(class(vol),'int16')
    elementtype = 'MET_SHORT';
elseif strcmp(class(vol),'uint64')
%     elementtype = 'MET_ULONG';
    elementtype = 'MET_ULONG_LONG';
elseif strcmp(class(vol),'int64')
    elementtype = 'MET_LONG';
elseif strcmp(class(vol),'uint8')
    elementtype = 'MET_UCHAR';
elseif strcmp(class(vol),'int8')
    elementtype = 'MET_CHAR';
elseif strcmp(class(vol),'single')
    elementtype = 'MET_FLOAT';
elseif strcmp(class(vol),'double')
    elementtype = 'MET_DOUBLE';
else
    error('uknown elementtype');
end

ext = filename(end-3:end);
if ~strcmp(ext,'.mha') && ~strcmp(ext,'.mhd')
    error('invalid extension');
end

fid = fopen(filename,'wb');
if fid == -1
    error('could not open file for writing');
end
fprintf(fid,'ObjectType = Image\n');
fprintf(fid,'NDims = %d\n',dims);
fprintf(fid,'BinaryData = True\n');
if endian == 'L'
    fprintf(fid,'BinaryDataByteOrderMSB = False\n');
else
    fprintf(fid,'BinaryDataByteOrderMSB = True\n');
end
if nargin > 2
    try
        origin = meta.origin;
        fprintf(fid,'Offset =');
        fprintf(fid,' %f', origin);
        fprintf(fid,'\n');
    end
    try
        spacing = meta.spacing;
        fprintf(fid,'ElementSpacing =');
        fprintf(fid,' %f', spacing);
        fprintf(fid,'\n');
    end         
end

fprintf(fid,'DimSize =');
fprintf(fid,' %d', sze);
fprintf(fid,'\n');
fprintf(fid,'ElementType = %s\n',elementtype);
    
if strcmp(ext,'.mha')
    fprintf(fid,'ElementDataFile = LOCAL\n');
    fwrite(fid,vol(:),class(vol));
    fclose(fid);
elseif strcmp(ext,'.mhd')
    rawname = strcat(filename(1:end-4),'.raw');
    fprintf(fid,'ElementDataFile = %s\n',rawname);
    fclose(fid);
    frawid = fopen(rawname,'wb');
    fwrite(frawid,vol(:),class(vol));
    fclose(frawid);
else
    error('invalid extension');
end
 
