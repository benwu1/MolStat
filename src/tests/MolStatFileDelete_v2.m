addpath('/Users/benwu/Documents/molstat-1.3.1/src/tests/')
nt = 300;

for j = 0:nt-1
        molecularfile = strcat('MolecularContribution',num2str(j),'.dat');
        tunnelingfile = strcat('TunnelingContribution',num2str(j),'.dat');
        if exist(molecularfile,'file') && exist(tunnelingfile,'file')
            delete(strcat('/Users/benwu/Documents/molstat-1.3.1/src/tests/',molecularfile));
            delete(strcat('/Users/benwu/Documents/molstat-1.3.1/src/tests/',tunnelingfile))
        end
end
