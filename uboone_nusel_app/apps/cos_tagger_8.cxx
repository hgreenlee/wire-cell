// cz: code modified from tutorials/tmva/TMVAClassification.C

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"
#include "TMVA/Reader.h"

using namespace std;

TFile *input = 0;
TFile *output = 0;
TMVA::Factory *factory = 0;
TMVA::DataLoader *dataloader = 0;


void Run_r1();
void InitBDT_r1();
void TestEvaluate(TString filename);


void Run_r2();
void InitBDT_r2();


void convert_file();


void convert_file(){
 TFile *file0 = new TFile("bdt.root"); // sig x1

 
 TTree *sig = (TTree*)file0->Get("sig");
 TTree *bkg = (TTree*)file0->Get("bkg");


 float cosmict_flag_1;

 float cosmict_flag_8;  // muon + michel + special
 float cosmict_8_filled;
 float cosmict_8_flag_out;
 float cosmict_8_muon_length;
 float cosmict_8_acc_length;
 
 sig->SetBranchAddress("cosmict_flag_1",&cosmict_flag_1);
 
 sig->SetBranchAddress("cosmict_flag_8",&cosmict_flag_8);
 sig->SetBranchAddress("cosmict_8_filled",&cosmict_8_filled);
 sig->SetBranchAddress("cosmict_8_flag_out",&cosmict_8_flag_out);
 sig->SetBranchAddress("cosmict_8_muon_length",&cosmict_8_muon_length);
 sig->SetBranchAddress("cosmict_8_acc_length",&cosmict_8_acc_length);

 bkg->SetBranchAddress("cosmict_flag_1",&cosmict_flag_1);
 
 bkg->SetBranchAddress("cosmict_flag_8",&cosmict_flag_8);
 bkg->SetBranchAddress("cosmict_8_filled",&cosmict_8_filled);
 bkg->SetBranchAddress("cosmict_8_flag_out",&cosmict_8_flag_out);
 bkg->SetBranchAddress("cosmict_8_muon_length",&cosmict_8_muon_length);
 bkg->SetBranchAddress("cosmict_8_acc_length",&cosmict_8_acc_length);
 
 

 float weight;
 sig->SetBranchAddress("weight",&weight);
 bkg->SetBranchAddress("weight",&weight);
 
 TFile *new_file = new TFile("round_0.root","RECREATE");
 TTree *Stree = new TTree("TreeS","signal tree");
 TTree *Btree = new TTree("TreeB","background tree");
 Stree->SetDirectory(new_file);
 Btree->SetDirectory(new_file);



 Stree->Branch("weight",&weight,"weight/F");
 Btree->Branch("weight",&weight,"weight/F");
 
 Stree->Branch("cosmict_flag_1",&cosmict_flag_1,"cosmict_flag_1/F");
 Btree->Branch("cosmict_flag_1",&cosmict_flag_1,"cosmict_flag_1/F");

 
 Stree->Branch("cosmict_flag_8",&cosmict_flag_8,"cosmict_flag_8/F");
 Stree->Branch("cosmict_8_filled",&cosmict_8_filled,"cosmict_8_filled/F");
 Stree->Branch("cosmict_8_flag_out",&cosmict_8_flag_out,"cosmict_8_flag_out/F");
 Stree->Branch("cosmict_8_muon_length",&cosmict_8_muon_length,"cosmict_8_muon_length/F");
 Stree->Branch("cosmict_8_acc_length",&cosmict_8_acc_length,"cosmict_8_acc_length/F");

 Btree->Branch("cosmict_flag_8",&cosmict_flag_8,"cosmict_flag_8/F");
 Btree->Branch("cosmict_8_filled",&cosmict_8_filled,"cosmict_8_filled/F");
 Btree->Branch("cosmict_8_flag_out",&cosmict_8_flag_out,"cosmict_8_flag_out/F");
 Btree->Branch("cosmict_8_muon_length",&cosmict_8_muon_length,"cosmict_8_muon_length/F");
 Btree->Branch("cosmict_8_acc_length",&cosmict_8_acc_length,"cosmict_8_acc_length/F");

 
 for (Int_t i=0;i!=sig->GetEntries();i++){
    sig->GetEntry(i);
     
    Stree->Fill();
 }

 for (Int_t i=0;i!=bkg->GetEntries();i++){
   bkg->GetEntry(i);
    
   Btree->Fill();
 }



 cout << "signal tree entries: " << Stree->GetEntries() << " / " << Stree->GetEntries("cosmict_8_filled>0 && cosmict_flag_1==0")<< endl;
 cout << "background tree entries: " << Btree->GetEntries() << " / " << Btree->GetEntries("cosmict_8_filled>0 && (cosmict_flag_8>0 )&& cosmict_flag_1==0")<< endl;
  
 new_file->Write();
 new_file->Close();
 
 
}

