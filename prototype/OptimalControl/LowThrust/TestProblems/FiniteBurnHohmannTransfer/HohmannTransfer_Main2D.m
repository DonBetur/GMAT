%% =====  Intializations
clear classes;
clear mex;
clear all;
close all;
clc; clearvars; 
global iGfun jGvar traj igrid

%% =====  Define the phase and trajectory properties

%  set cost function properties
traj                    = Trajectory();
traj.pathFunctionName   = 'HohmannTransferPathFunction2D';
traj.pointFunctionName  = 'HohmannTransferPointFunction2D';
traj.plotFunctionName   = 'HohmannTransferPlotFunction2D';
traj.showPlot           = true();
traj.plotUpdateRate     = 10;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;

%%  Set Time Properties
phase1                           = Phase();
phase1.meshIntervalFractions   = [ -1 -0.7 0 0.4 0.7 1];                
phase1.meshIntervalNumPoints   = [15 10 10 10 15]';
phase1.meshIntervalFractions     = [-1; 1];
phase1.meshIntervalNumPoints     = 20*ones(1,1);
phase1.initialEpoch              = 0;
phase1.finalEpoch                = 3.53494625378467;
phase1.initialTimeLowerBound     = 0;
phase1.initialTimeUpperBound     = 0;
phase1.finalTimeLowerBound       = 3.63494625378467;
phase1.finalTimeUpperBound       = 3.63494625378467;

%% Set state information
phase1.numStates                 = 5;
phase1.initialGuessMode          = 'LinearUnityControl';
phase1.initialGuessState         = [ 1.08182071932204 
    0 
    0
    0.961440367214808  
    1.250]; 
phase1.initialGuessEpoch         = 0;
% phase1.finalGuessState           = [-1.08532129521541
%     0.0613510652631364 
%     -0.0284986796881644
% -0.945224765111322 
% 1.153.09094491226]; 
phase1.finalGuessState           = [ -3
    2
    0
    0.961440367214808 
    1.250]; 
phase1.finalGuessEpoch           = 3.53494625378467;

%% Set state information
%                                 r t rd td m
phase1.initialStateLowerBound    = phase1.initialGuessState;
phase1.initialStateUppperBound   = phase1.initialGuessState;
phase1.finalStateLowerBound      = [-10 -10  -10 -10  .900 ]';
phase1.finalStateUpperBound      = [ 10  10   10  10  1.400]';
phase1.stateLowerBound           = [-10 -10  -10 -10  .900 ]';
phase1.stateUpperBound           = [ 10  10   10  10  1.400]';
phase1.numControls               = 2;
phase1.controlUpperBound         = [1 1]';
phase1.controlLowerBound         = [-1 -1 ]';
phase1.algConstraintLowerBound   = [0]';
phase1.algConstraintUpperBound   = [1]';
phase1.eventConstraintLowerBound = [1.11317784162123 0 ]';
phase1.eventConstraintUpperBound = [1.11317784162123 0 ]';


%% =====  Initialize the trajectory and phases and get ready for run

% Determine the sparsity pattern for the combined 
% cost function and constraint Jacobian
traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();