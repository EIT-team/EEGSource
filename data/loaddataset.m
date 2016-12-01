load_from_eeg=0; %set this if you have the raw data

if load_from_eeg
    
    HDR=sopen('S23May8.eeg');
    V=sread(HDR);
    Veeg=V(:,4); % pick random channel
    Fs=HDR.SampleRate;
    save('SingleChannelEEG','Veeg','Fs','HDR');
    
else
    load('SingleChannelEEG.mat');
end

%%


%% make highpass filter
hpFilt = designfilt('highpassiir','FilterOrder',3, ...
    'PassbandFrequency',0.5,'PassbandRipple',0.2, ...
    'SampleRate',Fs);
% fvtool(hpFilt)


%%


Veeg=filtfilt(hpFilt,Veeg); %filter out low freq fluctuations

% take a small chunk

StartEvent =2; % use the events as starting points because why not

T=Fs*10; % number of samples to take

Veeg=Veeg(HDR.EVENT.POS(StartEvent):HDR.EVENT.POS(StartEvent)+T-1);

Vdaq=detrend(Veeg); %remove any linear stuff just in case

Vdaq_rms= rms(Vdaq);
Vdaq_pp = range(Vdaq);
fprintf('RMS Value of signal is %.3f uV\n',Vdaq_rms);
fprintf('PP Value of signal is %.3f uV\n',Vdaq_pp);


Vdaq=Vdaq./rms(Vdaq); % scale rms to be 1

% interpolate data to sample rate of DAQ - as smooth an output as possible
FsTarget=120000;
InterpFactor=FsTarget/Fs;

%interp data
Vdaq=interp(Vdaq,InterpFactor);
Vdaq([1 length(Vdaq)])=0;

%
% plot(Vdaq)
%write into labview directory because fiddling with paths is a ballache in
%labview
% dlmwrite('..\src\labview\EEG_data.txt',Vdaq);