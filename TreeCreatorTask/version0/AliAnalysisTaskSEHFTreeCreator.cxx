/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/////////////////////////////////////////////////////////////
//
//
//
// Authors:
////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TList.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TDatabasePDG.h>
#include <THnSparse.h>
#include "AliVertexingHFUtils.h"
#include <AliAnalysisDataSlot.h>
#include <AliAnalysisDataContainer.h>
#include "AliAnalysisManager.h"
#include "AliESDtrack.h"
#include "AliVertexerTracks.h"
#include "AliAODHandler.h"
#include "AliAODEvent.h"
#include "AliAODVertex.h"
#include "AliAODTrack.h"
#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"
#include "AliAODRecoDecay.h"
#include "AliAODRecoDecayHF2Prong.h"
#include "AliAODRecoDecayHF3Prong.h"
#include "AliAODRecoCascadeHF.h"
#include "AliAnalysisVertexingHF.h"
#include "AliNormalizationCounter.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisTaskSEHFTreeCreator.h"

using std::cout;
using std::endl;

/// \cond CLASSIMP
ClassImp(AliAnalysisTaskSEHFTreeCreator);
/// \endcond

//________________________________________________________________________
AliAnalysisTaskSEHFTreeCreator::AliAnalysisTaskSEHFTreeCreator():
AliAnalysisTaskSE(),
fNentries(0),
fListCuts(0),
fFiltCutsD0toKpi(0),
fFiltCutsDstoKKpi(0),
fFiltCutsDplustoKpipi(0),
fCutsD0toKpi(0),
fCutsDstoKKpi(0),
fCutsDplustoKpipi(0),
fReadMC(0),
fListCounter(0),
fCounterD0(0),
fCounterDs(0),
fCounterDplus(0),
fUseSelectionBit(kTRUE),
fSys(0),
fAODProtection(1),
fWriteVariableTreeD0(0),
fWriteVariableTreeDs(0),
fWriteVariableTreeDplus(0),
fVariablesTreeD0(0),
fVariablesTreeDs(0),
fVariablesTreeDplus(0),
fWriteOnlySignal(kFALSE),
fTreeHandlerD0(0x0),
fTreeHandlerDs(0x0),
fTreeHandlerDplus(0x0),
fPIDoptD0(AliHFCutOptTreeHandler::kNsigmaPIDchar),
fPIDoptDs(AliHFCutOptTreeHandler::kNsigmaPIDchar),
fPIDoptDplus(AliHFCutOptTreeHandler::kNsigmaPIDchar)
{

}

//________________________________________________________________________
AliAnalysisTaskSEHFTreeCreator::AliAnalysisTaskSEHFTreeCreator(const char *name, TList *cutsList):
AliAnalysisTaskSE(name),
fNentries(0),
fListCuts(0),
fFiltCutsD0toKpi(0),
fFiltCutsDstoKKpi(0),
fFiltCutsDplustoKpipi(0),
fCutsD0toKpi(0),
fCutsDstoKKpi(0),
fCutsDplustoKpipi(0),
fReadMC(0),
fListCounter(0),
fCounterD0(0),
fCounterDs(0),
fCounterDplus(0),
fUseSelectionBit(kTRUE),
fSys(0),
fAODProtection(1),
fWriteVariableTreeD0(0),
fWriteVariableTreeDs(0),
fWriteVariableTreeDplus(0),
fVariablesTreeD0(0),
fVariablesTreeDs(0),
fVariablesTreeDplus(0),
fWriteOnlySignal(kFALSE),
fTreeHandlerD0(0x0),
fTreeHandlerDs(0x0),
fTreeHandlerDplus(0x0),
fPIDoptD0(AliHFCutOptTreeHandler::kNsigmaPIDchar),
fPIDoptDs(AliHFCutOptTreeHandler::kNsigmaPIDchar),
fPIDoptDplus(AliHFCutOptTreeHandler::kNsigmaPIDchar)
{
    /// Default constructor
    //fListCuts=cutsList;
    fFiltCutsD0toKpi     =(AliRDHFCutsD0toKpi*)cutsList->FindObject("D0toKpiFilteringCuts");
    fFiltCutsDstoKKpi    =(AliRDHFCutsDstoKKpi*)cutsList->FindObject("DstoKKpiFilteringCuts");
    fFiltCutsDplustoKpipi=(AliRDHFCutsDplustoKpipi*)cutsList->FindObject("DplustoKpipiFilteringCuts");
    fCutsD0toKpi         =(AliRDHFCutsD0toKpi*)cutsList->FindObject("D0toKpiAnalysisCuts");
    fCutsDstoKKpi        =(AliRDHFCutsDstoKKpi*)cutsList->FindObject("DstoKKpiAnalysisCuts");
    fCutsDplustoKpipi    =(AliRDHFCutsDplustoKpipi*)cutsList->FindObject("DplustoKpipiAnalysisCuts");
    
    // Output slot #1 writes into a TH1F container (number of events)
    DefineOutput(1,TH1F::Class());
    // Output slot #2 writes into a TList container (cuts)
    DefineOutput(2,TList::Class());
    // Output slot #3 writes Normalization Counter
    DefineOutput(3,TList::Class());
    // Output slot #4 stores the tree of the candidate variables after track selection
    if(fWriteVariableTreeD0) DefineOutput(4,TTree::Class());
    if(fWriteVariableTreeDs) DefineOutput(5,TTree::Class());
    if(fWriteVariableTreeDplus)  DefineOutput(6,TTree::Class());
}

