% Generate synthetic digit dataset (white numbers on red background)
% Create directory structure
if ~exist('SyntheticDigits', 'dir')
    mkdir('SyntheticDigits');
    for i = 0:9
        mkdir(fullfile('SyntheticDigits', num2str(i)));
    end
end

% Generate 100 images for each digit (0-9)
for digit = 0:9
    for i = 1:100
        % Create a 28x28 image with red background
        img = ones(28, 28, 3) .* cat(3, 1, 0, 0); % Red background (R=1, G=0, B=0)
        
        % Create a figure with invisible axes to render the digit
        figHandle = figure('Visible', 'off');
        axesHandle = axes('Visible', 'off', 'Position', [0 0 1 1]);
        
        % Randomize position slightly and rotation
        xPos = 0.5 + (rand() - 0.5) * 0.1;
        yPos = 0.5 + (rand() - 0.5) * 0.1;
        rotation = (rand() - 0.5) * 20; % Random rotation ±10 degrees
        
        % Plot the digit in white
        textHandle = text(xPos, yPos, num2str(digit), 'FontSize', 18, ...
            'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle', ...
            'Color', 'white', 'FontWeight', 'bold', 'Rotation', rotation);
        
        % Set axes limits and aspect ratio
        xlim([0 1]);
        ylim([0 1]);
        pbaspect([1 1 1]);
        
        % Capture the figure as an image
        frame = getframe(axesHandle);
        close(figHandle);
        
        % Resize to 28x28 and convert to RGB
        digitImg = imresize(frame.cdata, [28 28]);
        
        % Save the image
        filename = fullfile('SyntheticDigits', num2str(digit), sprintf('digit_%d_%03d.png', digit, i));
        imwrite(digitImg, filename);
    end
end

% Load the synthetic dataset
digitDatasetPath = fullfile('SyntheticDigits');
imds = imageDatastore(digitDatasetPath, 'IncludeSubfolders', true, 'LabelSource', 'foldernames');

% Display sample images
figure;
perm = randperm(1000, 20); % 1000 total images (100 per digit)
for i = 1:20
    subplot(4, 5, i);
    imshow(imds.Files{perm(i)});
    title(string(imds.Labels(perm(i))));
end

% Check dataset properties
labelCount = countEachLabel(imds)
img = readimage(imds, 1);
size(img)  % Should output [28 28 3]

% Split into training and validation sets
trainRatio = 0.8; % 80% for training, 20% for validation
numTrainFiles = round(trainRatio * 100); % 100 images per class
[imdsTrain, imdsValidation] = splitEachLabel(imds, numTrainFiles, 'randomize');

% Define CNN architecture
layers = [
    imageInputLayer([28 28 3])  % RGB input

    convolution2dLayer(3, 8, 'Padding', 'same')
    batchNormalizationLayer
    reluLayer

    maxPooling2dLayer(2, 'Stride', 2)

    convolution2dLayer(3, 16, 'Padding', 'same')
    batchNormalizationLayer
    reluLayer

    maxPooling2dLayer(2, 'Stride', 2)

    convolution2dLayer(3, 32, 'Padding', 'same')
    batchNormalizationLayer
    reluLayer
    
    fullyConnectedLayer(10)
    softmaxLayer
    classificationLayer];

% Training options
options = trainingOptions('sgdm', ...
    'InitialLearnRate', 0.01, ...
    'MaxEpochs', 10, ...
    'Shuffle', 'every-epoch', ...
    'ValidationData', imdsValidation, ...
    'ValidationFrequency', 30, ...
    'Verbose', false, ...
    'Plots', 'training-progress');

% Train the network
net = trainNetwork(imdsTrain, layers, options);

% Evaluate the network
YPred = classify(net, imdsValidation);
YValidation = imdsValidation.Labels;
accuracy = sum(YPred == YValidation) / numel(YValidation);
fprintf('Validation accuracy: %f\n', accuracy);

% Create confusion chart
figure;
confusionchart(YValidation, YPred);
title('Confusion Chart for Digit Recognition');

% Function to test with a custom image
function predictDigit(net, imagePath)
    % Read and display the image
    img = imread(imagePath);
    figure;
    imshow(img);
    
    % Resize to match network input size if needed
    if ~isequal(size(img), [28, 28, 3])
        img = imresize(img, [28, 28]);
    end
    
    % Predict the digit
    prediction = classify(net, img);
    title(['Predicted: ', char(prediction)]);
end

% Example of how to use the function to test a new image
% After training, you can use this to test:
% predictDigit(net, 'path_to_test_image.png');

% Function to create a test image with a user-specified digit
function createTestImage(digit)
    % Create a 28x28 image with red background
    img = ones(28, 28, 3) .* cat(3, 1, 0, 0); % Red background
    
    % Create a figure with invisible axes to render the digit
    figHandle = figure('Visible', 'off');
    axesHandle = axes('Visible', 'off', 'Position', [0 0 1 1]);
    
    % Plot the digit in white
    textHandle = text(0.5, 0.5, num2str(digit), 'FontSize', 18, ...
        'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle', ...
        'Color', 'white', 'FontWeight', 'bold');
    
    % Set axes limits and aspect ratio
    xlim([0 1]);
    ylim([0 1]);
    pbaspect([1 1 1]);
    
    % Capture the figure as an image
    frame = getframe(axesHandle);
    close(figHandle);
    
    % Resize to 28x28
    testImg = imresize(frame.cdata, [28 28]);
    
    % Save the image
    filename = sprintf('test_digit_%d.png', digit);
    imwrite(testImg, filename);
    
    fprintf('Test image for digit %d created as %s\n', digit, filename);
end

% Example of how to create a test image
% After training, you can use this to create a test image:
% createTestImage(5); % Creates an image of digit 5