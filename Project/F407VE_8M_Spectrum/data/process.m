baseList = [
    "800us_20us_nowave_10kres_10kstep_1.txt"
    "800us_20us_nowave_10kres_10kstep_2.txt"
    "800us_20us_nowave_10kres_10kstep_3.txt"
    "BASE_OPAMP_TPQ_1.txt"
].';
fileList = [
%     "2000us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "2300us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "2500us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "2800us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "3000us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "3200us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "3500us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "4000us_20us_4V90.123Msin_10kres_10kstep_noamp_1.txt"
%     "1V_OPAMP_TPQ_1.txt"
%     "1.5V_OPAMP_TPQ_1.txt"
%     "0.5V_OPAMP_TPQ_1.txt"
%     "1V_OPAMP_TPQ_2.txt"
%     "5V_OPAMP_1.txt"
%     "3V_OPAMP_1.txt"
%     "5V_OPAMP_2.txt"
%     "1V_OPAMP_2.txt"
%     "1V_OPAMP_1.txt"
%     "1V_OPAMP_3.txt"
    "2021-07-18_16-08-18.txt"
    "2021-07-18_15-53-03.txt"
    "2021-07-18_15-58-32.txt"
    "2021-07-18_16-19-08.txt"
].';
windowSize = 5;
peakNum = 5;
peakThre = @(y) (max(y) - min(y)) / 2 + min(y);
movAve = @(y) filter(ones(1,windowSize)/windowSize,1,y);

close all

% show baseline
figure
i = 1;
for baseName = baseList
    mat = csvread(baseName).';
    subplot(length(baseList),1,i);
    i = i+1;
    hold on;
    
    y = mat(2,:);
    yyaxis left
    plot(mat(1,:),y)
    
    y = movAve(y);
    yyaxis right
    plot(mat(1,:),y)
    
    grid on;
    hold off;
    title(baseName, 'Interpreter', 'none')
end


% show waves
for fileName = fileList
    mat = csvread(fileName).';
    figure
    hold on;
    
    x = mat(1,:);
    y = mat(2,:);
    yyaxis left
    plot(x, y)
    [~,id_peak] = findpeaks(y,'minpeakdistance',2,'minpeakheight',peakThre(y));
%     for id = id_peak
%         text(x(id), y(id), ['(',num2str(x(id)),', ',num2str(y(id)),')'])
%     end

    y = movAve(y);
    yyaxis right
    plot(x, y)
    [~,id_peak] = findpeaks(y,'minpeakdistance',2,'minpeakheight',peakThre(y));
%     for id = id_peak
%         text(x(id), y(id), ['(',num2str(x(id)),', ',num2str(y(id)),')'])
%     end
    
    grid on;
    hold off;
    title(fileName, 'Interpreter', 'none')
end