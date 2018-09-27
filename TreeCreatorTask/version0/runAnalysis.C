#if !defined (__CINT__) || defined (__CLING__)
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"

#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisTaskPIDResponse.h"

#include "AliAnalysisTaskSEHFTreeCreator.h"
#endif

void runAnalysis()
{
    // set if you want to run the analysis locally (kTRUE), or on grid (kFALSE)
    Bool_t local = kTRUE;
    // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
    Bool_t gridTest = kFALSE;
    
    // since we will compile a class, tell root where to look for headers  
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
#else
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");
#endif
     
    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("AnalysisTaskExample");
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);

    //AliMCEventHandler  *mcH = new AliMCEventHandler();
    //mgr->SetMCtruthEventHandler(mcH);



    // compile the class and load the add task macro
    // here we have to differentiate between using the just-in-time compiler
    // from root6, or the interpreter of root5
#if !defined (__CINT__) || defined (__CLING__)

    AliPhysicsSelectionTask *physseltask = reinterpret_cast<AliPhysicsSelectionTask *>(gInterpreter->ProcessLine(Form(".x %s (%d)", gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C"),kTRUE)));
    
    AliAnalysisTaskPIDResponse *pidResp = reinterpret_cast<AliAnalysisTaskPIDResponse *>(gInterpreter->ProcessLine(Form(".x %s (%d)", gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"),kTRUE)));
    
    AliMultSelectionTask *multSel = reinterpret_cast<AliMultSelectionTask *>(gInterpreter->ProcessLine(Form(".x %s", gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C"))));
    multSel->SetAlternateOADBforEstimators("LHC15o-DefaultMC-HIJING");

    gInterpreter->LoadMacro("AliHFCutOptTreeHandler.cxx++g");
    /*gInterpreter->LoadMacro("AliAnalysisTaskSED0_TreeMVA.cxx++g");
    //AliAnalysisTaskSED0Mass_AF *task = reinterpret_cast<AliAnalysisTaskSED0Mass_AF*>(gInterpreter->ExecuteMacro("AddTaskD0Mass_AF.C"));
    AliAnalysisTaskSED0_TreeMVA *task = reinterpret_cast<AliAnalysisTaskSED0_TreeMVA*>(gInterpreter->ProcessLine(Form(".x %s (%d,%d,%d,%d,%d,%d,%f,%f,\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,%d)",gSystem->ExpandPathName("AddTaskD0_TreeMVA.C"),0, kTRUE, kFALSE, kFALSE, 1, 0, 0., 80., "_080", "../cutfiles/D0Cuts_Y_R6.root", "D0toKpiCuts",kFALSE, kFALSE, kFALSE,kFALSE, kFALSE, kFALSE,kFALSE)));
    
    */
    gInterpreter->LoadMacro("AliAnalysisTaskSEHFTreeCreator.cxx++g");
    AliAnalysisTaskSEHFTreeCreator *task = reinterpret_cast<AliAnalysisTaskSEHFTreeCreator*>(gInterpreter->ProcessLine(Form(".x %s (%d,%d,\"%s\",\"%s\")",gSystem->ExpandPathName("AddTaskHFTreeCreator.C"),kTRUE, 1, "HFTreeCreator", "./cutfile/D0DsDplusCuts.root")));
    
    
    AliAnalysisTaskSECleanupVertexingHF *taskclean =reinterpret_cast<AliAnalysisTaskSECleanupVertexingHF *>(gInterpreter->ProcessLine(Form(".x %s", gSystem->ExpandPathName("$ALICE_PHYSICS/PWGHF/vertexingHF/macros/AddTaskCleanupVertexingHF.C"))));

#else

    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
    AliPhysicsSelectionTask *physSelTask= AddTaskPhysicsSelection(kTRUE);
    
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
    AliAnalysisTaskPIDResponse *pidResp = AddTaskPIDResponse(kTRUE);
    
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AliMultSelectionTask *multSel = AddTaskMultSelection();
    //multSel->SetAlternateOADBforEstimators("LHC15o-DefaultMC-HIJING");

    gROOT->LoadMacro("AliHFCutOptTreeHandler.cxx++g");
    gROOT->LoadMacro("AliAnalysisTaskSED0_TreeMVA.cxx++g");
    gROOT->LoadMacro("AddTaskD0_TreeMVA.C");
    AliAnalysisTaskSED0_TreeMVA *task = AddTaskD0_TreeMVA(0, kTRUE, kFALSE, kFALSE, 1, 0, 0., 80., "_080", "./D0Cuts_Y_R5.root", "D0toKpiCuts",kFALSE, kFALSE, kFALSE,kFALSE, kFALSE, kFALSE,kFALSE);

    gROOT->LoadMacro("$ALICE_PHYSICS/PWGHF/vertexingHF/macros/AddTaskCleanupVertexingHF.C");
    AliAnalysisTaskSECleanupVertexingHF *taskclean = AddTaskCleanupVertexingHF();

#endif


    if(!mgr->InitAnalysis()) return;
    mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) {

        // if you want to run locally, we need to define some input
        TChain* chainAOD = new TChain("aodTree");
        TChain *chainAODfriend = new TChain("aodTree");

        // add a few files to the chain (change this so that your local files are added)
        //chainAOD->Add("../AODfiles/AliAOD.root");
        //chainAODfriend->Add("../AODfiles/AliAOD.VertexingHF.root");
        chainAOD->Add("../AODFiles/AliAOD.root");
        chainAODfriend->Add("../AODFiles/AliAOD.VertexingHF.root");

        chainAOD->AddFriend(chainAODfriend);


        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chainAOD);



    } else {

        // if we want to run on grid, we create and configure the plugin
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();

        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

        // make sure your source files get copied to grid
        alienHandler->SetAdditionalLibs("AliHFCutOptTreeHandler.cxx AliHFCutOptTreeHandler.h");
        alienHandler->SetAdditionalLibs("AliAnalysisTaskSED0_TreeMVA.cxx AliAnalysisTaskSED0_TreeMVA.h");
        alienHandler->SetAnalysisSource("AliAnalysisTaskSED0_TreeMVA.cxx");

        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion("vAN-20180903-1");

        // set the Alien API version
        alienHandler->SetAPIVersion("V1.1x");

        // select the input data
        alienHandler->SetGridDataDir("/alice/sim/2016/LHC16i2a/");
        alienHandler->SetDataPattern("/AOD198/*AliAOD.root");
        alienHandler->SetFriendChainName("AliAOD.VertexingHF.root");

        // MC has no prefix, data has prefix 000
        //alienHandler->SetRunPrefix("000");
        // runnumber
        const Int_t nruns = 1;
        Int_t runlist[nruns] = {246994};//, 246991, 246989, 246984, 246982};

        for(Int_t k=0; k<nruns; k++){
            alienHandler->AddRunNumber(runlist[k]);
        }
        alienHandler->SetNrunsPerMaster(nruns);

        // number of files per subjob
        alienHandler->SetSplitMaxInputFileNumber(5);
        alienHandler->SetExecutable("myTask.sh");

        // specify how many seconds your job may take
        alienHandler->SetTTL(10000);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);

        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate") 
        // (see below) mode, set SetMergeViaJDL(kFALSE) 
        // to collect final results
        alienHandler->SetMaxMergeStages(1); //2, 3
        alienHandler->SetMergeViaJDL(kTRUE);

        // define the output folders
        alienHandler->SetGridWorkingDir("testNtuple");
        alienHandler->SetGridOutputDir("output");

        // connect the alien plugin to the manager
        mgr->SetGridHandler(alienHandler);

        if(gridTest) {

            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(1);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            mgr->StartAnalysis("grid");
        } 
        else {
            // else launch the full grid analysis
            alienHandler->SetRunMode("full"); //terminate
            mgr->StartAnalysis("grid");
        }
    }
}
