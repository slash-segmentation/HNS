function imout = vis_node (initseg, merges, label)

imout = initseg;
i = find(merges(:, 3) == label);
if ~isempty(i)
    q = [merges(i, 1) merges(i, 2)];
    while ~isempty(q) 
        t = q(1);
        q = q(2: end);
        j = find(merges(:, 3) == t);
        if ~isempty(j)
            q = [q [merges(j, 1) merges(j, 2)]];            
        else
            imout(imout == t) = 1000;
        end
    end
else
    disp('leaf!');
    imout(imout == label) = 1000;
end
