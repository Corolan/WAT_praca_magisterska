function data_analyser

format bank %format - dwa miejsca dziesiętne
disp("Welcome!")

%-----------Pobranie danych----------------
%filename="test.txt"; % plik z danymi
filename="rnd_data.txt"; % plik z danymi
if isfile(filename)
     % File exists.
     disp("Znaleziono plik... Analizuję...");
else
    warningMessage = sprintf('Warning: file does not exist:\n%s', filename);
    uiwait(msgbox(warningMessage));
    exit;
end

B=readmatrix(filename); % wczytanie pliku
[rows, ~]=size(B);%rozmiar wczytanego pliku

sensor1_data=zeros(rows,3); % alokacja pamięci na dane z serwerów
indexS1=1; % indeks pomocniczy do rozdzielania danych
sensor2_data=zeros(rows,3);
indexS2=1;

%-----------Rozdzielenie danych----------------
for i = 1:1:rows
    if B(i,1) == 0
        sensor1_data(indexS1, 1)= B(i, 2);
        sensor1_data(indexS1, 2)= B(i, 3);
        sensor1_data(indexS1, 3)= B(i, 4);
        indexS1=indexS1+1;
    elseif B(i,1) == 1
        sensor2_data(indexS2, 1)= B(i, 2);
        sensor2_data(indexS2, 2)= B(i, 3);
        sensor2_data(indexS2, 3)= B(i, 4);
        indexS2=indexS2+1;
    else
        disp("Błąd parsowania linii:\n")
        disp(B(i, :));
    end  
end

sensor1_data( ~any(sensor1_data,2), : ) = [];  %usuwanie rzędów wypełnionych samymi zerami
sensor2_data( ~any(sensor2_data,2), : ) = []; %

%-----------Analiza danych----------------

analyseData(sensor1_data, 1);
analyseData(sensor2_data, 2);
end

function analyseData(data, nmb_of_sensor)
    [data_rows, ~] = size(data);
    temp=data(:,1);
    avg_temp=mean(temp);
    max_temp=max(temp);
    min_temp=min(temp);
    std_temp=std(temp);
    
    pressure=data(:,2);
    avg_press=mean(pressure);
    max_press=max(pressure);
    min_press=min(pressure);
    std_press=std(pressure);
    
    humidity=data(:,3);
    avg_hum=mean(humidity);
    max_hum=max(humidity);
    min_hum=min(humidity);
    std_hum=std(humidity);
    
    x = linspace(0,1,data_rows);
    
    plot_title=sprintf("Sensor %d", nmb_of_sensor);
    figure('Name',plot_title, 'NumberTitle','off');
    
    subplot(2,2,1)
    plot(x, temp, 'ro-');
    hold on;
    yline(avg_temp, 'b:','LineWidth',2);
    title("Sensor " + nmb_of_sensor+ " - temperatura");
    xlabel('Kolejne pomiary');
    ylabel(['temperatura [' char(176) 'C]']);
    hold off;
    
    subplot(2,2,2)
    plot(x, pressure, 'ro-');
    hold on;
    yline(avg_press, 'b:','LineWidth',2);
    title("Sensor " + nmb_of_sensor+ " - ciśnienie");
    xlabel('Kolejne pomiary');
    ylabel('ciśnienie [kPa]');
    hold off;
    
    subplot(2,2,3)
    plot(x, humidity, 'ro-');
    hold on;
    yline(avg_hum, 'b:','LineWidth',2);
    title("Sensor " + nmb_of_sensor+ " - wilgotność");
    xlabel('Kolejne pomiary');
    ylabel('wilgotność względna [%]');
    hold off;
    
    analysis_params = {sprintf('Średnia temperatura: %.2f%cC\n', avg_temp,char(176)), ...
                       sprintf('Temperatura zawiera się w przedziale: %.2f%cC do %.2f%cC\n', min_temp,char(176), max_temp,char(176)), ...
                       sprintf('Średnie ciśnienie: %.2f kPa\n', avg_press), ...
                       sprintf('Ciśnienie zawiera się w przedziale: %.2f kPa do %.2f kPa\n', min_press, max_press), ...
                       sprintf('Średnia wilgotność: %.2f %%\n', avg_hum), ...
                       sprintf('Wilgotność zawiera się w przedziale: %.2f%% do %.2f%%\n', min_hum, max_hum), ...
                       %sprintf('Temperatura: %.2f%cC %c  %.2f%cC', avg_temp, char(176), char(177), std_temp, char(176)),
                       };
    
    
    subplot(2,2,4)
    text(0,0.60,analysis_params);
    set(gca,'XTick',[], 'YTick', []);
    set(gca,'visible','off')
    
end