void Run_r2(){
  TMVA::Tools::Instance();
  
  TString fname = "./round_1.root";
  input = TFile::Open( fname ); // check if file in local directory exists
  cout << "Using input file: " << input->GetName() << std::endl;

  TString outfileName( "TMVA_r2.root" );
  output = TFile::Open( outfileName, "RECREATE" );

  InitBDT_r2();

  // Train MVAs using the set of training events
  factory->TrainAllMethods();
  
  // Evaluate all MVAs using the set of test events
  factory->TestAllMethods();
  
  // Evaluate and compare performance of all configured MVAs
  factory->EvaluateAllMethods();
  
  output->Close();
  cout << "Results saved at: " << output->GetName() << std::endl;
  
  delete factory;
  delete dataloader;
  
  TestEvaluate("round_2.root");
  
  // Launch the GUI for the root macros
  if (!gROOT->IsBatch()) TMVA::TMVAGui( output->GetName() );
  
}

void Run_r1()
{
  TMVA::Tools::Instance();
  
  TString fname = "./round_0.root";
  input = TFile::Open( fname ); // check if file in local directory exists
  cout << "Using input file: " << input->GetName() << std::endl;
  
  TString outfileName( "TMVA.root" );
  output = TFile::Open( outfileName, "RECREATE" );
  
  
  InitBDT_r1();
  
  
  // Train MVAs using the set of training events
  factory->TrainAllMethods();
  
  // Evaluate all MVAs using the set of test events
  factory->TestAllMethods();
  
  // Evaluate and compare performance of all configured MVAs
  factory->EvaluateAllMethods();
  
  output->Close();
  cout << "Results saved at: " << output->GetName() << std::endl;
  
  delete factory;
  delete dataloader;
  
  TestEvaluate("round_1.root");
  
  // Launch the GUI for the root macros
  if (!gROOT->IsBatch()) TMVA::TMVAGui( output->GetName() );
  
}




void InitBDT_r1()
{
  factory = new TMVA::Factory( "Test", output,
			       "!V:!Silent:Color:DrawProgressBar:"
			       //    "Transformations=I;D;P;G,D:"
			       "AnalysisType=Classification" );
  
  
  
  dataloader = new TMVA::DataLoader("dataset");
  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
 

 dataloader->AddVariable( "cosmict_8_flag_out", "cosmict_8_flag_out", "", 'F' );
 dataloader->AddVariable( "cosmict_8_muon_length", "cosmict_8_muon_length", "cm", 'F' );
 dataloader->AddVariable( "cosmict_8_acc_length", "cosmict_8_acc_length", "cm", 'F' );
 

  
 
  TTree *signalTree     = (TTree*)input->Get("TreeS");
  TTree *backgroundTree = (TTree*)input->Get("TreeB");
  dataloader->AddSignalTree(signalTree, 1.0); // can add the global event weight
  dataloader->AddBackgroundTree( backgroundTree, 1.0);
  // Set individual event weights (the variables must exist in the original TTree)
  dataloader->SetSignalWeightExpression( "weight" );
  dataloader->SetBackgroundWeightExpression( "weight" );

  // Apply additional cuts on the signal and background samples (can be different)
  TCut mycut_s = "cosmict_8_filled>0 && cosmict_flag_1==0"; //  90450
  TCut mycut_b = "cosmict_8_filled>0 && (1>0 ) && cosmict_flag_1==0"; //  7643
    
  dataloader->PrepareTrainingAndTestTree( mycut_s, mycut_b,
					  "nTrain_Signal=40000:"
					  "nTrain_Background=6500:"
					  "nTest_Signal=4000:"
					  "nTest_Background=1143:"
					  "SplitMode=Random:"
					  "NormMode=NumEvents:"
					  "!V" );

    // variations of BDTs
    // BDT: uses Adaptive Boost
    // BDTG: uses Gradient Boost
    // BDTB: uses Bagging
    // BDTD: decorrelation + Adaptive Boost
    // BDTF: allow usage of fisher discriminant for node splitting
  factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
		      "!H:!V:"
		      "VarTransform=D,P,U,G,D,N:"
		      "NTrees=850:"
		      "MinNodeSize=2.5%:"
		      "MaxDepth=3:"
		      "BoostType=AdaBoost:"
		      "AdaBoostBeta=0.5:"
		      "UseBaggedBoost:"
		      "BaggedSampleFraction=0.5:"
		      "SeparationType=GiniIndex:"
		      "nCuts=20");

}

