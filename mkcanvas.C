void mkcanvas(){

  Int_t num[2][5]={{7,15,32,33,0},{10,15,31,33,34}};
  TH2F *hh[10];
  for(Int_t i=0;i<10;i++){
    hh[i] = (TH2F*)gROOT ->FindObject(TString::Format("hth2ph%i",i));
  }
  TH2F *hud[2];
  hud[0] = (TH2F*)gROOT ->FindObject("hUp");
  hud[1] = (TH2F*)gROOT ->FindObject("hDown");

  TCanvas *cud = new TCanvas("cud","cud",600,1000);
  cud->Divide(1,2);
  cud->cd(1);
  hud[0]->Draw();
  cud->Update();
  tread->Draw("phdeg[0]:thdeg[0]>>hu(150,60,90,200,0,40)","ny==32||ny==33||nx==7||nx==15","same,colz");
  cud->cd(2);
  hud[1]->Draw();
  cud->Update();
  tread->Draw("phdeg[1]:thdeg[1]>>hd(150,60,90,200,-60,-20)","ny==31||ny==33||ny==34||nx==10||nx==15","same,colz");
  
  // TCanvas *c[2];
  // c[0] = new TCanvas("cup","cup",1200,1000);
  // c[1] = new TCanvas("cdown","cdown",1200,1000);
  // for(Int_t ud=0;ud<2;ud++){
  //   c[ud]->Divide(3,2);
  //   c[ud]->cd(1);
  //   hh[ud*2+0]->Draw();
  //   // tread->Draw(((TString::Format("thdeg[%i]:phdeg[%i]",ud,ud)).Data()),TString::Format("nx==%i",num[ud][0]),"same,colz");
  //   c[ud]->cd(2);
  //   hh[ud*2+1]->Draw();
  //   // tread->Draw(((TString::Format("thdeg[%i]:phdeg[%i]",ud,ud)).Data()),TString::Format("nx==%i",num[ud][1]),"same,colz");

  //   c[ud]->cd(4);
  //   hh[ud*3+4]->Draw();
  //   // tread->Draw(((TString::Format("phdeg[%i]:thdeg[%i]",ud,ud)).Data()),TString::Format("ny==%i",num[ud][2]),"same,colz");
  //   c[ud]->cd(5);
  //   hh[ud*3+5]->Draw();
  //   // tread->Draw(((TString::Format("phdeg[%i]:thdeg[%i]",ud,ud)).Data()),TString::Format("ny==%i",num[ud][3]),"same,colz");
  //   c[ud]->cd(6);
  //   hh[ud*3+6]->Draw();
  //   // tread->Draw(((TString::Format("phdeg[%i]:thdeg[%i]",ud,ud)).Data()),TString::Format("ny==%i",num[ud][4]),"same,colz");

  // }

  
}
