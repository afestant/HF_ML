#include <Riostream.h>
#include <TFile.h>
#include <AliRDHFCutsDplustoKpipi.h>
#include <TClonesArray.h>
#include <TParameter.h>

/*
 whichCuts=0, nameCuts="DplustoKpipiFilteringCuts"
 whichCuts=1, nameCuts="DplustoKpipiAnalysisCuts"
 */


AliRDHFCutsDplustoKpipi *makeInputCutsDplustoKpipi(Int_t whichCuts=0, TString nameCuts="DplustoKpipiFilteringCuts", Float_t minc=0.,Float_t maxc=20.)
{
    
    AliRDHFCutsDplustoKpipi* cuts=new AliRDHFCutsDplustoKpipi();
    cuts->SetName(nameCuts.Data());
    cuts->SetTitle(nameCuts.Data());
    
    AliESDtrackCuts* esdTrackCuts=new AliESDtrackCuts();
    esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
    //default
    esdTrackCuts->SetRequireTPCRefit(kTRUE);
    esdTrackCuts->SetRequireITSRefit(kTRUE);
    if(whichCuts==0)esdTrackCuts->SetMinNClustersTPC(50);
    esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
    esdTrackCuts->SetEtaRange(-0.8,0.8);
    esdTrackCuts->SetMinDCAToVertexXY(0.);
    esdTrackCuts->SetPtRange(0.6,1.e10);
    esdTrackCuts->SetMaxDCAToVertexXY(1.);
    esdTrackCuts->SetMaxDCAToVertexZ(1.);
    if(whichCuts==1)esdTrackCuts->SetMinDCAToVertexXY(0.);
    if(whichCuts==0)esdTrackCuts->SetMinDCAToVertexXYPtDep("0.0060*TMath::Max(0.,(1-TMath::Floor(TMath::Abs(pt)/2.)))");
    if(whichCuts==1)esdTrackCuts->SetMinDCAToVertexXYPtDep("0.0060*TMath::Max(0.,(1-TMath::Floor(TMath::Abs(pt)/2.)))");
    
    cuts->AddTrackCuts(esdTrackCuts);
    cuts->SetTriggerClass("");
    cuts->SetTriggerMask(AliVEvent::kINT7);
    cuts->SetMinCentrality(minc);
    cuts->SetMaxCentrality(maxc);
    cuts->SetUseCentrality(AliRDHFCuts::kCentV0M); //kCentOff,kCentV0M,kCentTRK,kCentTKL,kCentCL1,kCentInvalid
    
    if(whichCuts==0){
        Int_t nptbins=2; Float_t ptlimits[2]={0.,1000000.};
        
        cuts->SetStandardCutsPbPb2010();
        cuts->SetUseTrackSelectionWithFilterBits(kFALSE);
        cuts->SetUsePID(kFALSE);
        Float_t cutsArrayDplustoKpipi[14]={0.25,0.3,0.3,0.,0.,0.01,0.05,0.05,0.,0.88,0.,10000000000.,3.,0.};
        cuts->SetPtBins(nptbins,ptlimits);
        cuts->SetCuts(14,cutsArrayDplustoKpipi);
        cuts->AddTrackCuts(esdTrackCuts);
        cuts->SetMinPtCandidate(2.);
    }
    
    else if(whichCuts==1){
        
        Int_t nptbins=2; Float_t ptlimits[2]={0.,1000000.};
        
        cuts->SetStandardCutsPbPb2010();
        cuts->SetUseTrackSelectionWithFilterBits(kFALSE);
        cuts->SetUsePID(kFALSE);
        Float_t cutsArrayDplustoKpipi[14]={0.25,0.3,0.3,0.,0.,0.01,0.05,0.05,0.,0.88,0.,10000000000.,3.,0.};
        cuts->SetPtBins(nptbins,ptlimits);
        cuts->SetCuts(14,cutsArrayDplustoKpipi);
        cuts->AddTrackCuts(esdTrackCuts);
        cuts->SetMinPtCandidate(2.);
        
        //activate pileup rejection (for pp)
        cuts->SetOptPileup(AliRDHFCuts::kNoPileupSelection);
        
        //Do not recalculate the vertex
        cuts->SetRemoveDaughtersFromPrim(kFALSE); //activate for pp
        
        cuts->SetMaxVtxZ(10.);
        
        cuts->SetCutOnzVertexSPD(2);
        
    }
    
    cout<<"This is the odject I'm going to save:"<<endl;
    cuts->SetName(nameCuts.Data());
    cuts->SetTitle(nameCuts.Data());
    cuts->PrintAll();
    
    return cuts;
    
}


