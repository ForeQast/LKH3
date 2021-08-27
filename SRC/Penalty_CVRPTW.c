#include "LKH.h"
#include "Segment.h"

/**
 * Modified Penlaty function for CVRPTW instances.
 * The basic idea behind this optimization is to exploit the fact
 * that at each call only few "routes" (aka petals) have been
 * modified, and we can retrieve this petals simply by looking
 * at the saved Opt moves in the SwapStack array.
 * The implementantio becames less general (since not all the
 * checked tour are obtained from Opt moves), so in some
 * special (and few) cases the old penalty is still called.
 */

#ifdef CAVA_PENALTY

GainType Penalty_CVRPTW_Old();
static GainType oldPenaltySum;
static int setup_Penalty_CVRPTW();
static Node *setup_node_CVRPTW(Node *N);
static void route_min_node_update(Node *t);
static void update_Penalty_CVRPTW();

GainType Penalty_CVRPTW() {
    GainType DemandSum, CostSum, P = 0, petalP;
    Node *N, *NextN;
    int NC_idx;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;

    if (Swaps && cava_PetalsData) {

        NC_idx = setup_Penalty_CVRPTW();

        while (NC_idx) {

            RouteData *petal = cava_NodeCache[--NC_idx]->PetalId;

            N = petal->minNode;

            if (N->Id > DimensionSaved)
                N -= DimensionSaved;

            CostSum = N->prevCostSum;
            DemandSum = N->prevDemandSum;
            petalP = N->prevPenalty;
            do {
                if ((DemandSum += N->Demand) > Capacity)
                    petalP += DemandSum - Capacity;
                if (CostSum < N->Earliest)
                    CostSum = N->Earliest;
                if (CostSum > N->Latest)
                    petalP += CostSum - N->Latest;
                if ((P + petalP) > oldPenaltySum ||
                    ((P + petalP) == oldPenaltySum && CurrentGain <= 0)) 
                    return CurrentPenalty + (CurrentGain > 0);

                CostSum += N->ServiceTime;

                NextN = (Forward ? SUC(N) : PRED(N));

                int cost = (Forward ? SUC_COST(N) : PRED_COST(N)) - N->Pi -
                           NextN->Pi;
                CostSum += cost / Precision;

                N = (Forward ? SUC(NextN) : PRED(NextN));
            } while (N->DepotId == 0);

            if (CostSum > Depot->Latest)
                petalP += CostSum - Depot->Latest;

            if ((P + petalP > oldPenaltySum ||
                 (P + petalP == oldPenaltySum && CurrentGain <= 0))) 
                return CurrentPenalty + (CurrentGain > 0);

            P += petalP;
            petal->CandPenalty = petalP;
        }

        if (!CurrentPenalty || P < oldPenaltySum ||
            (P == oldPenaltySum && CurrentGain > 0)) {
            
            update_Penalty_CVRPTW();
            return CurrentPenalty + P - oldPenaltySum;

        } else 
            return CurrentPenalty + (CurrentGain > 0);

    } else {

        if (!cava_NodeCache)
            cava_NodeCache = (Node **)calloc(Salesmen + 1, sizeof(Node *));

        if (!cava_PetalsData)
            cava_PetalsData = (RouteData *)calloc(Salesmen + 1,
                                                  sizeof(RouteData));
        P = Penalty_CVRPTW_Old();
        if (P < CurrentPenalty || (P == CurrentPenalty && CurrentGain > 0))
            update_Penalty_CVRPTW();

        return P;
    }
}

int setup_Penalty_CVRPTW() {
    Node *N;
    int touched_routes = 0;
    oldPenaltySum = 0;

    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s) {
        s->t1->PetalId->flag = s->t4->PetalId->flag = 0;
        s->t1->PetalId->minNode = s->t4->PetalId->minNode = NULL;
    }

    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s) {

        if ((N = setup_node_CVRPTW(s->t1)) != NULL)
            cava_NodeCache[touched_routes++] = N;

        if ((N = setup_node_CVRPTW(s->t4)) != NULL)
            cava_NodeCache[touched_routes++] = N;

        route_min_node_update(s->t1);
        route_min_node_update(s->t2);
        route_min_node_update(s->t3);
        route_min_node_update(s->t4);
    }

    cava_NodeCache[touched_routes] = NULL;

    return touched_routes;
}

void route_min_node_update(Node *t) {
    if (!t->PetalId->minNode || t->PetalId->minNode->PetalRank > t->PetalRank)
        t->PetalId->minNode = t;
}

Node *setup_node_CVRPTW(Node *N) {

    if (!N->PetalId->flag) {
        oldPenaltySum += N->PetalId->OldPenalty;
        N->PetalId->flag = 1;
        int DepotId = N->PetalId - cava_PetalsData;

        return DepotId == MTSPDepot ? Depot : NodeSet + Dim - 1 + DepotId;
    }

    return NULL;
}


