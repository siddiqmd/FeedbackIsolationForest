
function auc = getAUCPatt(dataname, MAXF)
    type = {'seq_marg', 'seq_drop'};
	d='out/';
	for t = 1:2
        result = zeros(6, MAXF+1);
	    for iter = 1:10
			for rep = 0:MAXF
                if rep == 0
                    f = strcat(d, dataname, '_iter', num2str(iter), '_', num2str(rep), '.csv');
                else
                    f = strcat(d, dataname, '_', type{t}, '_iter', num2str(iter), '_', num2str(rep), '.csv');
                end
				disp(f);
                X = readtable(f);
                DATA = table2array(X(:,2));
                LAB = table2array(X(:,1));
                [~, ~, ~, result(iter,rep+1)] = perfcurve(LAB, DATA, 'anomaly');
			end
        end
        csvwrite(strcat('auc/AUC.',dataname,'.',type{t},'.csv'), result);
    end
end
