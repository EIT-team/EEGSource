% script to make table of values for DAC output for arduino due skech
% "simple waveform generator" for a given output frequency and given
% sampling interval (Fs) the values are computed and then converted into
% the correct format - hex

%orginial A=2020 dc offset 2048, modified (from calibration) A=2000 offset
%2020

load('EEG_chn');

A=2000; %amplitude approx max of 12bit DAC
Offset=A; 

scf=A/max(abs(Vdaq));

y=Vdaq*scf + Offset;

%%

Fs=50000;

ts=1/Fs; %time between samples - this is determined by the speed of the arduino
T=0.5;
t=0:ts:T-ts; %full time vector
yint=int16(y); %make values  integers
ystr=strjoin(cellstr(num2str(yint)),',');

if min(yint) < 0 || max(yint) > 2^12
    warning('out of range');
end


ArrayDefStr=strcat('static uint16_t waveformsTable[maxSamplesNum] = {',ystr,'};');

fid=fopen('WaveformsEEG.h','w+');
fprintf(fid,'%s\n',ArrayDefStr);
fclose(fid);


%% check output after RC filter

% for the due version, the DAC output goes through HP RC filter, so lets
% check how this fucks it up

%RC filter is same as first order butterworth(or any type of filter i
%think)

ysig = [zeros(size(y)) y y y zeros(size(y))];
ysigideal = [zeros(size(y)) y-Offset y-Offset y-Offset zeros(size(y))];

R=10e3;
C=1e-6;

Fc=1/(2*pi*R*C);
[B,A]=butter(1,Fc/Fs,'high');

% figure;
% freqz(B,A,0:0.1:100,Fs);

figure
hold on
plot(filter(B,A,ysig));
plot(ysigideal);
hold off

%% check output after RC filter

% for the due version, the DAC output goes through HP RC filter, so lets
% check how this fucks it up

%RC filter is same as first order butterworth(or any type of filter i
%think)


phase_pad = 45;
phase_coeff = phase_pad/360;

pad_length= round(length(y)*phase_coeff);

y_pad = [y(end-pad_length:end); y; y(1:end-pad_length) ]; %y(1:pad_length)

ysig = [zeros(size(y)); y_pad; zeros(size(y))];
ysigideal = [zeros(size(y)); y_pad-Offset; zeros(size(y))];

R=10e3;
C=1e-6;

Fc=1/(2*pi*R*C);
[B,A]=butter(1,Fc/Fs,'high');

% figure;
% freqz(B,A,0:0.1:100,Fs);

figure
hold on
plot(filter(B,A,ysig));
plot(ysigideal);
hold off

%%

y_padint=int16(y_pad); %make values  integers
y_padstr=strjoin(cellstr(num2str(y_padint')),',');

if min(y_padint) < 0 || max(y_padint) > 2^12
    warning('out of range');
end


ArrayDefStr_pad=strcat('static uint16_t waveformsTable[maxSamplesNum] = {',y_padstr,'};');

fid=fopen('WaveformsEEG_pad.h','w+');
fprintf(fid,'%s\n',ArrayDefStr_pad);
fclose(fid);








