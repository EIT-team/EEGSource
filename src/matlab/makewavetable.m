% script to make table of values for DAC output for arduino due skech
% "simple waveform generator" for a given output frequency and given
% sampling interval (Fs) the values are computed and then converted into
% the correct format - hex

%orginial A=2020 dc offset 2048, modified (from calibration) A=2000 offset
%2020

A=2000; %amplitude approx max of 12bit DAC
Offset=A; 
F= 10; % frequency of sinewave in HZ
T=1/F; %period of wave
ts=2e-6; %time between samples - this is determined by the speed of the arduino
t=0:ts:T-ts; %full time vector
y=A*sin(2*pi*F*t)+Offset; %sine values plus offset
yint=int16(y); %make values  integers
ystr=strjoin(cellstr(num2str(yint')),',');

if min(yint) < 0 || max(yint) > 2^12
    warning('out of range');
end


ArrayDefStr=strcat('static uint16_t waveformsTable[maxSamplesNum] = {',ystr,'};');

fid=fopen('Waveforms.h','w+');
fprintf(fid,'%s\n',ArrayDefStr);
fclose(fid);