void InitBDT_r2()
{
    factory = new TMVA::Factory( "Test", output,
				 "!V:!Silent:Color:DrawProgressBar:"
				 //    "Transformations=I;D;P;G,D:"
				 "AnalysisType=Classification" );
    			 


    dataloader = new TMVA::DataLoader("dataset");
    // Define the input variables that shall be used for the MVA training
    // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
    // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
    
    
    dataloader->AddVariable( "cosmict_3_flag_inside", "cosmict_3_flag_inside", "", 'F' );
    dataloader->AddVariable( "cosmict_3_angle_beam", "cosmict_3_angle_beam", "deg", 'F' );
    dataloader->AddVariable( "cosmict_3_flag_dir_weak", "cosmict_3_flag_dir_weak", "", 'F' );
    dataloader->AddVariable( "cosmict_3_dQ_dx_end", "cosmict_3_dQ_dx_end", "MeV/cm", 'F' );
    dataloader->AddVariable( "cosmict_3_dQ_dx_front", "cosmict_3_dQ_dx_front", "MeV/cm", 'F' );
    dataloader->AddVariable( "cosmict_3_theta", "cosmict_3_theta", "deg", 'F' );
    dataloader->AddVariable( "cosmict_3_phi", "cosmict_3_phi", "deg", 'F' );
    dataloader->AddVariable( "cosmict_3_valid_tracks", "cosmict_3_valid_tracks", "", 'F' );
    dataloader->AddVariable( "cosmict_5_connected_showers", "cosmict_5_connected_showers", "", 'F' );
    
    
    TTree *signalTree     = (TTree*)input->Get("TreeS");
    TTree *backgroundTree = (TTree*)input->Get("TreeB");
    dataloader->AddSignalTree(signalTree, 1.0); // can add the global event weight
    dataloader->AddBackgroundTree( backgroundTree, 1.0);
    // Set individual event weights (the variables must exist in the original TTree)
    dataloader->SetSignalWeightExpression( "weight" );
    dataloader->SetBackgroundWeightExpression( "weight" );

    // Apply additional cuts on the signal and background samples (can be different)
    TCut mycut_s = "cosmict_3_filled>0 && cosmict_flag_1==0"; // 
    TCut mycut_b = "cosmict_3_filled>0 && (cosmict_flag_3>0 || cosmict_flag_5>0 || cosmict_3_5_bdt < 0.2) && cosmict_flag_1==0"; // 1956

    dataloader->PrepareTrainingAndTestTree( mycut_s, mycut_b,
					  "nTrain_Signal=3400:"
					  "nTrain_Background=210:"
					  "nTest_Signal=722:"
					  "nTest_Background=50:"
					  "SplitMode=Random:"
					  "NormMode=NumEvents:"
					  "!V" );
   
    

    // variations of BDTs
    // BDT: uses Adaptive Boost
    // BDTG: uses Gradient Boost
    // BDTB: uses Bagging
    // BDTD: decorrelation + Adaptive Boost
    // BDTF: allow usage of fisher discriminant for node splitting
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
        "!H:!V:"
       	"VarTransform=D,P,U,G,D,N:"
        "NTrees=850:"
        "MinNodeSize=2.5%:"
        "MaxDepth=3:"
        "BoostType=AdaBoost:"
        "AdaBoostBeta=0.5:"
        "UseBaggedBoost:"
        "BaggedSampleFraction=0.5:"
        "SeparationType=GiniIndex:"
        "nCuts=20");

}


