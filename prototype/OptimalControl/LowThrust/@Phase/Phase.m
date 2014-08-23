classdef Phase < handle
    %PHASE Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'public')
        
        %%  Scale Factors
        positionScaleFactor
        timeScaleFactor
        massScaleFactor
        controlScaleFactor
        
        %%  Bounds during phase
        positionUpperBound
        positionLowerBound
        velocityUpperBound
        velocityLowerBound
        massLowerBound
        controlUpperBound
        controlLowerBound
        stateUpperBound
        stateLowerBound
        
        %%  Bounds at beginning and end of phase
        initialTimeUpperBound
        initialTimeLowerBound
        finalTimeUpperBound
        finalTimeLowerBound
        initialStateLowerBound
        initialStateUppperBound
        finalStateLowerBound
        finalStateUpperBound
        finalGuessState
        finalGuessEpoch
        
        %%  Initial Guess
        initialEpoch
        finalEpoch
        initialGuessState
        initialGuessEpoch
        initialGuessMode
        initialGuessControl
        
        %%  Phase properties
        % Row vector that contains the number of control points for
        % each mesh interval
        meshIntervalNumPoints
        %  Row vector that the time fraction each mesh makes of the whole
        %  phase.  If meshFraction = [1 1 1], then there are three mesh
        %  intervals, and each one is one third of the phase duration
        meshIntervalFractions
        numStates
        numControls
        
        decisionVector
        timeVector 
        
        %% User functions
        pathFunctionName
        pointFunctionName
        
        %%  Bound vectors for different constraint types. These are
        %  assembled from user inputs.
        algConstraintLowerBound
        algConstraintUpperBound
        eventConstraintLowerBound
        eventConstraintUpperBound
        intConstraintUpperBound
        intConstraintLowerBound
        decisionVecLowerBound
        decisionVecUpperBound
        
    end
    
    properties (SetAccess = 'protected')
        
        %OptimizationVector = DecisionVector;
        numTimeParams = 2;  % For orthogonal methods
        radauPoints
        radauWeights
        radauDiffMatrix
        numRadauPoints
        
        %  Cost and constraint related parameters
        
        numStatePoints
        numControlPoints
        numStateParams
        numControlParams
        numStaticParams = 0;
        numDecisionParams
        defectConstraintVec
        algConstraintVec
        eventConstraintVec
        intConstraintVec
        costFuncIntegral
        costFuncPoint
        defectConLowerBound
        defectConUpperBound
        pathConLowerBound
        pathConUpperBound
        eventConLowerBound
        eventConUpperBound
        allConLowerBound
        allConUpperBound
        numConstraints
        numDefectConstraints
        numPathConstraints
        numEventConstraints
        
        %%  Bookkeeping parameters
        stateStartIdx
        controlStartIdx
        timeStartIdx
        defectStartIdx
        pathStartIdx
        eventStartIdx
        stateEndIdx
        controlEndIdx
        timeEndIdx
        defectEndIdx
        pathEndIdx
        eventEndIdx
        
        %%  Helper classes
        % For evaluating user functions
        PathFunction;% = UserPathFunction();
        % For evaluating user functions
        PointFunction;% = UserPointFunction();
        %  For managinging decision vector
        DecVector;% = DecisionVector();
        %probTranscription();  %  For transcription details
        
    end
    
    properties (SetAccess = 'private')
        numMeshIntervals
    end
    
    methods
        
        %  The constructor
        function obj = Phase()
            obj.DecVector     = DecisionVector;
            obj.PathFunction  = UserPathFunction();
            obj.PointFunction = UserPointFunction();
        end
        
        %  Initialize the phase
        function obj = Initialize(obj)
            
            %  Compute the radau properties for defined mesh properties
            ValidatePhaseConfig(obj);
            obj = ConfigureRadauProperties(obj);
            
            %  Compute bookkeeping properties of the discretization
            %  TODO:  NumStatePoints and numControl Points should come from
            %  transcription helper class
            obj.numStatePoints = obj.numRadauPoints + 1;
            obj.numControlPoints = obj.numRadauPoints;
            obj.numStateParams   = obj.numStates*(obj.numStatePoints );
            obj.numControlParams = obj.numControls*(obj.numControlPoints);
            obj.numDecisionParams = obj.numStateParams + ...
                obj.numControlParams + obj.numStaticParams + ...
                obj.numTimeParams;
            
            obj = SetStateChunkIndeces(obj);
            
            
            %  Initialize the decision vector helper class
            numIntegrals = 0;  % TODO:  should not be hard coded
            obj.DecVector.ConfigureDecisionVector(obj.numStates, ...
                obj.numControls,numIntegrals,obj.numStaticParams,...
                obj.numStatePoints,obj.numControlPoints)
            
            obj = SetInitialGuess(obj);
            
            %  Set the handle for user function on the function data class
            obj.PathFunction.functionName = obj.pathFunctionName;
            obj.PointFunction.functionName = obj.pointFunctionName;
            obj.PreparePathFunction(1);  % Evaluate at first point
            obj.PathFunction.Initialize();
            obj.ComputePathFunctions();
            obj.PreparePointFunction();
            obj.PointFunction.Initialize();
            obj = SetConstraintChunkIndeces(obj);
            obj = SetConstraintBounds(obj);
            obj = SetDecisionVectorBounds(obj);
            obj.ComputeTimeVector();
        end
        
        function obj = SetStateChunkIndeces(obj)
            %  Set start and end indeces of different state paramter types.
            %  These describe where different chunks of the decision vector
            %  begin and end.
            obj.numTimeParams = 2;  % TODO: Hard coded for Radau!
            obj.stateStartIdx = 1;
            obj.stateEndIdx  = obj.stateStartIdx + obj.numStateParams -1;
            obj.controlStartIdx = obj.stateEndIdx + 1;
            obj.controlEndIdx = obj.controlStartIdx + ...
                obj.numControlParams -1;
            obj.timeStartIdx = obj.controlEndIdx + 1;
            obj.timeEndIdx = obj.timeStartIdx + obj.numTimeParams  - 1;
        end
        
        %  MERGE
        function obj = SetConstraintChunkIndeces(obj)
            %  Set start and end indeces of different constraint paramter types.
            %  These describe where different chunks of the contstraint vector
            %  begin and end.
            obj.numDefectConstraints = obj.numStates*obj.numRadauPoints;
            obj.numPathConstraints = obj.numRadauPoints*...
                obj.PathFunction.numAlgFunctions;
            obj.numEventConstraints = obj.PointFunction.numEventFunctions;
            obj.defectStartIdx = 1;
            obj.defectEndIdx  = obj.defectStartIdx + ...
                obj.numDefectConstraints -1;
            obj.pathStartIdx = obj.defectEndIdx + 1;
            obj.pathEndIdx = obj.pathStartIdx + obj.numPathConstraints -1;
            obj.eventStartIdx = obj.pathEndIdx + 1;
            obj.eventEndIdx = obj.eventStartIdx + ...
                obj.numEventConstraints -1;
            obj.numConstraints = obj.numDefectConstraints + ...
                obj.numPathConstraints + obj.numEventConstraints;
        end
        
        %  MERGE
        function obj = SetDecisionVectorBounds(obj)
            
            p1RadauPoints = obj.numRadauPoints+1;
            %  Dimension the vectors
            obj.decisionVecLowerBound = zeros(obj.numDecisionParams,1);
            obj.decisionVecUpperBound = zeros(obj.numDecisionParams,1);
            
            %  Fill in the state vector portion of the bounds
            for i = 1:obj.numStates
                obj.decisionVecLowerBound((i-1)*p1RadauPoints+1) = ...
                    obj.initialStateLowerBound(i);
                obj.decisionVecLowerBound(((i-1)*p1RadauPoints+2):...
                    (i*p1RadauPoints-1)) = obj.stateLowerBound(i);
                obj.decisionVecLowerBound(i*p1RadauPoints) = ...
                    obj.finalStateLowerBound(i);
                obj.decisionVecUpperBound((i-1)*p1RadauPoints+1) = ...
                    obj.initialStateUppperBound(i);
                obj.decisionVecUpperBound(((i-1)*p1RadauPoints+2):...
                    (i*p1RadauPoints-1)) = obj.stateUpperBound(i);
                obj.decisionVecUpperBound(i*p1RadauPoints) = ...
                    obj.finalStateUpperBound(i);
            end
            
            %  Fill in the control vector portion of the bounds
            cnt = obj.numStates*p1RadauPoints+1;
            for i = 1:obj.numControls
                obj.decisionVecLowerBound(cnt:cnt+obj.numRadauPoints-1) = ...
                    obj.controlLowerBound(i);
                obj.decisionVecUpperBound(cnt:cnt+obj.numRadauPoints-1) = ...
                    obj.controlUpperBound(i);
                cnt = cnt+obj.numRadauPoints;
            end
            
            %  Fill in the static params portion of the bounds
            %  TODO: Implement static parameters
            
            %  Fill in the time portion of the bounds
            cnt = obj.numDecisionParams - 1;
            obj.decisionVecUpperBound(cnt:cnt+1) = ...
                [obj.initialTimeUpperBound obj.finalTimeUpperBound]';
            obj.decisionVecLowerBound(cnt:cnt+1) = ...
                [obj.initialTimeLowerBound obj.finalTimeLowerBound]';
            
        end
        
        %  MERGE
        function obj = SetConstraintBounds(obj)
            obj = SetDefectConstraintBounds(obj);
            obj = SetPathConstraintBounds(obj);
            obj = SetEventConstraintBounds(obj);
            % TODO:  concatenate constraint types as we add them
            %             obj.allConLowerBound = [obj.defectConLowerBound;...
            %                 obj.pathConLowerBound; obj.eventConLowerBound];
            %             obj.allConUpperBound = [obj.defectConUpperBound;...
            %                 obj.pathConUpperBound; obj.eventConUpperBound];
            %             obj.numConstraints = length(obj.allConLowerBound);
            
            dStartIdx = obj.defectStartIdx;
            dEndIdx = obj.defectEndIdx;
            pStartIdx = obj.pathStartIdx;
            pEndIdx = obj.pathEndIdx;
            eStartIdx = obj.eventStartIdx;
            eEndIdx = obj.eventEndIdx;
            obj.allConLowerBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConLowerBound;
            obj.allConUpperBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConUpperBound;
            obj.allConLowerBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConLowerBound;
            obj.allConUpperBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConUpperBound;
            obj.allConLowerBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConLowerBound;
            obj.allConUpperBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConUpperBound;
        end
        
        %  MERGE
        function obj = SetDefectConstraintBounds(obj)
            obj.defectConLowerBound = ...
                zeros(obj.numStates*obj.numRadauPoints,1);
            obj.defectConUpperBound = ...
                zeros(obj.numStates*obj.numRadauPoints,1);
        end
        
        %  MERGE
        function obj = SetPathConstraintBounds(obj)
            if obj.PathFunction.hasAlgFunctions
                numPathCon = obj.numRadauPoints*...
                    obj.PathFunction.numAlgFunctions;
                obj.pathConLowerBound = zeros(numPathCon,1);
                obj.pathConUpperBound = zeros(numPathCon,1);
                lowIdx = 1;
                for conIdx = 1:obj.numRadauPoints
                    highIdx = lowIdx + obj.PathFunction.numAlgFunctions - 1;
                    obj.pathConLowerBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintLowerBound;
                    obj.pathConUpperBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintUpperBound;
                    lowIdx = lowIdx + obj.PathFunction.numAlgFunctions;
                end
            else
                obj.pathConLowerBound = [];
                obj.pathConUpperBound = [];
            end
        end
        
        %  MERGE
        function obj = SetEventConstraintBounds(obj)
            obj.eventConLowerBound = obj.eventConstraintLowerBound;
            obj.eventConUpperBound = obj.eventConstraintUpperBound;
        end
               
        %  Compute the initial guess
        function obj = SetInitialGuess(obj)
            
            switch obj.initialGuessMode
                
                case {'LinearNoControl','LinearUnityControl'}
                    
                    % Supply an initial guess
                    p1RadauPoints = obj.numRadauPoints+1;
                    xguess  = zeros(length(obj.initialGuessState)*...
                        p1RadauPoints,1);
                    uguess  = zeros(obj.numControls*obj.numRadauPoints,1);
                    for i=1:obj.numStates
                        xguess((i-1)*p1RadauPoints+1:...
                            i*p1RadauPoints)  = ...
                            linspace(obj.initialGuessState(i),...
                            obj.finalGuessState(i),p1RadauPoints).';
                    end
                    if strcmp(obj.initialGuessMode,'LinearUnityControl')
                        controlVar = 1;
                    else
                        controlVar = 0;
                    end
                    for i=1:obj.numControls
                        uguess((i-1)*obj.numRadauPoints+1:...
                            i*obj.numRadauPoints)  = ...
                            linspace(controlVar,controlVar,...
                            obj.numRadauPoints).';
                    end
                    
                    obj.DecVector.SetStateVector(xguess);
                    obj.DecVector.SetControlVector(uguess);
                    obj.DecVector.SetTimeVector(...
                        [obj.initialEpoch; obj.finalEpoch]);
                    
            end
            
        end
        
        %  Compute Radau points, weights, and differentiation matrix
        function obj = ConfigureRadauProperties(obj)
            
            %  Compute Radau points, weights, and differentiation matrix
            [obj.radauPoints,obj.radauWeights,obj.radauDiffMatrix]...
                = lgrPS(obj.meshIntervalFractions, ...
                obj.meshIntervalNumPoints);
            obj.numRadauPoints = length(obj.radauWeights);
            
        end
        
        %  Validate that the mesh properties are set acceptably
        function ValidateMeshProperties(obj)
            
            %  Check that sizes are consistent
            if size(obj.meshIntervalPoints,2) ~=  ...
                    size(obj.meshIntervalFractions,2)
                errorMsg = ['meshIntervalPoints must have the same' ...
                    'number of columns as meshFraction'];
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Check that each are row vectors
            if size(obj.meshIntervalPoints,1) ~= 1
                errorMsg = 'meshIntervalPoints must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if size(obj.meshIntervalFractions,1) ~= 1
                errorMsg = 'meshFraction must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateBoundProperties(obj)
            
            if  obj.positionUpperBound <= obj.positionLowerBound
                errorMsg = ['positionUpperBound must be greater than' ...
                    'positionLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.velocityUpperBound <= obj.velocityLowerBound
                errorMsg = ['velocityUpperBound must be greater than' ...
                    'velocityLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.massUpperBound <= obj.massLowerBound
                errorMsg = ['massUpperBound must be greater than' ...
                    'massLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateStateProperties(obj)
            
            [rows,cols] = size(obj.initialEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'initialEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.finalEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'finalEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'intialGuessEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessState);
            if rows ~= 7 && cols ~= 1
                errorMsg = 'intialGuessState must be 7x1 vector';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        %  Sets decision vector provided by optimizer
        function obj = SetDecisionVector(obj,decVector)
            
            %  Set the decision vector on the trajectory.
            %  Maybe this is not needed, maybe we should just pass straight
            %  through to phases.  Not Sure.
            if length(decVector) == obj.numDecisionParams
                obj.DecVector.SetDecisionVector(decVector);
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Phase:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
        end
        
        %  Returns the time decision variables in [2x1] matrix form
        function  timeMat = GetTimeMatrix(obj)
            timeMat = reshape(obj.DecVector.decisionVector...
                (obj.timeStartIdx:obj.timeEndIdx),...
                obj.numTimeParams,1);
        end
        
        %  Validates user configuration of the phase
        function obj = ValidatePhaseConfig(obj)
            if length(obj.meshIntervalFractions) ~= ...
                    length(obj.meshIntervalNumPoints) + 1
                errorMsg = ['Length of meshIntervalNumPoints' ...
                    ' must be the one greater than length ' ....
                    ' of meshIntervalFractions' ];
                errorLoc  = 'Phase:ValidatePhaseConfig';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
        end
        
        function [contraintVec] = GetContraintVector(obj)
            % Call path and point functions and then assemble the
            % constraint vector
            obj.ComputePathFunctions();
            obj.ComputePointFunctions();
            if obj.PathFunction.hasDynFunctions
                contraintVec = ...
                    [obj.defectConstraintVec];
            end
            if obj.PathFunction.hasAlgFunctions
                contraintVec = [contraintVec; ...
                    obj.algConstraintVec];
            end
            if obj.PointFunction.hasEventFunctions
                contraintVec = [contraintVec; ...
                    obj.eventConstraintVec];
            end
        end
        
        %  Compute defect constraints
        function defectConstraints = ComputePathFunctions(obj)
            
            stateMat = obj.DecVector.GetStateArray();
            % TODO: This is Radau specific and should not be.
            defectConstraintsLHS = obj.radauDiffMatrix*stateMat;
            
            %  Loop over the number of defect constraints
            rhsMatrix = defectConstraintsLHS*0;
            algIdx    = 1;
            obj.costFuncIntegral = 0; 
            
            %algArray = zeros(obj.numPathConstraints,1);
            %  Initialize integral array if there are integral functions
            if obj.PathFunction.hasIntFunctions
                intArray = zeros(obj.PathFunction.numAlgFunctions,1);
            end
            
            for pointIdx = 1:obj.numRadauPoints % TODO: numMeshPoints
                
                %  Prepare then evaluate the path function
                obj.PreparePathFunction(pointIdx);
                obj.PathFunction.Evaluate();
                
                %  Handle defect constraints
                if obj.PathFunction.hasDynFunctions
                    rhsMatrix(pointIdx,1:obj.numStates) = ...
                        obj.PathFunction.dynFunctions';
                end
                
                %  Handle cost function
                if obj.PathFunction.hasCostFunction
                    obj.costFuncIntegral = obj.costFuncIntegral + ...
                        obj.radauWeights(pointIdx)*...
                        obj.PathFunction.costFunction;
                end
                
                %  Handle algebraic constraints
                if obj.PathFunction.hasAlgFunctions
                    algArray(algIdx:algIdx+...
                        obj.PathFunction.numAlgFunctions-1,1) = ...
                        obj.PathFunction.algFunctions;
                    algIdx = algIdx + obj.PathFunction.numAlgFunctions;
                end
                
                %  Handle integral constraints
                if obj.PathFunction.hasIntFunctions
                    intArray = intArray + obj.radauWeights(pointIdx)*...
                        obj.PathFunction.intFunctions;
                end
                
            end
            timeMat     = GetTimeMatrix(obj);
            finalTime   = obj.DecVector.GetLastTime();
            initialTime = obj.DecVector.GetFirstTime();
            
            %  Set cost function
            if obj.PathFunction.hasCostFunction
                obj.costFuncIntegral = ((finalTime-initialTime)/2)*...
                    obj.costFuncIntegral;
            end
            
            %  Set defect constraints
            if obj.PathFunction.hasDynFunctions
                defectConstraints = defectConstraintsLHS - ...
                    (timeMat(2)-timeMat(1))*0.5*rhsMatrix;
                defectConstraints = reshape(defectConstraints,...
                    obj.numDefectConstraints,1);
                obj.defectConstraintVec = defectConstraints;
            end
            
            %  Set algebraic constraints
            if obj.PathFunction.hasAlgFunctions
                obj.algConstraintVec = algArray;
            end
            
            %  Configure integral constraint input for point function.
            if obj.PathFunction.hasIntFunctions
                intArray = ((finalTime-initialTime)/2)*intArray;
                obj.intConstraintVec = intArray;
            end
            
        end
        
        %  Evaluate the user point functions
        function ComputePointFunctions(obj)
            %  Prepare and call the user function
            obj.PreparePointFunction();
            obj.PointFunction.Evaluate();
            %  Set event constraints if there are any
            if obj.PointFunction.hasEventFunctions
                obj.eventConstraintVec = obj.PointFunction.eventFunctions;
            end
            if obj.PointFunction.hasCostFunction
                obj.costFuncPoint =  obj.PointFunction.costFunction;
            end
            if obj.PointFunction.hasIntegralFunctions
                obj.intConstraintVec = obj.PointFunction.intFunctions;
            end
        end
        
        %  Compute defect constraints
        function costFunction = GetCostFunction(obj)
            %  Warning.  Call to GetCostFunction must ALWAYS be preceded by
            %  call to GetContraintVector()
            costFunction = 0;
            if obj.PathFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncIntegral;
            end
            if obj.PointFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncPoint;
            end
        end
        
        %  Configure input structure for given mesh point
        function inputData = UpdateInputData(obj,pointIdx)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh poin "pointIdx"
            inputData.stateVec =...
                obj.DecVector.GetStateAtMeshPoint(pointIdx);
            inputData.controlVec = ...
                obj.DecVector.GetControlAtMeshPoint(pointIdx);
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePathFunction(obj,pointIdx)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh point "pointIdx"
            obj.PathFunction.stateVec =...
                obj.DecVector.GetStateAtMeshPoint(pointIdx);
            obj.PathFunction.controlVec = ...
                obj.DecVector.GetControlAtMeshPoint(pointIdx);
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePointFunction(obj)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh point "pointIdx"
            obj.PointFunction.initialStateVec =...
                obj.DecVector.GetFirstStateVector();
            obj.PointFunction.finalStateVec = ...
                obj.DecVector.GetLastStateVector();
            obj.PointFunction.initialControlVec = ...
                obj.DecVector.GetLastControlVector();
            obj.PointFunction.intConstraintVec = obj.intConstraintVec;
            obj.PointFunction.intialTime = ...
                obj.DecVector.GetFirstTime();
            obj.PointFunction.finalTime = ...
                obj.DecVector.GetLastTime();
  
        end
        
        %  Configure the time vector
        function ComputeTimeVector(obj)
           initialTime = obj.DecVector.GetFirstTime();
           obj.timeVector = (obj.DecVector.GetLastTime() - initialTime)*...
               (obj.radauPoints + 1)/2 + initialTime;
        end
        
    end
    
end

%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_State(obj)
%             sparsityPattern = abs(sign(obj.radauDiffMatrix));
%         end
%         
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Control(obj)
%             sparsityPattern = eye(obj.numRadauPoints);
%         end
%         
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Time(obj)
%             sparsityPattern = ones(obj.numRadauPoints,2);
%         end