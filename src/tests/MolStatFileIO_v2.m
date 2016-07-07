clear
nt = 300; % number of traces
n = 300; % number of distance points
z = linspace(0,1,n); % trace length
X = [];
X1 = [];
dist = [];
gcm = []; % reading in conductance and counts from molecular input file
gct = []; % reading in conductance and counts from tunneling input file

addpath('/Users/benwu/Documents/molstat-1.3.1/src/tests/') % location of histogram files
for j = 0:nt - 1
        molecularfile = strcat('MolecularContribution',num2str(j),'.dat');
        tunnelingfile = strcat('TunnelingContribution',num2str(j),'.dat');
        if exist(molecularfile,'file') && exist(tunnelingfile, 'file')
            gcm = table2array(readtable(molecularfile));
            gct = table2array(readtable(tunnelingfile));
            X1 = [X1;gcm(:,1);gct(:,1)];
            dist = [dist;transpose(z)];
        end
end

X = [dist,X1];

scatter(X(:,1),X(:,2));
view(2)
set(gca,'yscale','log')
title('MolStat Generated Single Traces')
xlabel('Distance (nm)') % x-axis label
ylabel('Conductance (G/Go)') % y-axis label
colormap(jet)
grid on

data = [dist,log10(X1)];
m = 300;
N = [m,m];

[counts,centers] = hist3(data,N);

centers = transpose(centers);
center = cell2mat(centers);
center = transpose(center);

u = 1;

for i = 1:m
    for j = 1:m
        if counts(i,j) ~= 0;
        X2(u,1) = center(i,1);
        X2(u,2) = center(j,2);
        X2(u,3) = counts(i,j);
        u = u+1;
        end
    end
end

figure()
scatter(X2(:,1),10.^(X2(:,2)),50,X2(:,3),'fill','o')
set(gca,'yscale','log')
title('MolStat Generated 2D Histogram')
xlabel('Distance (nm)') % x-axis label
ylabel('Conductance (G/Go)') % y-axis label
colormap(jet)


