% Author: Cory Jones
function varargout = imagesc3d(varargin)

% setappdata(0,'image',im);
% H = display3dGUI;
% if nargout==1
%     varargout{1} = H;
% end
% rmappdata(0,'image');
if ischar(varargin{1})
    eval([varargin{1} '()']);
%     hObject = varargin{2};
%     eventdata = varargin{3};
%     handles = varargin{4};
%     eval([fun_call '(hObject,eventdata,handles)']);
else

    %Display image
    if nargin>1
        range = varargin{2};
    else
       range = getrangefromclass(varargin{1});
    end
    handles.figure = figure(100+floor(900*rand(1)));
    set(handles.figure,'Units','character');
    figPos = get(handles.figure,'Position');
    handles.slider1 = uicontrol('Style','slider','Units','character',...
                               'Position',[0 0 figPos(3)-.5 1.5],'Visible','on');
    handles.axes1 = axes;
    set(handles.axes1,'Units','character');
    set(handles.axes1,'Position',[3.1 4.6 figPos(3)-4.7 figPos(4)-3.2]);
    handles.range = range;
    imagesc(squeeze(varargin{1}(:,:,1,:)));
    set(handles.figure, 'ResizeFcn','imagesc3d(''figure1_ResizeFcn'')');
    
    %Create Title Text
    handles.title = uicontrol('Style','text','Units','character',...
                              'BackgroundColor',get(handles.figure,'Color'));
    
    %Create slider
    set(handles.slider1,'Callback','imagesc3d(''slider1_Callback'')');
    if isequal(get(handles.slider1,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
        set(handles.slider1,'BackgroundColor',[.9 .9 .9]);
    end
    im = varargin{1};
    nSlices = size(im,3);
    if nSlices~=1
        set(handles.slider1,'value',1);
        set(handles.slider1,'SliderStep',[1/(nSlices-1) 1/(nSlices-1)]);
        set(handles.slider1,'Min',1);
        set(handles.slider1,'Max',nSlices);
        
        handles.titletext = ['Slice ' num2str(1)];
        set(handles.title,'Position',[floor((figPos(3)-length(handles.titletext))/2), figPos(4)-2.5, length(handles.titletext), 2]);
        set(handles.title,'String',handles.titletext);
    end
    
    %Create axes
    
    %Hide slider if only 2D
    if size(im,3)==1
        set(handles.slider1,'Visible','off');
        axesPos = get(handles.axes1,'Position');
        axesPos(1:2) = .1;
        axesPos(3:4) = figPos(3:4)-.2;
        set(handles.axes1,'Position',axesPos);
    end
    setappdata(handles.figure, 'handles',handles);
    setappdata(handles.figure, 'image', im);
end
if nargout==1
    varargout{1} = handles.figure;
end


% --- Executes on slider movement.
function slider1_Callback()
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
im = getappdata(gcf,'image');
handles = getappdata(gcf,'handles');
slice = get(handles.slider1,'value');
slice = round(slice);
set(handles.slider1,'value',slice);

figPos = get(handles.figure, 'Position');
handles.titletext = ['Slice ' num2str(slice)];
set(handles.title,'Position',[floor((figPos(3)-length(handles.titletext))/2), figPos(4)-2.5, length(handles.titletext), 2]);
set(handles.title,'String',handles.titletext);

imagesc(squeeze(im(:,:,slice,:)));

% --- Executes during object creation, after setting all properties.
% function slider1_CreateFcn(hObject, eventdata, handles)
% % hObject    handle to slider1 (see GCBO)
% % eventdata  reserved - to be defined in a future version of MATLAB
% % handles    empty - handles not created until after all CreateFcns called
% 
% % Hint: slider controls usually have a light gray background.
% if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
%     set(hObject,'BackgroundColor',[.9 .9 .9]);
% end
% im = getappdata(0,'image');
% nSlices = size(im,3);
% if nSlices~=1
%     set(hObject,'value',1);
%     set(hObject,'SliderStep',[1/(nSlices-1) 5/(nSlices-1)]);
%     set(hObject,'Min',1);
%     set(hObject,'Max',nSlices);
% end


% --- Executes when figure1 is resized.
function figure1_ResizeFcn()
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles = getappdata(gcf,'handles');
figPos = get(handles.figure,'Position');
sliderPos = get(handles.slider1, 'Position');
sliderPos(3) = figPos(3)-.4;
sliderPos(1) = 0;
axesPos = get(handles.axes1,'Position');
axesPos(3) = figPos(3)-6;
axesPos(4) = figPos(4)-axesPos(2)-3;
set(handles.axes1,'Position',axesPos);
set(handles.slider1,'Position',sliderPos);
set(handles.title,'Position',[floor((figPos(3)-length(handles.titletext))/2), figPos(4)-2.5, length(handles.titletext), 2]);
