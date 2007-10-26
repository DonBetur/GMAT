function OpenGMAT()

global gmatService;
global gmatTopic;
global gmatChannel;

gmatService = '4242';
gmatTopic = 'GMAT-MATLAB';

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to initialize
% conversion with GMAT
%-----------------------------------------------------------
if (computer == 'PCWIN')
   gmatChannel = ddeinit(gmatService, gmatTopic);
else
   disp('OpenGMAT(): Only PC windows is supported at this time');
end

if (gmatChannel == 0)
   disp('GMAT Server has not started. Please start the server first!');
else
   CallGMAT('Open', '');
end

