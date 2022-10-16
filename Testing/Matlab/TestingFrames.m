%% Preamble
close all;
clear variables;
format longG;
clc;

%% Parameters
Params.FileName = 'SensorDataTest.csv';			% CSV File Data Name
Params.Date = '2022-09-12';						% Date in YYYY-MM-DD format
Params.User = "oliver";							% User Value in Lower Case
Params.BufLen = 120;							% Buffer Length
Params.BufGrn = 12;								% Buffer Granularity
Params.MinGSR = 0;								% Minimum Value for GSR (Exclusive)
Params.MinBPM = 0;								% Minimum Value for BPM (Exclusive)
Params.MaxGSR = 1000;							% Maximum Value for GSR (Exclusive)
Params.MaxBPM = 230;							% Maximum Value for BPM (Exclusive)
Params.WeiGSR = 1;								% Weighting for GSR in Stress Calculations
Params.WeiBPM = 1;								% Weighting for BPM in Stress Calculations

%% Figure Settings
figure( ...
	"WindowStyle","docked", "Name" ...
	,"Stress Derivation Figures" ...
	...
);

%% Set Up Data Table and Calculated Values
d = readtable(Params.FileName);
d.Properties.VariableNames("Var1") = "ID";
d.Properties.VariableNames("Var2") = "GSR";
d.Properties.VariableNames("Var3") = "BPM";
d.Properties.VariableNames("Var4") = "Tmp";
d.Properties.VariableNames("Var5") = "Dat";
d.Properties.VariableNames("Var6") = "Usr";
d.ID=[];

% Reorder Table
d = movevars(d,"Usr",'Before',"GSR");

% Clean Data
d.Usr = lower(d.Usr);
StartDate = posixtime(datetime(Params.Date)+0);
EndDate = posixtime(datetime(Params.Date)+1);
d.Dat = posixtime(d.Dat);
rowidx = ( ...
	  d.GSR > Params.MinGSR ...
	& d.GSR < Params.MaxGSR ...
	& d.BPM > Params.MinGSR ...
	& d.BPM < Params.MaxBPM ...
	& d.Dat < EndDate ...
	& d.Dat > StartDate ...
	& d.Usr == Params.User ...
);
d = d(rowidx, :);

% Setup Buffer
buf.l = Params.BufLen;
buf.GSR = dsp.AsyncBuffer(buf.l);
buf.BPM = dsp.AsyncBuffer(buf.l);

% Prefill Stats Arrays
Stats.Length = length(d.Dat);
Stats.GSRAve = nan(Stats.Length,1);
Stats.GSRStd = nan(Stats.Length,1);
Stats.GSRCat1 = nan(Stats.Length,1);
Stats.BPMAve = nan(Stats.Length,1);
Stats.BPMStd = nan(Stats.Length,1);
Stats.BPMCat1 = nan(Stats.Length,1);
Stats.StrCat1 = nan(Stats.Length,1);

% Create Arrays from Table
ResGSR = d{:,"GSR"}';
ResBPM = d{:,"BPM"}';

% Calculate Whole Data Averages and Standard Deviations
AveGSR = mean(ResGSR);
StdGSR = std(ResGSR);
AveBPM = mean(ResBPM);
StdBPM = std(ResBPM);

% Fill Buffer With First Set of Values
for i = 1:buf.l
	write(buf.GSR,ResGSR(i));
end
for i = 1:buf.l
	write(buf.BPM,ResBPM(i));
end

% Calculate Buffer Averages and Standard Deviations
GSR = peek(buf.GSR);
Stats.GSRAve(1:buf.l) = mean(GSR);
Stats.GSRStd(1:buf.l) = std(GSR);
BPM = peek(buf.BPM);
Stats.BPMAve(1:buf.l) = mean(BPM);
Stats.BPMStd(1:buf.l) = std(BPM);

% Loop Through Values
for g = Params.BufLen:Params.BufGrn:length(ResGSR)
	% Calculate End of Buffer Position
	gg = min(g + Params.BufGrn - 1,Stats.Length);
	% Fill Granularity amount of vlaues
	for i = g:gg
		write(buf.GSR,ResGSR(i));
	end
	for i = g:gg
		write(buf.BPM,ResBPM(i));
	end

	% Calculate Buffer Averages and Standard Deviations
	GSR = peek(buf.GSR,buf.l);
	Stats.GSRAve(g:gg) = mean(GSR);
	Stats.GSRStd(g:gg) = std(GSR);
	BPM = peek(buf.BPM,buf.l);
	Stats.BPMAve(g:gg) = mean(BPM);
	Stats.BPMStd(g:gg) = std(BPM);
end

%% Type 1 Category Calculations
for i = 1:Stats.Length
	Stats.GSRCat1(i) = floor((ResGSR(i) - AveGSR) / StdGSR);
	Stats.BPMCat1(i) = floor((ResBPM(i) - AveBPM) / StdBPM);
	Stats.StrCat1(i) = floor(mean([(Stats.GSRCat1(i)/(Params.WeiGSR/Params.WeiBPM)),(Stats.BPMCat1(i)/(Params.WeiBPM/Params.WeiGSR))]));
end

%% Type 2 Category Calculations
for i = 1:Stats.Length
	Stats.GSRCat2(i) = floor((ResGSR(i) - Stats.GSRAve(i)) / Stats.GSRStd(i));
	Stats.BPMCat2(i) = floor((ResBPM(i) - Stats.BPMAve(i)) / Stats.BPMStd(i));
	Stats.StrCat2(i) = floor(mean([(Stats.GSRCat2(i)/(Params.WeiGSR/Params.WeiBPM)),(Stats.BPMCat2(i)/(Params.WeiBPM/Params.WeiGSR))]));
end

%% Plot Figures
plottime = datetime(d.Dat, 'ConvertFrom','posixtime');

subplot(6,2,1);
plot(plottime,ResGSR);
title("GSR Values")

subplot(6,2,2);
plot(plottime,ResBPM);
title("BPM Values")

subplot(6,2,3);
plot(plottime,Stats.GSRAve);
title("GSR Averages")

subplot(6,2,4);
plot(plottime,Stats.BPMAve);
title("BPM Averages")

subplot(6,2,5);
plot(plottime,Stats.GSRCat1);
title("GSR Categories Type 1")

subplot(6,2,6);
plot(plottime,Stats.BPMCat1);
title("BPM Categories Type 1")

subplot(6,2,[7,8]);
plot(plottime,Stats.StrCat1);
title("Stress Categories Type 1")

subplot(6,2,9);
plot(plottime,Stats.GSRCat2);
title("GSR Categories Type 2")

subplot(6,2,10);
plot(plottime,Stats.BPMCat2);
title("BPM Categories Type 2")

subplot(6,2,[11,12]);
plot(plottime,Stats.StrCat2);
title("Stress Categories Type 2")