//________________________________________________________________________
AliAnalysisTaskSEHFTreeCreator::~AliAnalysisTaskSEHFTreeCreator()
{
    if (fListCuts) {
        delete fListCuts;
        fListCuts = 0;
    }
    if (fFiltCutsD0toKpi) {
        delete fFiltCutsD0toKpi;
        fFiltCutsD0toKpi = 0;
    }
    if (fFiltCutsDstoKKpi) {
        delete fFiltCutsDstoKKpi;
        fFiltCutsDstoKKpi = 0;
    }
    if (fFiltCutsDplustoKpipi) {
        delete fFiltCutsDplustoKpipi;
        fFiltCutsDplustoKpipi = 0;
    }
    if (fCutsD0toKpi) {
        delete fCutsD0toKpi;
        fCutsD0toKpi = 0;
    }
    if (fCutsDstoKKpi) {
        delete fCutsDstoKKpi;
        fCutsDstoKKpi = 0;
    }
    if (fCutsDplustoKpipi) {
        delete fCutsDplustoKpipi;
        fCutsDplustoKpipi = 0;
    }
    if (fNentries){
        delete fNentries;
        fNentries = 0;
    }
    if (fListCounter) {
        delete fListCounter;
        fListCounter = 0;
    }
    if(fCounterD0){
        delete fCounterD0;
        fCounterD0=0;
    }
    if(fCounterDs){
        delete fCounterDs;
        fCounterDs=0;
    }
    if(fCounterDplus){
        delete fCounterDplus;
        fCounterDplus=0;
    }
    if(fTreeHandlerD0) {
        delete fTreeHandlerD0;
        fTreeHandlerD0 = 0;
    }
    if(fTreeHandlerDs) {
        delete fTreeHandlerDs;
        fTreeHandlerDs = 0;
    }
    if(fTreeHandlerDplus) {
        delete fTreeHandlerDplus;
        fTreeHandlerDplus = 0;
    }
    
}

//________________________________________________________________________
void AliAnalysisTaskSEHFTreeCreator::Init()
{
    /// Initialization
    
    if(fDebug > 1) printf("AliAnalysisTaskSEHFTreeCreator::Init() \n");
    
    fListCuts=new TList();
    fListCuts->SetOwner();
    fListCuts->SetName("CutObjects");
    
    AliRDHFCutsD0toKpi* copyFiltfCutsD0=new AliRDHFCutsD0toKpi(*fFiltCutsD0toKpi);
    copyFiltfCutsD0->SetName("D0toKpi_filtering_cuts");
    AliRDHFCutsDstoKKpi* copyFiltfCutsDs=new AliRDHFCutsDstoKKpi(*fFiltCutsDstoKKpi);
    copyFiltfCutsDs->SetName("DstoKKpi_filtering_cuts");
    AliRDHFCutsDplustoKpipi* copyFiltfCutsDplus=new AliRDHFCutsDplustoKpipi(*fFiltCutsDplustoKpipi);
    copyFiltfCutsDplus->SetName("DplustoKpipi_filtering_cuts");
    AliRDHFCutsD0toKpi* copyfCutsD0=new AliRDHFCutsD0toKpi(*fCutsD0toKpi);
    copyfCutsD0->SetName("D0toKpi_analysis_cuts");
    AliRDHFCutsDstoKKpi* copyfCutsDs=new AliRDHFCutsDstoKKpi(*fCutsDstoKKpi);
    copyfCutsDs->SetName("DstoKKpi_analysis_cuts");
    AliRDHFCutsDplustoKpipi* copyfCutsDplus=new AliRDHFCutsDplustoKpipi(*fCutsDplustoKpipi);
    copyfCutsDplus->SetName("DplustoKpipi_analysis_cuts");
    // Post the data
    fListCuts->Add(copyFiltfCutsD0);
    fListCuts->Add(copyFiltfCutsDs);
    fListCuts->Add(copyFiltfCutsDplus);
    fListCuts->Add(copyfCutsD0);
    fListCuts->Add(copyfCutsDs);
    fListCuts->Add(copyfCutsDplus);
    PostData(2,fListCuts);
    
    
    return;
}