void TestEvaluate(TString filename)
{
  float cosmict_flag_1;

  float cosmict_flag_8;  // muon + michel + special
 float cosmict_8_filled;
 float cosmict_8_flag_out;
 float cosmict_8_muon_length;
 float cosmict_8_acc_length;
 


  
  
  
  float weight;
  
  TFile *file = new TFile("round_0.root");
  TTree *sig = (TTree*)file->Get("TreeS");
  TTree *bkg = (TTree*)file->Get("TreeB");

  sig->SetBranchAddress("cosmict_flag_1",&cosmict_flag_1);

  sig->SetBranchAddress("cosmict_flag_8",&cosmict_flag_8);
  sig->SetBranchAddress("cosmict_8_filled",&cosmict_8_filled);
  sig->SetBranchAddress("cosmict_8_flag_out",&cosmict_8_flag_out);
  sig->SetBranchAddress("cosmict_8_muon_length",&cosmict_8_muon_length);
  sig->SetBranchAddress("cosmict_8_acc_length",&cosmict_8_acc_length);

 
  
  bkg->SetBranchAddress("cosmict_flag_1",&cosmict_flag_1);
  
   bkg->SetBranchAddress("cosmict_flag_8",&cosmict_flag_8);
  bkg->SetBranchAddress("cosmict_8_filled",&cosmict_8_filled);
  bkg->SetBranchAddress("cosmict_8_flag_out",&cosmict_8_flag_out);
  bkg->SetBranchAddress("cosmict_8_muon_length",&cosmict_8_muon_length);
  bkg->SetBranchAddress("cosmict_8_acc_length",&cosmict_8_acc_length);
  
  
  TFile *new_file = new TFile(filename,"RECREATE");
  TTree *Tsig = new TTree("TreeS","TreeS");
  TTree *Tbkg = new TTree("TreeB","TreeB");
  Tsig->SetDirectory(new_file);
  Tbkg->SetDirectory(new_file);

  
  Tsig->Branch("weight",&weight,"weight/F");
  Tsig->Branch("cosmict_flag_1",&cosmict_flag_1,"cosmict_flag_1/F");
 

  Tbkg->Branch("weight",&weight,"weight/F");
  Tbkg->Branch("cosmict_flag_1",&cosmict_flag_1,"cosmict_flag_1/F");
  
 
  Tsig->Branch("cosmict_flag_8",&cosmict_flag_8,"cosmict_flag_8/F");
  Tsig->Branch("cosmict_8_filled",&cosmict_8_filled,"cosmict_8_filled/F");
  Tsig->Branch("cosmict_8_flag_out",&cosmict_8_flag_out,"cosmict_8_flag_out/F");
  Tsig->Branch("cosmict_8_muon_length",&cosmict_8_muon_length,"cosmict_8_muon_length/F");
  Tsig->Branch("cosmict_8_acc_length",&cosmict_8_acc_length,"cosmict_8_acc_length/F");
  
  Tbkg->Branch("cosmict_flag_8",&cosmict_flag_8,"cosmict_flag_8/F");
  Tbkg->Branch("cosmict_8_filled",&cosmict_8_filled,"cosmict_8_filled/F");
  Tbkg->Branch("cosmict_8_flag_out",&cosmict_8_flag_out,"cosmict_8_flag_out/F");
  Tbkg->Branch("cosmict_8_muon_length",&cosmict_8_muon_length,"cosmict_8_muon_length/F");
  Tbkg->Branch("cosmict_8_acc_length",&cosmict_8_acc_length,"cosmict_8_acc_length/F");
 
  
  sig->SetBranchAddress("weight",&weight);
  bkg->SetBranchAddress("weight",&weight);
  
  float bdt_value;
  Tsig->Branch("cosmict_8_bdt", &bdt_value, "cosmict_8_bdt/F");
  Tbkg->Branch("cosmict_8_bdt", &bdt_value, "cosmict_8_bdt/F");
  
  TMVA::Reader *reader = new TMVA::Reader();
  reader->AddVariable( "cosmict_8_flag_out", &cosmict_8_flag_out);
  reader->AddVariable( "cosmict_8_muon_length", &cosmict_8_muon_length);
  reader->AddVariable( "cosmict_8_acc_length", &cosmict_8_acc_length);
  reader->BookMVA( "MyBDT", "dataset/weights/Test_BDT.weights.xml");
  
  
  for (Int_t i=0;i!=sig->GetEntries();i++){
    sig->GetEntry(i);
    bdt_value = reader->EvaluateMVA("MyBDT");
    Tsig->Fill();
  }
  
  for (Int_t i=0;i!=bkg->GetEntries();i++){
    bkg->GetEntry(i);
    bdt_value = reader->EvaluateMVA("MyBDT");
    Tbkg->Fill();
  }

  new_file->Write();
  new_file->Close();
  

  
}




int main( int argc, char** argv )
{
  int process = 1;

  for (Int_t i=1;i!=argc;i++){
    switch(argv[i][1]){
    case 'p':
      process = atoi(&argv[i][2]);
      break;
    }
  }

  if (process==1){
    std::cout << "Prepare the initial rootfile" << std::endl;
    convert_file();
  }else if (process == 2){
    std::cout << "testing BDT in ROOT TMVA (1st round)" << std::endl;
    Run_r1();
  }else if (process == 3){
    std::cout << "testing BDT in ROOT TMVA (2nd round)" << std::endl;
    Run_r2();
  }
  
  return 1;
    
}

