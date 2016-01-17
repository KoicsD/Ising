clear all
close all
clc
disp('Adja meg a spinmátrix méretét!')
disp('A kilépéshez írja: 0')
while true
command=input('Ising>>','s');
if (strcmp(command,'0'))
    return
end
try
    msize=str2double(command);
    break
catch ME
    disp('A megadott paraméter helytelen. Próbálja újra!')
    disp('A kilépéshez írja: 0')
end
end
disp('Adja meg az egy generáció egyedszámát!')
disp('A kilépéshez írja: 0')
while true
command=input('Ising>>','s');
if (strcmp(command,'0'))
    return
end
try
    gensize=str2double(command);
    break
catch ME
    disp('A megadott paraméter helytelen. Próbálja újra!')
    disp('A kilépéshez írja: 0')
end
end
disp('Adja meg a párosodással létrejövõ generációk számát!')
disp('A kilépéshez írja: 0')
while true
command=input('Ising>>','s');
if (strcmp(command,'0'))
    return
end
try
    maxgen=str2double(command);
    break
catch ME
    disp('A megadott paraméter helytelen. Próbálja újra!')
    disp('A kilépéshez írja: 0')
end
end
disp('Adja meg a kimeneti fájl elérési útját és nevét!')
disp('A kilépéshez írja: 0')
command=input('Ising>>','s');
if (strcmp(command,'0'))
    return
end
filename=command;
fid=fopen('Caller.m','w');
if fid ~= -1
    try
    fprintf(fid,['!Ising ' num2str(msize) ' ' num2str(gensize) ' ' num2str(maxgen) ' ' filename]);
    fclose(fid);
    catch ME
        disp('Command for console application cannot be constructed.')
        fclose(fid);
        return
    end
else
    disp('Command for console application cannot be constructed.')
    return
end
Caller
fid = fopen(filename);
if fid ~= -1
    Matrix = zeros(gensize+1,maxgen+1);
    ind = 0;
    try
        while 1
            line=fgetl(fid);
            if ~ischar(line)
                fclose(fid);
                break
            end
            if length(line) >= 15 && strcmp(line(1:13), '<Generation #')
                j = str2double(line(14:end-1)) + 1;
            end
            if length(line) >= 11 && strcmp(line(1:9), '<Entity #')
                i = str2double(line(10:end-1));
            end
            if length(line) > 6 && strcmp(line(1:6), 'Eps = ')
                Matrix(i,j) = str2double(line(7:end));
            end
            ind = ind + 1;
        end
    catch ME
        disp('Hiba az eredményfájl olvasásakor.')
        fclose(fid);
    end
    Matrix(end,:) = mean(Matrix(1:end-1,:));
    plot(0:maxgen,Matrix(end,:));
    xlabel('Generáció sorszáma')
    ylabel('Egy spinre jutó energia')
    saveas(figure(1),[filename '.png'],'png')
else
    disp('Hiba az eredményfájl megnyitásakor.')
end