//________________________________________________________________________
void AliAnalysisTaskSEHFTreeCreator::UserCreateOutputObjects()
{
    
    /// Create the output container
    //
    if(fDebug > 1) printf("AliAnalysisTaskSEHFTreeCreator::UserCreateOutputObjects() \n");
    
    const char* nameoutput=GetOutputSlot(1)->GetContainer()->GetName();
    fNentries=new TH1F(nameoutput, "Number of events", 23,-0.5,22.5);
    fNentries->GetXaxis()->SetBinLabel(1,"n. evt. read");
    fNentries->GetXaxis()->SetBinLabel(2,"n. evt. matched dAOD");
    fNentries->GetXaxis()->SetBinLabel(3,"n. evt. mismatched dAOD");
    fNentries->GetXaxis()->SetBinLabel(4,"n. evt. analised");
    fNentries->GetXaxis()->SetBinLabel(5,"n. evt. passing IsEvSelected");
    fNentries->GetXaxis()->SetBinLabel(6,"n. evt. rejected due to trigger");
    fNentries->GetXaxis()->SetBinLabel(7,"n. evt. rejected due to not reco vertex");
    fNentries->GetXaxis()->SetBinLabel(8,"n. evt. rejected for contr vertex");
    fNentries->GetXaxis()->SetBinLabel(9,"n. evt. rejected for vertex out of accept");
    fNentries->GetXaxis()->SetBinLabel(10,"n. evt. rejected for pileup events");
    fNentries->GetXaxis()->SetBinLabel(11,"n. evt. of out centrality events");
    fNentries->GetXaxis()->SetBinLabel(12,"n. of 2 prong candidates");
    fNentries->GetXaxis()->SetBinLabel(13,"n. D0 after filtering");
    fNentries->GetXaxis()->SetBinLabel(14,"n. D0 after selection");
    fNentries->GetXaxis()->SetBinLabel(15,"n. of not on-the-fly rec D0");
    fNentries->GetXaxis()->SetBinLabel(16,"n. of 3 prong candidates");
    fNentries->GetXaxis()->SetBinLabel(17,"n. Ds after filtering");
    fNentries->GetXaxis()->SetBinLabel(18,"n. Ds after selection");
    fNentries->GetXaxis()->SetBinLabel(19,"n. of not on-the-fly rec Ds");
    fNentries->GetXaxis()->SetBinLabel(20,"n. Dplus after filtering");
    fNentries->GetXaxis()->SetBinLabel(21,"n. Dplus after selection");
    fNentries->GetXaxis()->SetBinLabel(22,"n. of not on-the-fly rec Dplus");
    
    
    fListCounter=new TList();
    fListCounter->SetOwner();
    fListCounter->SetName("NormCounter");
    fCounterD0 = new AliNormalizationCounter("norm_counter_D0");
    fCounterD0->Init();
    fCounterDs = new AliNormalizationCounter("norm_counter_Ds");
    fCounterDs->Init();
    fCounterDplus = new AliNormalizationCounter("norm_counter_Dplus");
    fCounterDplus->Init();
    fListCounter->Add(fCounterD0);
    fListCounter->Add(fCounterDs);
    fListCounter->Add(fCounterDplus);
    
    //
    // Output slot 4 : tree of the candidate variables after track selection
    //
    
    if(fWriteVariableTreeD0){
        nameoutput = GetOutputSlot(4)->GetContainer()->GetName();
        fTreeHandlerD0 = new AliHFCutOptTreeHandler(AliHFCutOptTreeHandler::kD0toKpi,fPIDoptD0,fReadMC);
        if(fSys>0) fTreeHandlerD0->SetUseCentrality();
        if(fReadMC && fWriteOnlySignal) fTreeHandlerD0->SetFillOnlySignal();
        fTreeHandlerD0->SetIsMC(fReadMC);
        fVariablesTreeD0 = (TTree*)fTreeHandlerD0->BuildTree(nameoutput,nameoutput);
    }
    if(fWriteVariableTreeDs){
        nameoutput = GetOutputSlot(5)->GetContainer()->GetName();
        fTreeHandlerDs = new AliHFCutOptTreeHandler(AliHFCutOptTreeHandler::kDstoKKpi,fPIDoptDs,fReadMC);
        if(fSys>0) fTreeHandlerDs->SetUseCentrality();
        if(fReadMC && fWriteOnlySignal) fTreeHandlerDs->SetFillOnlySignal();
        fTreeHandlerDs->SetIsMC(fReadMC);
        fVariablesTreeDs = (TTree*)fTreeHandlerDs->BuildTree(nameoutput,nameoutput);
    }
    if(fWriteVariableTreeDplus){
        nameoutput = GetOutputSlot(6)->GetContainer()->GetName();
        fTreeHandlerDplus = new AliHFCutOptTreeHandler(AliHFCutOptTreeHandler::kDplustoKpipi,fPIDoptDplus,fReadMC);
        if(fSys>0) fTreeHandlerDplus->SetUseCentrality();
        if(fReadMC && fWriteOnlySignal) fTreeHandlerDplus->SetFillOnlySignal();
        fTreeHandlerDplus->SetIsMC(fReadMC);
        fVariablesTreeDplus = (TTree*)fTreeHandlerDplus->BuildTree(nameoutput,nameoutput);
    }
    
    // Post the data
    PostData(1,fNentries);
    PostData(3,fListCounter);
    if(fWriteVariableTreeD0)  PostData(4,fVariablesTreeD0);
    if(fWriteVariableTreeDs)  PostData(5,fVariablesTreeDs);
    if(fWriteVariableTreeDplus)   PostData(6,fVariablesTreeDplus);
   
    return;
}

