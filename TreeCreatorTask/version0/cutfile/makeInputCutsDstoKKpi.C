#include <Riostream.h>
#include <TFile.h>
#include <AliRDHFCutsDstoKKpi.h>
#include <TClonesArray.h>
#include <TParameter.h>

/*
 whichCuts=0, nameCuts="DstoKKpiFilteringCuts"
 whichCuts=1, nameCuts="DstoKKpiAnalysisCuts"
 */


AliRDHFCutsDstoKKpi *makeInputCutsDstoKKpi(Int_t whichCuts=0, TString nameCuts="DstoKKpiFilteringCuts", Float_t minc=0.,Float_t maxc=20.)
{
    
    AliRDHFCutsDstoKKpi* cuts=new AliRDHFCutsDstoKKpi();
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
        Float_t cutsArrayDstoKKpi[20]={0.3,0.3,0.3,0.,0.,0.,0.06,0.02,0.,0.9,0.,100000.,0.03,0.0001,-1.,1.,0.,0.,0.,-1.};
        cuts->SetPtBins(nptbins,ptlimits);
        cuts->SetCuts(20,cutsArrayDstoKKpi);
        cuts->AddTrackCuts(esdTrackCuts);
        cuts->SetMinPtCandidate(2.);
    }
    
    else if(whichCuts==1){
      
        Int_t nptbins=2; Float_t ptlimits[2]={0.,1000000.};
        
        cuts->SetStandardCutsPbPb2010();
        cuts->SetUseTrackSelectionWithFilterBits(kFALSE);
        cuts->SetUsePID(kFALSE);
        Float_t cutsArrayDstoKKpi[20]={0.3,0.3,0.3,0.,0.,0.,0.06,0.02,0.,0.9,0.,100000.,0.03,0.0001,-1.,1.,0.,0.,0.,-1.};
        cuts->SetPtBins(nptbins,ptlimits);
        cuts->SetCuts(20,cutsArrayDstoKKpi);
        cuts->AddTrackCuts(esdTrackCuts);
        cuts->SetMinPtCandidate(2.);
        
        //activate pileup rejection (for pp)
        cuts->SetOptPileup(AliRDHFCuts::kNoPileupSelection);
        
        //Do not recalculate the vertex
        cuts->SetRemoveDaughtersFromPrim(kFALSE); //activate for pp
        
        cuts->SetMaxVtxZ(10.);
        
        cuts->SetCutOnzVertexSPD(2);

    }
    
    cuts->SetName(nameCuts.Data());
    cuts->SetTitle(nameCuts.Data());
    cout<<"This is the odject I'm going to save:"<<endl;
    cuts->PrintAll();
    
    return cuts;
    
}