void update_Penalty_CVRPTW() {
    Node *N, *NextN;
    GainType DemandSum, CostSum, petalP;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;

    for (Node **CN = cava_NodeCache; *CN; ++CN) {

        N = *CN;
        if (N->Id > DimensionSaved)
            N -= DimensionSaved;

        RouteData *CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = CurrId->CandPenalty;

        DemandSum = CostSum = petalP = 0;
        int PetalRank = 0;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);

            N->PetalId = NextN->PetalId = CurrId;
            N->PetalRank = PetalRank++;
            NextN->PetalRank = PetalRank++;

            N->prevDemandSum = NextN->prevDemandSum = DemandSum;
            N->prevCostSum = NextN->prevCostSum = CostSum;
            N->prevPenalty = NextN->prevPenalty = petalP;

            if ((DemandSum += N->Demand) > Capacity)
                petalP += DemandSum - Capacity;
            if (CostSum < N->Earliest)
                CostSum = N->Earliest;
            if (CostSum > N->Latest)
                petalP += CostSum - N->Latest;

            CostSum += N->ServiceTime;
            CostSum += (C(N, NextN) - N->Pi - NextN->Pi) / Precision;

            N = Forward ? SUCC(NextN) : PREDD(NextN);
        } while (N->DepotId == 0);
    }
}

GainType Penalty_CVRPTW_Old() {
    static _Thread_local Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    GainType CostSum, DemandSum, P = 0, petalP;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    int cache_index = 0;

    if (!StartRoute)
        StartRoute = Depot;
    if (StartRoute->Id > DimensionSaved)
        StartRoute -= DimensionSaved;
    N = StartRoute;
    do {
        cava_NodeCache[cache_index++] = CurrentRoute = N;
        CostSum = DemandSum = petalP = 0;
        do {
            if (N->Id <= Dim && N != Depot) {
                if ((DemandSum += N->Demand) > Capacity)
                    petalP += DemandSum - Capacity;
                if (CostSum < N->Earliest)
                    CostSum = N->Earliest;
                if (CostSum > N->Latest)
                    petalP += CostSum - N->Latest;
                if ((P + petalP) > CurrentPenalty ||
                    ((P + petalP) == CurrentPenalty && CurrentGain <= 0)) {
                    StartRoute = CurrentRoute;
                    return CurrentPenalty + (CurrentGain > 0);
                }
                CostSum += N->ServiceTime;
            }
            NextN = Forward ? SUCC(N) : PREDD(N);
            CostSum += (C(N, NextN) - N->Pi - NextN->Pi) / Precision;
            N = Forward ? SUCC(NextN) : PREDD(NextN);
        } while (N->DepotId == 0);
        if (CostSum > Depot->Latest &&
            ((P + (petalP += CostSum - Depot->Latest)) > CurrentPenalty ||
             ((P + petalP) == CurrentPenalty && CurrentGain <= 0))) {
            StartRoute = CurrentRoute;
            return CurrentPenalty + (CurrentGain > 0);
        }
        P += petalP;
        cava_PetalsData[CurrentRoute->DepotId].CandPenalty = petalP;
    } while (N != StartRoute);

    return P;
}

#else

GainType Penalty_CVRPTW() {
    static _Thread_local Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    GainType CostSum, DemandSum, P = 0;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;

    if (!StartRoute)
        StartRoute = Depot;
    if (StartRoute->Id > DimensionSaved)
        StartRoute -= DimensionSaved;
    N = StartRoute;
    do {
        CurrentRoute = N;
        CostSum = DemandSum = 0;
        do {
            if (N->Id <= Dim && N != Depot) {
                if ((DemandSum += N->Demand) > Capacity)
                    P += DemandSum - Capacity;
                if (CostSum < N->Earliest)
                    CostSum = N->Earliest;
                if (CostSum > N->Latest)
                    P += CostSum - N->Latest;
                if (P > CurrentPenalty ||
                    (P == CurrentPenalty && CurrentGain <= 0)) {
                    StartRoute = CurrentRoute;
                    return CurrentPenalty + (CurrentGain > 0);
                }
                CostSum += N->ServiceTime;
            }
            NextN = Forward ? SUCC(N) : PREDD(N);

            CostSum += (C(N, NextN) - N->Pi - NextN->Pi) / Precision;
            N = Forward ? SUCC(NextN) : PREDD(NextN);
        } while (N->DepotId == 0);
        if (CostSum > Depot->Latest &&
            ((P += CostSum - Depot->Latest) > CurrentPenalty ||
             (P == CurrentPenalty && CurrentGain <= 0))) {
            StartRoute = CurrentRoute;
            return CurrentPenalty + (CurrentGain > 0);
        }
    } while (N != StartRoute);
    return P;
}
#endif