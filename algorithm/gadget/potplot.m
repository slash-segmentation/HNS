function potplot (data)
pot = data(:, 1); % potentials
mc = data(:, 2); % max coverings
bin = cell(10, 1);
bin{1} = mc(pot >= 0.9);
bin{2} = mc(pot >= 0.8 & pot < 0.9);
bin{3} = mc(pot >= 0.7 & pot < 0.8);
bin{4} = mc(pot >= 0.6 & pot < 0.7);
bin{5} = mc(pot >= 0.5 & pot < 0.6);
bin{6} = mc(pot >= 0.4 & pot < 0.5);
bin{7} = mc(pot >= 0.3 & pot < 0.4);
bin{8} = mc(pot >= 0.2 & pot < 0.3);
bin{9} = mc(pot >= 0.1 & pot < 0.2);
bin{10} = mc(pot < 0.1);
x = 0.95: -0.1: 0.05;
means = zeros(10, 1);
stds = zeros(10, 1);
for i = 1: 10
    means(i) = mean(bin{i});
    stds(i) = std(bin{i});
end
figure, errorbar(x, means, stds);
axis([0, 1, 0, 1]);