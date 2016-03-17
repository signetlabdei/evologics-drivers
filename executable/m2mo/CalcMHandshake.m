function [M_1, P_MPR, P_IdealMPR] = CalcMHandshake(DistMat, CheadID, ChannelParam)

N = size(DistMat,1);

Gamma = ChannelParam(1);
Alfa = ChannelParam(2);
m = ChannelParam(3);
SL = ChannelParam(4);
NL = ChannelParam(5);
BW = ChannelParam(6);
Th = ChannelParam(7);

M_1 = diag(ones(1,N));
P_MPR = diag(ones(1,N));
P_IdealMPR = diag(ones(1,N));
for NodeInd = 1: N
    if NodeInd ~= CheadID
        for JammNodeInd = 1: N
            if (JammNodeInd ~= CheadID) && (JammNodeInd ~= NodeInd)
                %check the relative SNIR
                TL = 10 * Gamma * log10(DistMat(CheadID,NodeInd)) + Alfa * DistMat(CheadID,NodeInd) / 1000;
                RL1 = SL - TL;
                TL = 10 * Gamma * log10(DistMat(CheadID,JammNodeInd)) + Alfa * DistMat(CheadID,JammNodeInd) / 1000;
                RL2 = SL - TL;
                
                rl2 = 10^(RL2/10);
                nl = 10^((NL+10*log10(BW))/10);
                SNIR = RL1 - 10*log10(nl+rl2);
                snir = 10 ^ (SNIR / 10);
                %calculate symbol error rate assuming BPSK
                PeSymbol = (m - 1) / m * erfc(sqrt(snir));
                if PeSymbol < Th
                    M_1(NodeInd, JammNodeInd) = 1;
                    
                    %calculate MPR probability
                    SIR = RL1 - RL2;
                    sir = 10^(SIR/10);
                    MPRprob = 1 - (m - 1) / m * erfc(sqrt(sir));
                    P_MPR(NodeInd, JammNodeInd) = MPRprob;
                    %also calculate the other case in case near-far
                    SIR = RL2 - RL1;
                    sir = 10^(SIR/10);
                    MPRprob = 1 - (m - 1) / m * erfc(sqrt(sir));
                    P_MPR(JammNodeInd, NodeInd) = MPRprob;        
                    
                    %ideal MPR
                    P_IdealMPR(NodeInd, JammNodeInd) = 1;
                    P_IdealMPR(JammNodeInd, NodeInd) = 1;
                end
            end
        end
    end
end