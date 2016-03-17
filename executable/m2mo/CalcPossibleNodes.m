function PossbileNodeTx = CalcPossibleNodes(DestNode, M, P_MPR, MPR_Th)

NodeNum = length(M);
loc = find(M(1,:) == 1);
loc(find(loc == DestNode)) = [];

RatioVec = zeros(1, length(loc));
for ind = 1: length(loc)
    if P_MPR(1,loc(ind)) > MPR_Th
        RatioVec(ind) = P_MPR(1,loc(ind));
    end
end
pos = find(RatioVec == 0);
loc(pos) = [];
RatioVec(pos) = [];

TotalProb = sum(P_MPR(1,loc));
RatioVec = RatioVec ./ TotalProb;

if any(loc)
    %r = rand(1,1);
    %x = sum(r >= cumsum([0, RatioVec]));
    PossbileNodeTx = [DestNode, loc];
else
    PossbileNodeTx = DestNode;
end    