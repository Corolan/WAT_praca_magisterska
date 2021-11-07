function random_data_gen
Tmin=21.00;
Tmax=30.00;
Pmin=1000.00;
Pmax=1020.00;
Hmin=0;
Hmax=100;



nmb_to_gen=200;
file_to_gen="rnd_data.txt"

fid = fopen( file_to_gen, 'wt' );
s=0;

for i = 1:1:nmb_to_gen

    t= Tmin + rand*(Tmax-Tmin);
    p= Pmin + rand*(Pmax-Pmin);
    h= Hmin + rand*(Hmax-Hmin);
    fprintf(fid, "%d,%.2f,%.2f,%.2f\n", s,t,p,h);
    if (s==0)
        s=1;
    elseif (s==1)
        s=0;
    end
end

fclose('all');
end