//________________________________________________________________________
void AliAnalysisTaskSEHFTreeCreator::UserExec(Option_t */*option*/)

{
    /// Execute analysis for current event:
    
    AliAODEvent *aod = dynamic_cast<AliAODEvent*> (InputEvent());
    
    fNentries->Fill(0); // all events
    if(fAODProtection>=0){
        //   Protection against different number of events in the AOD and deltaAOD
        //   In case of discrepancy the event is rejected.
        Int_t matchingAODdeltaAODlevel = AliRDHFCuts::CheckMatchingAODdeltaAODevents();
        if (matchingAODdeltaAODlevel<0 || (matchingAODdeltaAODlevel==0 && fAODProtection==1)) {
            // AOD/deltaAOD trees have different number of entries || TProcessID do not match while it was required
            fNentries->Fill(2);
            return;
        }
        fNentries->Fill(1);
    }
    

    TString candidates2prongArrayName="D0toKpi";
    TString candidates3prongArrayName="Charm3Prong";
    TClonesArray *array2prong=0;
    TClonesArray *array3Prong=0;
    
    if(!aod && AODEvent() && IsStandardAOD()) {
        // In case there is an AOD handler writing a standard AOD, use the AOD
        // event in memory rather than the input (ESD) event.
        aod = dynamic_cast<AliAODEvent*> (AODEvent());
        // in this case the braches in the deltaAOD (AliAOD.VertexingHF.root)
        // have to taken from the AOD event hold by the AliAODExtension
        AliAODHandler* aodHandler = (AliAODHandler*)
        ((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler());
        
        if(aodHandler->GetExtensions()) {
            AliAODExtension *ext = (AliAODExtension*)aodHandler->GetExtensions()->FindObject("AliAOD.VertexingHF.root");
            AliAODEvent* aodFromExt = ext->GetAOD();
            array2prong=(TClonesArray*)aodFromExt->GetList()->FindObject(candidates2prongArrayName.Data());
            array3Prong=(TClonesArray*)aodFromExt->GetList()->FindObject(candidates3prongArrayName.Data());
        }
    } else if(aod) {
        array2prong=(TClonesArray*)aod->GetList()->FindObject(candidates2prongArrayName.Data());
        array3Prong=(TClonesArray*)aod->GetList()->FindObject(candidates3prongArrayName.Data());
    }
    
    if(!array2prong || !array3Prong || !aod) {
        printf("AliAnalysisTaskSEHFTreeCreator::UserExec: input branches not found!\n");
        return;
    }
    // fix for temporary bug in ESDfilter
    // the AODs with null vertex pointer didn't pass the PhysSel
    if(!aod->GetPrimaryVertex() || TMath::Abs(aod->GetMagneticField())<0.001) return;
    
    fNentries->Fill(3); // count events
    
    fCounterD0->StoreEvent(aod,fCutsD0toKpi,fReadMC);
    fCounterDs->StoreEvent(aod,fCutsDstoKKpi,fReadMC);
    fCounterDplus->StoreEvent(aod,fCutsDplustoKpipi,fReadMC);
    
    if(fCutsD0toKpi->IsEventRejectedDueToTrigger())fNentries->Fill(5);
    if(fCutsD0toKpi->IsEventRejectedDueToNotRecoVertex())fNentries->Fill(6);
    if(fCutsD0toKpi->IsEventRejectedDueToVertexContributors())fNentries->Fill(7);
    if(fCutsD0toKpi->IsEventRejectedDueToZVertexOutsideFiducialRegion())fNentries->Fill(8);
    if(fCutsD0toKpi->IsEventRejectedDueToPileup())fNentries->Fill(9);
    if(fCutsD0toKpi->IsEventRejectedDueToCentrality())fNentries->Fill(10);
    
    TClonesArray *mcArray = 0;
    AliAODMCHeader *mcHeader = 0;
    
    if(fReadMC) {
        // load MC particles
        mcArray = (TClonesArray*)aod->GetList()->FindObject(AliAODMCParticle::StdBranchName());
        if(!mcArray) {
            printf("AliAnalysisTaskSEHFTreeCreator::UserExec: MC particles branch not found!\n");
            return;
        }
        
        // load MC header
        mcHeader = (AliAODMCHeader*)aod->GetList()->FindObject(AliAODMCHeader::StdBranchName());
        if(!mcHeader) {
            printf("AliAnalysisTaskSEHFTreeCreator::UserExec: MC header branch not found!\n");
            return;
        }
    }
    
    Bool_t isEvSel=fCutsD0toKpi->IsEventSelected(aod);
    if(!isEvSel)return;
    fNentries->Fill(4);
    
    Float_t ntracks=aod->GetNumberOfTracks();
    Float_t evCentr=fCutsD0toKpi->GetCentrality(aod);
    Int_t runNumber=aod->GetRunNumber();
    
    Process2Prong(array2prong,aod,mcArray,evCentr);
    Process3Prong(array3Prong,aod,mcArray,evCentr);
    
    
    // Post the data
    // Post the data
    PostData(1,fNentries);
    PostData(3,fListCounter);
    if(fWriteVariableTreeD0)  PostData(4,fVariablesTreeD0);
    if(fWriteVariableTreeDs)  PostData(5,fVariablesTreeDs);
    if(fWriteVariableTreeDplus)   PostData(6,fVariablesTreeDplus);
    
    return;
}
//________________________________________________________________________
void AliAnalysisTaskSEHFTreeCreator::Terminate(Option_t */*option*/)
{
    /// Terminate analysis
    //
    if(fDebug > 1) printf("AliAnalysisTaskSEHFTreeCreator: Terminate() \n");
    
    
    fNentries = dynamic_cast<TH1F*>(GetOutputData(1));
    if(!fNentries){
        printf("ERROR: fNEntries not available\n");
        return;
    }
    
    fListCuts = dynamic_cast<TList*>(GetOutputData(2));
    if(!fListCuts){
        printf("ERROR: fListCuts not available\n");
        return;
    }
    
    fListCounter = dynamic_cast<TList*>(GetOutputData(3));
    if(!fListCounter){
        printf("ERROR: fListCounter not available\n");
        return;
    }
    
    return;
}
//--------------------------------------------------------
void AliAnalysisTaskSEHFTreeCreator::Process2Prong(TClonesArray *array2prong, AliAODEvent *aod, TClonesArray *arrMC, Float_t centrality){
    
    Int_t n2prong = array2prong->GetEntriesFast();
    if(fDebug>2) printf("Number of D0->Kpi: %d\n",n2prong);
    
    Int_t nSelectedD0=0;
    Int_t nFilteredD0=0;
    
    // vHF object is needed to call the method that refills the missing info of the candidates
    // if they have been deleted in dAOD reconstruction phase
    // in order to reduce the size of the file
    AliAnalysisVertexingHF *vHF = new AliAnalysisVertexingHF();
    
    for (Int_t i2prong = 0; i2prong < n2prong; i2prong++) {
        AliAODRecoDecayHF2Prong *d = (AliAODRecoDecayHF2Prong*)array2prong->UncheckedAt(i2prong);
        fNentries->Fill(11);
        
        if(fUseSelectionBit && d->GetSelectionMap()) if(!d->HasSelectionBit(AliRDHFCuts::kD0toKpiCuts)){
            continue;
        }
        fNentries->Fill(12);
        nFilteredD0++;
        if(!(vHF->FillRecoCand(aod,d))) {//Fill the data members of the candidate only if they are empty.
            fNentries->Fill(14); //monitor how often this fails
            continue;
        }
        
        //fiducial acceptance
        Bool_t isFidAcc=fCutsD0toKpi->IsInFiducialAcceptance(d->Pt(),d->Y(421));
        
        if(isFidAcc){
            Int_t isSelectedCandidate=fCutsD0toKpi->IsSelected(d,AliRDHFCuts::kAll,aod); //selected
            if(isSelectedCandidate){
                fNentries->Fill(13);
                nSelectedD0++;
            }
            if(!isSelectedCandidate) return;
            
            Int_t pdgDgD0toKpi[2]={321,211};
            Int_t labD0=-1;
            if(fReadMC) labD0 = d->MatchToMC(421,arrMC,2,pdgDgD0toKpi); //return MC particle label if the array corresponds to a D0, -1 if not (cf. AliAODRecoDecay.cxx)
            
            Bool_t issignal=kFALSE;
            Bool_t isprompt=kFALSE;
            Bool_t isrefl=kFALSE;
            Int_t masshypo=0;
            
            AliAODPidHF* pidHF = fCutsD0toKpi->GetPidHF();
            
            if (isSelectedCandidate==1 || isSelectedCandidate==3) { //D0
                if(fWriteVariableTreeD0 && fSys>0) fTreeHandlerD0->SetCentrality((Char_t)centrality);
                masshypo=0;
                if(fReadMC){
                    if(labD0>=0){
                        AliAODMCParticle *partD0 = (AliAODMCParticle*)arrMC->At(labD0);
                        Int_t pdgD0 = partD0->GetPdgCode();
                        Int_t origin=AliVertexingHFUtils::CheckOrigin(arrMC,partD0,kTRUE);
                        if(origin==4) isprompt=kTRUE;
                        else if(origin==5) isprompt=kFALSE;
                        
                        if(pdgD0==421){
                            issignal=kTRUE;
                            isrefl=kFALSE;
                        }
                        else {
                            issignal=kTRUE;
                            isrefl=kTRUE;
                        }
                    }//end labD0check
                    else{//background
                        issignal=kFALSE;
                        isrefl=kFALSE;
                    }
                    if(fWriteVariableTreeD0){
                        fTreeHandlerD0->SetCandidateType(issignal,isprompt,isrefl);
                        fTreeHandlerD0->SetVariables(d,masshypo,pidHF,0x0);
                        fTreeHandlerD0->FillTree();
                    }
                }//end read MC
                else{
                    issignal=kFALSE;
                    isrefl=kFALSE;
                    if(fWriteVariableTreeD0){
                        fTreeHandlerD0->SetVariables(d,masshypo,pidHF,0x0);
                        fTreeHandlerD0->FillTree();
                    }
                }
                
            }//end D0
            if (isSelectedCandidate>1){//D0bar
                if(fWriteVariableTreeD0 && fSys>0) fTreeHandlerD0->SetCentrality((Char_t)centrality);
                masshypo=1;
                if(fReadMC){
                    if(labD0>=0){
                        AliAODMCParticle *partD0 = (AliAODMCParticle*)arrMC->At(labD0);
                        Int_t pdgD0 = partD0->GetPdgCode();
                        Int_t origin=AliVertexingHFUtils::CheckOrigin(arrMC,partD0,kTRUE);
                        if(origin==4) isprompt=kTRUE;
                        else if(origin==5) isprompt=kFALSE;
                        
                        if(pdgD0==-421){
                            issignal=kTRUE;
                            isrefl=kFALSE;
                        }
                        else {
                            issignal=kTRUE;
                            isrefl=kTRUE;
                        }
                    } //end label check
                    else{ //background MC
                        issignal=kFALSE;
                        isrefl=kFALSE;
                    }
                    if(fWriteVariableTreeD0){
                        fTreeHandlerD0->SetCandidateType(issignal,isprompt,isrefl);
                        fTreeHandlerD0->SetVariables(d,masshypo,pidHF,0x0);
                        fTreeHandlerD0->FillTree();
                    }
                }//end readMC
                else{
                    issignal=kFALSE;
                    isrefl=kFALSE;
                    if(fWriteVariableTreeD0){
                        fTreeHandlerD0->SetVariables(d,masshypo,pidHF,0x0);
                        fTreeHandlerD0->FillTree();
                    }
                }
            }//end D0bar
        }//end is fid acc
        
    }//end loop on candidates
    
    fCounterD0->StoreCandidates(aod,nFilteredD0,kTRUE);
    fCounterD0->StoreCandidates(aod,nSelectedD0,kFALSE);
    delete vHF;
    return;
}

//--------------------------------------------------------
void AliAnalysisTaskSEHFTreeCreator::Process3Prong(TClonesArray *array3Prong, AliAODEvent *aod, TClonesArray *arrMC, Float_t centrality){
    
    AliAODVertex *vtx1 = (AliAODVertex*)aod->GetPrimaryVertex();
    
    Int_t n3prong = array3Prong->GetEntriesFast();
    if(fDebug>1) printf("Number of Ds->KKpi: %d\n",n3prong);
    
    Int_t pdgDstoKKpi[3]={321,321,211};
    Int_t nSelectedDs=0;
    Int_t nFilteredDs=0;
    Double_t massPhi=TDatabasePDG::Instance()->GetParticle(333)->Mass();
    
    // vHF object is needed to call the method that refills the missing info of the candidates
    // if they have been deleted in dAOD reconstruction phase
    // in order to reduce the size of the file
    AliAnalysisVertexingHF *vHF=new AliAnalysisVertexingHF();
    
    for (Int_t i3prong = 0; i3prong < n3prong; i3prong++) {
        fNentries->Fill(15);
        
        //Ds
        Bool_t isDstagged=kTRUE;
        AliAODRecoDecayHF3Prong *ds    = (AliAODRecoDecayHF3Prong*)array3Prong->UncheckedAt(i3prong);
        if(fUseSelectionBit && !(ds->HasSelectionBit(AliRDHFCuts::kDsCuts))){
            isDstagged=kFALSE;
        }
        
        if(isDstagged){
            fNentries->Fill(16);
            nFilteredDs++;
            if((vHF->FillRecoCand(aod,ds))) {////Fill the data members of the candidate only if they are empty.
                
                Bool_t unsetvtx=kFALSE;
                if(!ds->GetOwnPrimaryVtx()){
                    ds->SetOwnPrimaryVtx(vtx1);
                    unsetvtx=kTRUE;
                    // NOTE: the ow primary vertex should be unset, otherwise there is a memory leak
                    // Pay attention if you use continue inside this loop!!!
                }
                
                Bool_t recVtx=kFALSE;
                AliAODVertex *origownvtx=0x0;
                Bool_t isFidAcc=fCutsDstoKKpi->IsInFiducialAcceptance(ds->Pt(),ds->YDs());
                if(isFidAcc){
                    Int_t isSelectedCandidateAnalysisCuts=fCutsDstoKKpi->IsSelected(ds,AliRDHFCuts::kAll,aod);
                    Int_t isSelectedCandidateNoRes=isSelectedCandidateAnalysisCuts;
                    Bool_t origRes=fCutsDstoKKpi->IsCutOnResonancesApplied();
                    if(origRes){
                        fCutsDstoKKpi->ApplyCutOnResonances(kFALSE);
                        isSelectedCandidateNoRes=fCutsDstoKKpi->IsSelected(ds,AliRDHFCuts::kAll,aod);
                        fCutsDstoKKpi->ApplyCutOnResonances(origRes);
                    }
                    
                    Int_t isKKpi=isSelectedCandidateAnalysisCuts&1;
                    Int_t ispiKK=isSelectedCandidateAnalysisCuts&2;
                    Int_t isPhiKKpi=isSelectedCandidateAnalysisCuts&4;
                    Int_t isPhipiKK=isSelectedCandidateAnalysisCuts&8;
                    Int_t isK0starKKpi=isSelectedCandidateAnalysisCuts&16;
                    Int_t isK0starpiKK=isSelectedCandidateAnalysisCuts&32;
                    
                    if(isSelectedCandidateAnalysisCuts>0){
                        
                        fNentries->Fill(17);
                        nSelectedDs++;
                        
                        if(fCutsDstoKKpi->GetIsPrimaryWithoutDaughters()){
                            if(ds->GetOwnPrimaryVtx()) origownvtx=new AliAODVertex(*ds->GetOwnPrimaryVtx());
                            if(fCutsDstoKKpi->RecalcOwnPrimaryVtx(ds,aod))recVtx=kTRUE;
                            else fCutsDstoKKpi->CleanOwnPrimaryVtx(ds,aod,origownvtx);
                        }
                        
                        Int_t labDs=-1;
                        Int_t labDplus=-1;
                        Int_t pdgCode0=-999;
                        Int_t orig=0;
                        
                        if(fReadMC){
                            
                            AliAODMCParticle *partDs = 0x0;
                    
                            
                            labDs = ds->MatchToMC(431,arrMC,3,pdgDstoKKpi);
                            labDplus = ds->MatchToMC(411,arrMC,3,pdgDstoKKpi);
                            
                            if(labDs>=0){
                                Int_t labDau0=((AliAODTrack*)ds->GetDaughter(0))->GetLabel();
                                AliAODMCParticle* p=(AliAODMCParticle*)arrMC->UncheckedAt(TMath::Abs(labDau0));
                                pdgCode0=TMath::Abs(p->GetPdgCode());
                                
                                partDs = (AliAODMCParticle*)arrMC->At(labDs);
                                
                            }
                            else{
                                if(labDplus>=0) {
                                    Int_t labDau0=((AliAODTrack*)ds->GetDaughter(0))->GetLabel();
                                    AliAODMCParticle* p=(AliAODMCParticle*)arrMC->UncheckedAt(TMath::Abs(labDau0));
                                    pdgCode0=TMath::Abs(p->GetPdgCode());
                                    
                                    partDs = (AliAODMCParticle*)arrMC->At(labDplus);
                                }
                            }
                            if(partDs) orig = AliVertexingHFUtils::CheckOrigin(arrMC,partDs,kTRUE);
                        }
                        
                        if(fWriteVariableTreeDs>0 && fWriteVariableTreeDs<4){
                            if ((fWriteVariableTreeDs==1 && (isPhiKKpi || isPhipiKK)) || (fWriteVariableTreeDs==2 && (isK0starKKpi || isK0starpiKK)) || (fWriteVariableTreeDs==3 && (isKKpi || ispiKK))){
                                if(fSys>0) fTreeHandlerDs->SetCentrality((Char_t)centrality);
                                AliAODPidHF* pidHF = fCutsDstoKKpi->GetPidHF();
                                
                                Bool_t issignal = kFALSE;
                                Bool_t isprompt = kFALSE;
                                Bool_t isrefl = kFALSE;
                                
                                if(isKKpi || isPhiKKpi || isK0starKKpi) {
                                    if(fReadMC) {
                                        if(labDs>=0) issignal=kTRUE;
                                        else issignal=kFALSE;
                                        
                                        if(orig==4) isprompt=kTRUE;
                                        else if(orig==5) isprompt=kFALSE;
                                        
                                        if(pdgCode0==211) isrefl=kTRUE;
                                        else isrefl=kFALSE;
                                        
                                        fTreeHandlerDs->SetCandidateType(issignal,isprompt,isrefl);
                                        fTreeHandlerDs->SetVariables(ds,0,pidHF,0x0);
                                        fTreeHandlerDs->FillTree();
                                    }
                                    else { //real data
                                        fTreeHandlerDs->SetVariables(ds,0,pidHF,0x0);
                                        fTreeHandlerDs->FillTree();
                                    }
                                }
                                if(ispiKK || isPhipiKK || isK0starpiKK) {
                                    if(fReadMC) {
                                        if(labDs>=0) issignal=kTRUE;
                                        else issignal=kFALSE;
                                        
                                        if(orig==4) isprompt=kTRUE;
                                        else if(orig==5) isprompt=kFALSE;
                                        
                                        if(pdgCode0==321) isrefl=kTRUE;
                                        else isrefl=kFALSE;
                                        
                                        fTreeHandlerDs->SetCandidateType(issignal,isprompt,isrefl);
                                        fTreeHandlerDs->SetVariables(ds,1,pidHF,0x0);
                                        fTreeHandlerDs->FillTree();
                                    }
                                    else { //real data
                                        fTreeHandlerDs->SetVariables(ds,1,pidHF,0x0);
                                        fTreeHandlerDs->FillTree();
                                    }
                                }
                            }
                        }//end fill tree
                    }//end is selected
                }//fid acc
            }
            else{
                fNentries->Fill(18); //monitor how often this fails
            }
        }//end Ds
        
        
        //*************************************
        //Dplus
        Bool_t isDplustagged=kTRUE;
        AliAODRecoDecayHF3Prong *dplus = (AliAODRecoDecayHF3Prong*)array3Prong->UncheckedAt(i3prong);
        if(fUseSelectionBit && !(dplus->HasSelectionBit(AliRDHFCuts::kDplusCuts))){
            isDplustagged=kFALSE;
        }
        if(isDplustagged){
            fNentries->Fill(19);
            if((vHF->FillRecoCand(aod,dplus))) {////Fill the data members of the candidate only if they are empty.
                
                
            }
            else{
                fNentries->Fill(21); //monitor how often this fails
            }
        }//end Dplus
        
        
    }//end loop on cadidates
    
    fCounterDs->StoreCandidates(aod,nFilteredDs,kTRUE);
    fCounterDs->StoreCandidates(aod,nSelectedDs,kFALSE);
    
    delete vHF;
    return;
    
}

