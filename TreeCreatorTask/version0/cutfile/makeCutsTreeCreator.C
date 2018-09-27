#include <Riostream.h>
#include <TFile.h>
#include <AliRDHFCutsD0toKpi.h>
#include <AliRDHFCutsDstoKKpi.h>
#include <AliRDHFCutsDplustoKpipi.h>
#include <TClonesArray.h>
#include <TParameter.h>

#include "makeInputCutsD0toKpi.C"
#include "makeInputCutsDstoKKpi.C"
#include "makeInputCutsDplustoKpipi.C"


/*
 AliRDHFCutsD0toKpi*      looseCutsD0toKpi        =(AliRDHFCutsD0toKpi*)filecuts->Get("D0toKpiFilteringCuts");
 AliRDHFCutsDstoKKpi*     looseCutsDstoKKpi       =(AliRDHFCutsDstoKKpi*)filecuts->Get("DstoKKpiFilteringCuts");
 AliRDHFCutsDplustoKpipi* looseCutsDplustoKpipi   =(AliRDHFCutsDplustoKpipi*)filecuts->Get("DplustoKpipiFilteringCuts");
 AliRDHFCutsD0toKpi*      analysisCutsD0toKpi     =(AliRDHFCutsD0toKpi*)filecuts->Get("D0toKpiAnalysisCuts");
 AliRDHFCutsDstoKKpi*     analysisCutsDstoKKpi    =(AliRDHFCutsDstoKKpi*)filecuts->Get("DstoKKpiAnalysisCuts");
 AliRDHFCutsDplustoKpipi* analysisCutsDplustoKpipi=(AliRDHFCutsDplustoKpipi*)filecuts->Get("DplustoKpipiAnalysisCuts");
 */



void makeCutsTreeCreator()
{
    
    AliRDHFCutsD0toKpi  *looseCutsD0toKpi    = makeInputCutsD0toKpi(0,"D0toKpiFilteringCuts",0.,100.);
    AliRDHFCutsD0toKpi  *analysisCutsD0toKpi = makeInputCutsD0toKpi(1,"D0toKpiAnalysisCuts",0.,100.);
    
    AliRDHFCutsDstoKKpi  *looseCutsDstoKKpi    = makeInputCutsDstoKKpi(0,"DstoKKpiFilteringCuts",0.,100.);
    AliRDHFCutsDstoKKpi  *analysisCutsDstoKKpi = makeInputCutsDstoKKpi(1,"DstoKKpiAnalysisCuts",0.,100.);
    
    AliRDHFCutsDplustoKpipi  *looseCutsDplustoKpipi    = makeInputCutsDplustoKpipi(0,"DplustoKpipiFilteringCuts",0.,100.);
    AliRDHFCutsDplustoKpipi  *analysisCutsDplustoKpipi = makeInputCutsDplustoKpipi(1,"DplustoKpipiAnalysisCuts",0.,100.);
    
    TFile* fout=new TFile("D0DsDplusCuts.root","recreate");
    fout->cd();
    looseCutsD0toKpi->Write();
    analysisCutsD0toKpi->Write();
    looseCutsDstoKKpi->Write();
    analysisCutsDstoKKpi->Write();
    looseCutsDplustoKpipi->Write();
    analysisCutsDplustoKpipi->Write();
    fout->Close();
    
}


