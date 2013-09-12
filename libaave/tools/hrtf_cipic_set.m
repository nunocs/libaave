load('hrir_final.mat'); % subject_008

elevation = 0;
elevation_index = (elevation + 45) * 64 / 360 + 1;

hrir = zeros(25 * 2, 200);

for azimuth_index = 1 : 25
	for sample_index = 1 : 200
		hrir(azimuth_index * 2 - 1, sample_index) = hrir_l(azimuth_index, elevation_index, sample_index);
		hrir(azimuth_index * 2 - 0, sample_index) = hrir_r(azimuth_index, elevation_index, sample_index);
	end
end

l = zeros(1, 200);
r = zeros(1, 200);
for i = 1 : 200
	l(i) = hrir_l(1, elevation_index, i);
	r(i) = hrir_r(1, elevation_index, i);
end
save('-ascii', 'cipic_hrir_l.txt', 'l');
save('-ascii', 'cipic_hrir_r.txt', 'r');

save('-ascii', 'cipic_hrir.txt', 'hrir');
