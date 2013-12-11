function [vol meta] = readmeta(filename)
   
  %Get the full path if needed
  [directory] = fileparts(filename);

  fid = fopen(filename,'rb');
  if fid == -1
      error('Could not open MetaImage');
  end
  
  dflag = 1;
  
  binarydata = true; 
  
  while dflag
    l = fgetl(fid);
    key = sscanf(l,'%s',1);
    sscanf(l,'%*s %s',1);
    value = sscanf(l,'%*s %*s %s',1);

    if strcmp(key,'ObjectType')
      if ~strcmp(value,'Image')
          error('Only Image ObjectType allowed');
      end
    elseif strcmp(key,'NDims')
      dims = uint32(str2num(value));
    elseif strcmp(key,'BinaryDataByteOrderMSB')
      if strcmp(value,'False')
        byteorder = 'ieee-le';
      else
        byteorder = 'ieee-be';
      end
    elseif strcmp(key,'BinaryData')
      if strcmp(value,'True')
          binarydata = true;
      end
    elseif strcmp(key,'DimSize')
      isize = uint32(sscanf( l(11:end) ,'%d'));
    elseif strcmp(key,'ElementSpacing')
      spacing = sscanf( l(18:end) ,'%f');
    elseif strcmp(key,'Offset')
      origin = sscanf( l(10:end), '%f');
    elseif strcmp(key,'ElementType')
      if strcmp(value,'MET_USHORT')
        etype = 'uint16';
      elseif strcmp(value,'MET_SHORT')
        etype = 'int16';
      elseif strcmp(value,'MET_CHAR')
        etype = 'int8';
      elseif strcmp(value,'MET_UCHAR')
        etype = 'uint8';
      elseif strcmp(value,'MET_INT')
        etype = 'int32';
      elseif strcmp(value,'MET_UINT')
        etype = 'uint32';
      elseif strcmp(value,'MET_LONG')
        etype = 'int64';
      elseif strcmp(value,'MET_ULONG')
        etype = 'uint64';
      % Added by Ting
      elseif strcmp(value, 'MET_ULONG_LONG')
        etype = 'uint64';
      % ~ Added by Ting
      elseif strcmp(value,'MET_FLOAT')
        etype = 'single';
      elseif strcmp(value,'MET_DOUBLE')
        etype = 'double';
      else
        error('Unknown pixel type');
      end      
    elseif strcmp(key,'ElementDataFile')
        if ~strcmp(value,'LOCAL')
            %Add the full path if it's not specified
            [dir,file,ext,ver] = fileparts(value);
            if (strcmp(dir,'') || (strcmp(dir,'.')))                
                value = [directory '/' file '' ext];
            end            
            fclose(fid);          
            fid = fopen(value,'rb');
        end
        dflag = 0;
    end
    
  end
  
  if length(isize) ~= dims
      error('Number sizes != dims');
  end
  nelem = prod(double(isize));

  if binarydata
    if exist('byteorder')
        vol = fread(fid,nelem,etype,0,byteorder);
    else
        vol = fread(fid,nelem,etype);
    end
  else
    if strcmp(etype,'float') || strcmp(etype,'double')
      vol = fscanf(fid,'%f',nelem);
    else
      vol = fscanf(fid,'%d',nelem);
    end
  end

  vol = cast(vol,etype);
  vol = reshape(vol,isize');
  
  % Transpose input vol to output correct image
  % Only account for 2d and 3d images
  % Unclear about higher dimensional images
  dims = ndims(vol);
  if dims == 2
    vol = vol';
  elseif dims == 3
    vol = permute(vol, [2 1 3]);
  end
  
  if nargout > 1
    meta = struct('spacing',{0});
    if exist('spacing')
        %setfield(meta,'spacing', spacing);
        meta.spacing = spacing;
    end
    if exist('origin')
        meta.origin = origin;
    end
  end
