#ifndef AliAnalysisTaskSEHFTreeCreator_H
#define AliAnalysisTaskSEHFTreeCreator_H

/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///*************************************************************************
/// \class Class AliAnalysisTaskSEHFTreeCreator
/// \brief AliAnalysisTaskSE for
/// \author Authors:
///
///*************************************************************************

#include <TROOT.h>
#include <TSystem.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>

#include "AliAnalysisTaskSE.h"
#include "AliRDHFCutsD0toKpi.h"
#include "AliRDHFCutsDstoKKpi.h"
#include "AliRDHFCutsDplustoKpipi.h"
#include "AliNormalizationCounter.h"
#include "AliHFCutOptTreeHandler.h"


class AliAODEvent;

class AliAnalysisTaskSEHFTreeCreator : public AliAnalysisTaskSE
{
public:
    
    AliAnalysisTaskSEHFTreeCreator();
    AliAnalysisTaskSEHFTreeCreator(const char *name,TList *cutsList);
    virtual ~AliAnalysisTaskSEHFTreeCreator();
    
    
    /// Implementation of interface methods
    virtual void UserCreateOutputObjects();
    virtual void Init();
    virtual void LocalInit() {Init();}
    virtual void UserExec(Option_t *option);
    virtual void Terminate(Option_t *option);
    
    
    void SetReadMC(Bool_t opt=kFALSE){fReadMC=opt;}
    void SetSystem(Int_t opt){fSys=opt;}
    void SetAODMismatchProtection(Int_t opt=1) {fAODProtection=opt;}
    void SetWriteOnlySignalTree(Bool_t opt){fWriteOnlySignal=opt;}
    void SetFillD0Tree(Int_t opt){fWriteVariableTreeD0=opt;}
    void SetFillDsTree(Int_t opt){fWriteVariableTreeDs=opt;}
    void SetFillDplusTree(Int_t opt){fWriteVariableTreeDplus=opt;}
    void SetPIDoptD0Tree(Int_t opt){fPIDoptD0=opt;}
    void SetPIDoptDsTree(Int_t opt){fPIDoptDs=opt;}
    void SetPIDoptDplusTree(Int_t opt){fPIDoptDplus=opt;}
    
    
    Int_t  GetSystem() const {return fSys;}
    Bool_t GetWriteOnlySignalTree() const {return fWriteOnlySignal;}
    
private:
    
    AliAnalysisTaskSEHFTreeCreator(const AliAnalysisTaskSEHFTreeCreator &source);
    AliAnalysisTaskSEHFTreeCreator& operator=(const AliAnalysisTaskSEHFTreeCreator& source);
    
    void Process2Prong(TClonesArray *array2prong, AliAODEvent *aod, TClonesArray *arrMC, Float_t centrality);
    void Process3Prong(TClonesArray *array3Prong, AliAODEvent *aod, TClonesArray *arrMC, Float_t centrality);
    
    TH1F                    *fNentries;                  //!<! histogram with number of events on output slot 3
    TList                   *fListCuts;                  //    Cuts - sent to output slot 2
    AliRDHFCutsD0toKpi      *fFiltCutsD0toKpi;           //    Cuts - sent to output slot 2
    AliRDHFCutsDstoKKpi     *fFiltCutsDstoKKpi;          //    Cuts - sent to output slot 2
    AliRDHFCutsDplustoKpipi *fFiltCutsDplustoKpipi;      //    Cuts - sent to output slot 2
    AliRDHFCutsD0toKpi      *fCutsD0toKpi;               //    Cuts - sent to output slot 2
    AliRDHFCutsDstoKKpi     *fCutsDstoKKpi;              //    Cuts - sent to output slot 2
    AliRDHFCutsDplustoKpipi *fCutsDplustoKpipi;          //    Cuts - sent to output slot 2
    Bool_t                  fReadMC;                     ///  flag for MC array: kTRUE = read it, kFALSE = do not read it
    TList                   *fListCounter;               //
    AliNormalizationCounter *fCounterD0;                 //!<! AliNormalizationCounter on output slot 5
    AliNormalizationCounter *fCounterDs;                 //!<! AliNormalizationCounter on output slot 5
    AliNormalizationCounter *fCounterDplus;              //!<! AliNormalizationCounter on output slot 5
    Bool_t                  fUseSelectionBit;
    Int_t                   fSys;                        /// fSys=0 -> p-p; fSys=1 ->PbPb
    Int_t                   fAODProtection;              /// flag to activate protection against AOD-dAOD mismatch.
    /// -1: no protection,  0: check AOD/dAOD nEvents only,  1: check AOD/dAOD nEvents + TProcessID names
    Int_t                    fWriteVariableTreeD0;       /// flag to decide whether to write the candidate variables on a tree variables
    //0 don't fill
    //1 fill standard tree
    Int_t                    fWriteVariableTreeDs;       /// flag to decide whether to write the candidate variables on a tree variables
    //0 don't fill
    //1 fill standard tree
    Int_t                    fWriteVariableTreeDplus;    /// flag to decide whether to write the candidate variables on a tree variables
    //0 don't fill
    //1 fill standard tree
    TTree                   *fVariablesTreeD0;           //!<! tree of the candidate variables
    TTree                   *fVariablesTreeDs;           //!<! tree of the candidate variables
    TTree                   *fVariablesTreeDplus;        //!<! tree of the candidate variables
    Bool_t                  fWriteOnlySignal;
    AliHFCutOptTreeHandler  *fTreeHandlerD0;             /// helper object for the tree with topological variables
    AliHFCutOptTreeHandler  *fTreeHandlerDs;             /// helper object for the tree with topological variables
    AliHFCutOptTreeHandler  *fTreeHandlerDplus;          /// helper object for the tree with topological variables
    Int_t                   fPIDoptD0;
    Int_t                   fPIDoptDs;
    Int_t                   fPIDoptDplus;
    
    /// \cond CLASSIMP
    ClassDef(AliAnalysisTaskSEHFTreeCreator,0);
    /// \endcond
};

#endif

