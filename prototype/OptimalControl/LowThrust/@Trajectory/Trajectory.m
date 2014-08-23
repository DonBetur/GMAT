classdef Trajectory < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
        %%  Bound constraint vectors
        costLowerBound
        costUpperBound
        decisionVecLowerBound
        decisionVecUpperBound
        allConLowerBound
        allConUpperBound
        
        %%  Decision vector and cost function and sparsity
        decisionVector
        costFunction
        costSparsity
        constraintSparsity
        sparsityPattern
        
        %%  Key quantities regarding problem dimensions
        phaseList
        numPhases
        numDecisionParams
        numFunctions
        numConstraints
        totalnumDecisionParams
        totalnumConstraints
        
        %%  Linkage config
        %  linkageConfig is a cell array of "pointers" to the phases in
        %  each linkage config.  To join p1,p2, and p3 together, as well
        %  link phase p2 to p4
        %  linkageConfig{1} = {p1,p2,p3}
        %  linkageConfig{2} = {p2,p4}
        linkageConfig
        numLinkageConfigs
        %  An array of Linkage classes, one for each linkage in the
        %  linkageConfig arrays.  For the example above, linkage array
        %  would be of length 3:  (p1->p2), (p2->p3), and (p2->p4)
        linkageList
        %  Array of the number of links in each config.  This is just one
        %  minus the number of phases in each config.
        numLinkages
        
        %  Temp for debugging
        sparsityCost
        sparsityConstraints
        
        %  Indeces into arrays
        stateStartIdx
        decVecStartIdx
        decVecEndIdx
        
        %  User function names
        pathFunctionName  = '';
        pointFunctionName = '';
        plotFunctionName = '';
        
        %  Plotting parameters
        showPlot     = false();
        isOptimizing = false();
        isFinished   = false();
        plotUpdateRate = 5;
        plotUpdateCounter = 1;
        
    end
    
    methods
        
        function obj = Initialize(obj)
            
            %  Get properties of each phase for later book-keeping
            obj.numPhases = length(obj.phaseList);
            if obj.numPhases <= 0
                errorMsg = 'Trajectory requires at least one phase.';
                errorLoc  = 'Phase:Initialize';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            obj.numDecisionParams = 0;
            obj.numFunctions = 1; % CHANGE BACK TO 1
            obj.totalnumDecisionParams = 0;
            obj.totalnumConstraints = 0;
            
            %  Initialize phase data
            for phaseIdx = 1:obj.numPhases
                
                %  Configure user functinos
                obj.phaseList{phaseIdx}.pathFunctionName = ...
                    obj.pathFunctionName;
                obj.phaseList{phaseIdx}.pointFunctionName = ...
                    obj.pointFunctionName;
                
                %  Intialize the current phase
                obj.phaseList{phaseIdx}.Initialize();
                
                % Decision vector properties
                obj.numFunctions = obj.numFunctions + ...
                    obj.phaseList{phaseIdx}.numConstraints;
                obj.numDecisionParams(phaseIdx) = ...
                    obj.phaseList{phaseIdx}.numDecisionParams;
                
                %  Constraint properties
                obj.totalnumDecisionParams = ...
                    obj.totalnumDecisionParams + ...
                    obj.phaseList{phaseIdx}.numDecisionParams;
                obj.totalnumConstraints = obj.totalnumConstraints + ...
                    obj.phaseList{phaseIdx}.numConstraints;
            end
            
            %%  Intialize link data
            linkIdx = 0;
            for configIdx = 1:length(obj.linkageConfig)
                while linkIdx < length(obj.linkageConfig{configIdx})-1
                    linkIdx = linkIdx + 1;
                    obj.linkageList{linkIdx} = LinkageConstraint;
                    obj.linkageList{linkIdx}.Initialize(...
                        obj.phaseList{linkIdx},obj.phaseList{linkIdx+1});
                    obj.totalnumConstraints = obj.totalnumConstraints + ...
                        obj.linkageList{linkIdx}.numLinkageConstraints;
                end
            end
            obj.numLinkages = linkIdx;
            
            %
            obj.SetStateChunkIndeces();
            obj.SetConstraintBounds();
            obj.SetDecisionVectorBounds();
            obj.InitializeSparsityPattern();
            obj = SetInitialGuess(obj);
            obj = SetSparsityPattern(obj);
            obj.WriteSetupReport()

        end
        
        %  Configure start and stop indeces for different chunks fo the c
        %  complete decision vector
        function obj = SetStateChunkIndeces(obj)
            
            %Loop over phases and set start and end indeces for different
            %  chunks of the decision vector
            for phaseIdx = 1:obj.numPhases
                obj.stateStartIdx(phaseIdx) = ...
                    obj.totalnumDecisionParams - ...
                    sum(obj.numDecisionParams(phaseIdx:end)) + 1;
                obj.decVecStartIdx(phaseIdx) = ...
                    obj.stateStartIdx(phaseIdx);
                obj.decVecEndIdx(phaseIdx) = ...
                    obj.stateStartIdx(phaseIdx)...
                    + obj.numDecisionParams(phaseIdx) - 1;
            end
        end
        
        %  Initializes the sparse matrix for the constraint Jacobian
        function obj = InitializeSparsityPattern(obj)
            obj.sparsityPattern = sparse(obj.totalnumConstraints,...
                obj.totalnumDecisionParams);
        end
        
        %  Sets upper and lower bounds on the complete constraint vector
        %  concatenating all phases.
        function obj = SetConstraintBounds(obj)
            
            %  Loop over all phases concatenating constraint vectors
            obj.allConLowerBound = [];
            obj.allConUpperBound = [];
            for phaseIdx = 1:obj.numPhases
                obj.allConLowerBound = [obj.allConLowerBound; ...
                    obj.phaseList{phaseIdx}.allConLowerBound];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.phaseList{phaseIdx}.allConUpperBound];
            end
            
            %  Now loop over the linkage constraints
            for linkIdx = 1:obj.numLinkages
                obj.allConLowerBound = [obj.allConLowerBound; ...
                    obj.linkageList{linkIdx}.lowerBound];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.linkageList{linkIdx}.upperBound];
            end
           
        end
        
        %  Sets upper and lower bounds on the complete decision vector
        %  concatenating all phases.
        function obj = SetDecisionVectorBounds(obj)
            % Loop over all phases
            obj.decisionVecLowerBound = ...
                zeros(obj.totalnumDecisionParams,1);
            obj.decisionVecUpperBound = ...
                zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx = lowIdx + ...
                    obj.phaseList{phaseIdx}.numDecisionParams-1;
                obj.decisionVecLowerBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecLowerBound;
                obj.decisionVecUpperBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecUpperBound;
                lowIdx = highIdx + 1;
            end
        end
        
        function obj = SetInitialGuess(obj)
            % Loop over all phases
            obj.decisionVector = zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx=lowIdx+obj.phaseList{phaseIdx}.numDecisionParams-1;
                obj.decisionVector(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.DecVector.decisionVector;
                lowIdx = highIdx + 1;
            end
        end
        
        function obj = SetSparsityPattern(obj)
            
            %  Intialize the sparse matrices
            %             obj = InitializeSparsityPattern(obj);
            %             obj = SetSparsityPattern_Cost(obj);
            %             obj.sparsityConstraints = obj.sparsityPattern(1:end,:);
            %             obj.SetSparsityPattern_Defect_State();
            %             obj.SetSparsityPattern_Defect_Control();
            %             obj.SetSparsityPattern_Defect_Time();
            %             obj.sparsityPattern = ...
            %                 [obj.sparsityCost; obj.sparsityConstraints];
            
            %  This is a generic algorithm, but it will NOT work in GMAT.
            iGfun = [];
            jGvar = [];
            initialGuess = obj.decisionVector;
            for decIdx = 1:obj.totalnumDecisionParams
                tempVec         = obj.decisionVector;
                tempVec(decIdx) = NaN;
                obj.SetDecisionVector(tempVec);
                conVec   = obj.GetContraintVector();
                costVal  = obj.GetCostFunction();
                totalVec = [costVal;conVec];
                for funIdx = 1:obj.totalnumConstraints + 1
                    if isnan(totalVec(funIdx))
                        iGfun = [iGfun funIdx];
                        jGvar = [jGvar decIdx];
                    end
                end
            end
            %  Set the initial guess back
            obj.sparsityPattern = sparse(iGfun,jGvar,1);
            obj.SetDecisionVector(initialGuess);
        end
        
        %  Sets decision vector provided by optimizer
        function obj = SetDecisionVector(obj,decVector)
            
            %  Set the decision vector on the trajectory.
            %  Maybe this is not needed, maybe we should just pass straight
            %  through to phases.  Not Sure.
            if length(decVector) == obj.totalnumDecisionParams
                obj.decisionVector = decVector;
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Trajectory:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Now loop over phases and set on each phase
            for phaseIdx = 1:obj.numPhases
                obj.phaseList{phaseIdx}.SetDecisionVector(...
                    decVector(obj.decVecStartIdx(phaseIdx):...
                    obj.decVecEndIdx(phaseIdx)));
            end
            
            %  Handle plotting
            obj.PlotUserFunction();
            
        end
        
        %  Compute all constraints
        function [constraintVec] = GetContraintVector(obj)
            %  Loop over the phases and concatenate the constraint vectors
            constraintVec = [];
            for phaseIdx = 1:obj.numPhases
                constraintVec = [constraintVec ; ...
                    obj.phaseList{phaseIdx}.GetContraintVector()];
            end
            
            %  Now loop over the linkage constraints
            for linkIdx = 1:obj.numLinkages
                constraintVec = [constraintVec; ...
                    obj.linkageList{linkIdx}.GetLinkageConstraints()];
            end
        end
        
        %  Compute the cost function.
        function [costFunction] = GetCostFunction(obj)
            costFunction = 0;
            for phaseIdx = 1:obj.numPhases
                costFunction = costFunction + ...
                    obj.phaseList{phaseIdx}.GetCostFunction();
            end
        end
        
        %  Report data about the problem
        function WriteSolutionReport(obj)
            
            phaseIdx = 1;
            
            %  Write out states that have active bound constraints
            decVector = obj.phaseList{phaseIdx}.DecVector.decisionVector;
            if isa(decVector,'gradient')
                decVector = decVector.x;
            end
            upperBounds = obj.phaseList{phaseIdx}.decisionVecUpperBound;
            upperBounds = obj.phaseList{phaseIdx}.decisionVecUpperBound;
            
            %  Write out info on functions that have active constraints
            
        end
        
        function [z,F,xmul,Fmul] = Optimize(obj)
            
            % SNOPT Required Globals
            global igrid iGfun jGvar traj
            obj.Initialize();
            
            %  Set the bounds on the Decision Variables
            zmax = obj.decisionVecUpperBound;
            zmin = obj.decisionVecLowerBound;
            
            % Set the bounds on the NLP constraints
            Fmin = [-Inf;obj.allConLowerBound];
            Fmax = [Inf;obj.allConUpperBound];
            z0   = obj.decisionVector;
            
            %% =====  Initialize the optimizer and execute the problem
            % Set the derivative option
            snseti('Derivative Option',1);
            % Set the derivative verification level
            snseti('Verify Level',-1);
            % Set name of SNOPT print file
            snprint('snoptmain.out');
            % Print CPU times at bottom of SNOPT print file
            snseti('Timing level',3);
            % Echo SNOPT Output to MATLAB Command Window
            snscreen on;
            
            % Set initial guess on basis and Lagrange multipliers to zero
            zmul = zeros(size(z0));
            zstate = zmul;
            Fmul = zeros(size(Fmin));
            Fstate = Fmul;
            ObjAdd = 0;
            % Row 1 is the objective function row
            ObjRow = 1;
            % Assume for simplicity that all constraints
            % are purely nonlinear
            AA      = [];
            iAfun   = [];
            jAvar   = [];
            userfun = 'SNOPTFunctionWrapper';
            
            [iGfun,jGvar] = find(obj.sparsityPattern);
            obj.isOptimizing = true();
            obj.plotUpdateCounter = 1;
            %obj.PlotUserFunction(); %  Plot the guess
            %              load 'c:\temp\initGuess.mat'
            %              z0 = initGuess.x;
            [z,F,xmul,Fmul,info,xstate,Fstate,ns,...
                ninf,sinf,mincw,miniw,minrw]...
                = snsolve(z0,zmin,zmax,zmul,zstate,...
                Fmin,Fmax,Fmul,Fstate,...
                ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar,userfun);
            obj.isOptimizing = false();
            obj.isFinished   = true();
            obj.PlotUserFunction();
            
        end
        
        %  Handles plotting of user function
        function PlotUserFunction(obj)
            
            %  Plot the function if at solution or at rate
            if ~isempty(obj.plotFunctionName)
                if (obj.showPlot && obj.isOptimizing && ...
                        obj.plotUpdateCounter == 1) ||...
                        (obj.showPlot && obj.isFinished);
                    feval(obj.plotFunctionName,obj);
                end
                
                %  Update rate counter
                obj.plotUpdateCounter = obj.plotUpdateCounter + 1;
                if obj.plotUpdateCounter == obj.plotUpdateRate
                    obj.plotUpdateCounter = 1;
                end
            end
            
        end
        
    end
end


%   %  Set sparsity pattern for partial of defects w/r/t state
%         function obj = SetSparsityPattern_Defect_State(obj)
%             for phaseIdx = 1:obj.numPhases
%                 obj.sparsityConstraints(...
%                     obj.startDefectConIdx(phaseIdx):...
%                     obj.endDefectConIdx(phaseIdx),...
%                     obj.stateStartIdx(phaseIdx):...
%                     obj.stateEndIdx(phaseIdx))...
%                     = obj.phaseList{phaseIdx}.GetSparsityPattern_Defect_State();
%             end
%         end
%         
%         %  Set sparsity pattern for partial of defects w/r/t control
%         function obj = SetSparsityPattern_Defect_Control(obj)
%             for phaseIdx = 1:obj.numPhases
%                 obj.sparsityConstraints(...
%                     obj.startDefectConIdx(phaseIdx):...
%                     obj.endDefectConIdx(phaseIdx),...
%                     obj.controlStartIdx(phaseIdx):...
%                     obj.controlEndIdx(phaseIdx))...
%                     = obj.phaseList{phaseIdx}.GetSparsityPattern_Defect_Control();
%             end
%         end
%         
%         %  Set sparsity pattern for partial of defects w/r/t time
%         function obj = SetSparsityPattern_Defect_Time(obj)
%             for phaseIdx = 1:obj.numPhases
%                 obj.sparsityConstraints(...
%                     obj.startDefectConIdx(phaseIdx):...
%                     obj.endDefectConIdx(phaseIdx),...
%                     obj.timeStartIdx(phaseIdx):...
%                     obj.timeEndIdx(phaseIdx))...
%                     = obj.phaseList{phaseIdx}.GetSparsityPattern_Defect_Time();
%             end
%         end
%         
%         %  Set sparsity pattern of the cost function
%         function obj = SetSparsityPattern_Cost(obj)
%             obj.sparsityCost      = obj.sparsityPattern(1,:);
%             obj.sparsityCost(1,:) = 1;
%         end


