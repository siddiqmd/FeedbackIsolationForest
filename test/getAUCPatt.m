
function auc = getAUCPatt(MAX,dataname)
	d='output/';
	result = zeros(6,14);
	t = 1;
	for type = {'IF', 'PA', 'PM', 'adp.IF', 'adp.PA', 'adp.PM'}
		s = 1;
	    for Size = {'16','32','64','128','256','512','1024','2048','4096','8192','16384','32768','65536','131072'}
			auc = zeros(1,MAX);
			for rep = 1:MAX
				f = strcat(dataname, '.', num2str(Size{1}), '.', num2str(rep-1), '.', num2str(type{1}), '.csv');
				if exist(strcat(d, f), 'file') > 0
					disp(f);
					X = readtable(strcat(d, f));
					DATA = table2array(X(:,2));
					LAB = table2array(X(:,1));
					[~, ~, ~, auc(rep)] = perfcurve(LAB, -DATA, 'anomaly');
				else
					auc(rep) = 0;
				end
			end
			result(t, s) = mean(auc);
			s = s + 1;
	    end
		t = t + 1;
	end
	csvwrite(strcat('AUC.',dataname,'.csv'), result);
end
