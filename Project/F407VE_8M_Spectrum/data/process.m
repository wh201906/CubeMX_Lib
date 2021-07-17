baseLine = [ ...
    "800us_20us_nowave_10kres_10kstep_1.txt" 
    "800us_20us_nowave_10kres_10kstep_2.txt" 
    "800us_20us_nowave_10kres_10kstep_3.txt" 
].';
fileName = '2800us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt';

% show baseline
% baseid = 0;
% for baseName = baseLine
%     baseName
%     mat = csvread(baseName);
%     mat = mat.';
%     tmp = mat(2,:);
%     tmp = filter(ones(1,windowSize)/windowSize,1,tmp);
%     plot(mat(1,:),tmp+baseid*0.1)
%     hold on
%     baseid = baseid+1;
% end


% show waves
mat = csvread(fileName);
mat = mat.';
x = mat(1,:);
y_raw = mat(2,:);
plot(x,y_raw);
hold on

windowSize = 5;
y_mvave = filter(ones(1,windowSize)/windowSize,1,y_raw);
plot(x,y_mvave+0.2);