% Load the .mat file
data = load('results.mat');

% Access the 'results.pwr.generatedTotalPower' field
if isfield(data, 'results') && isfield(data.results, 'pwr') && isfield(data.results.pwr, 'generatedTotalPower')
    generatedPower = data.results.pwr.generatedTotalPower;
else
    error('The .mat file does not contain "results.pwr.generatedTotalPower".');
end

% Write to a CSV file
csvFileName = 'generated_total_power_min.csv';
writematrix(generatedPower, csvFileName);

disp(['Generated total power data has been written to ', csvFileName]);
