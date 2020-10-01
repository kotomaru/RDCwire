#ifndef __CINT__
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TEllipse.h>
#include <TString.h>
#include <TLatex.h>
#include <TCutG.h>
#include <TVector3.h>
#include <TText.h>

#endif

#define PI 3.14159265358979323846

#define REAL 0
#define IMAGE 1

using namespace std;

const Double_t rgeom[2]={21.7,21.7};//cm //!!!!RIBF2016
const Double_t r2d = 57.2958;
const Double_t d2r = 0.0174533;

int main(int argc, char * argv[]){

  /** Wire Reading Result   **/
  ifstream finup("UpReadWire.csv",ios::in);
  ifstream findw("DownReadWire.csv",ios::in);
  ofstream fout("chi2result.txt",ios::out);
  if(!finup){
    cerr<<"Error UpReadWire file open"<<endl;
    return 1;
  }
  if(!findw){
    cerr<<"Error DownReadWire file open"<<endl;
    return 1;
  }
  Int_t readx[2][200]={{}};
  Int_t ready[2][200]={{}};
  Double_t readth[2][200]={{}};//degree theta
  double_t readph[2][200]={{}};//degree phi
  Int_t a,b; Double_t c,d;
  Int_t num[2]={0};
  while((finup>>a>>b>>c>>d)){
    readx[0][num[0]]=a; ready[0][num[0]]=b;
    readth[0][num[0]]=c; readph[0][num[0]++]=d;
  }
  while((findw>>a>>b>>c>>d)){
    readx[1][num[1]]=a; ready[1][num[1]]=b;
    readth[1][num[1]]=c; readph[1][num[1]++]=d;
    //    cout<<num<<endl;
  }

  Int_t readxt[2]={},readyt[2]={};
  Double_t readtht[2]={},readpht[2]={};
  TTree *tread = new TTree("tread","tread");
  tread->Branch("nx",&readxt,"nx[2]/I");
  tread->Branch("ny",&readyt,"ny[2]/I");
  tread->Branch("thdeg",&readtht,"thdeg[2]/D");
  tread->Branch("phdeg",&readpht,"phdeg[2]/D");
  
  //*******BDC &  RDC  ***********
  Double_t cx_tgt=0.005;Double_t cy_tgt=-0.055;//center of SHT

  Double_t vrx[2][4];//[up, down][X,th,Y,ph]
  Double_t theta[2],phi[2];
  Double_t wspace=1.4;

  Int_t xwire[2][2]={{7,15},{10,15}};Int_t ywire[2][3]={{32,33,-1},{31,33,34}};
  Int_t xw[2]={-1};Int_t yw[2]={-1};
//*********new TTree for analysis*************/

  string output = "rdcwire.root";
  //  TFile *anafile = new TFile(Form("%s",output.c_str()),"RECREATE");
  TTree *tana = new TTree("tana","tana");
  tana->Branch("theta",&theta,"theta[2]/D");
  tana->Branch("phi",&phi,"phi[2]/D");
  tana->Branch("xwire",&xw,"xwire[2]/I");
  tana->Branch("ywire",&yw,"ywire[2]/I");

  TH2F *hth2ph[2][5];//[ud][X,X,Y,Y,Y]
  hth2ph[0][0]=new TH2F("hth2ph0","UpX=7",700,-30,40,500,60,110);
  hth2ph[0][1]=new TH2F("hth2ph1","UpX=15",700,-30,40,500,60,110);
  hth2ph[1][0]=new TH2F("hth2ph2","DwX=10",900,-60,30,500,60,110);
  hth2ph[1][1]=new TH2F("hth2ph3","DwX=15",900,-60,30,500,60,110);
  hth2ph[0][2]=new TH2F("hth2ph4","UpY=32",600,60,90,700,-30,40);
  hth2ph[0][3]=new TH2F("hth2ph5","UpY=33",600,60,90,700,-30,40);
  hth2ph[0][4]=new TH2F("hth2ph6","UpY=-1 dummy",600,60,90,700,-30,40);
  hth2ph[1][2]=new TH2F("hth2ph7","DwY=31",600,60,90,900,-60,30);
  hth2ph[1][3]=new TH2F("hth2ph8","DwY=33",600,60,90,900,-60,30);
  hth2ph[1][4]=new TH2F("hth2ph9","DwY=34",600,60,90,900,-60,30);
  TH2F *hth2phfin[2][5];//[ud][X,X,Y,Y,Y]
  
  TF1 *ffit[2][5];
  Double_t par[2][5][3];
  for(Int_t ud=0;ud<2;ud++){
    for(Int_t i=0;i<5;i++)
      ffit[ud][i]=new TF1(TString::Format("fit%i",5*ud+i),"pol2");
  }
  /**************************************************************/
  
  TVector3 bvec;
  bvec.SetXYZ(0.0,0.0,1.);

  // Double_t pos[3][3]={{19.84,0.0,100.5},{19.84,0.0,100.5},{cx_tgt,cy_tgt,0.}};//aligment of RDCup, RDCdw, target 
  // Double_t ang[3][3]={{-4.145,12.0,0.0},{-4.145,-28.0,0.0},{0.,0.,0.}};//aligment of RDCup, RDCdw, dummy

  //initial value
  Double_t pos[3][3]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
  Double_t ang[3][3]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
  
  const Double_t posmin[3][3]={{24.0,-1.0,105.0},{28.0,0.0,109.0},{cx_tgt,cy_tgt,0.}};
  const Double_t posmax[3][3]={{24.5,0.0,106.5},{28.5,1.0,110.5},{cx_tgt,cy_tgt,0.}};
  const Double_t angmin[3][3]={{-3.5,15.0,6.0},{-5.0,-36.0,-13.0},{0.,0.,0.}};
  const Double_t angmax[3][3]={{-2.5,16.0,7.0},{-4.0,-35.0,-12.0},{0.,0.,0.}};
  Double_t posstp[3][3]={{0.25,0.25,0.25},{0.25,0.25,0.25},{0.0,0.0,0.0}};
  Double_t angstp[3][3]={{0.25,0.25,0.25},{0.25,0.25,0.25},{0.0,0.0,0.0}};
  Double_t posfin[3][3]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
  Double_t angfin[3][3]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

  Int_t nstp=0;
  for(Int_t i=0;i<3;i++){
    nstp = nstp * ((Int_t)((posmax[0][i]-posmin[0][i])/posstp[0][i]) + 1);
    nstp = nstp * ((Int_t)((angmax[0][i]-angmin[0][i])/angstp[0][i]) + 1);
  }

  
  Double_t chi2[2][5]={{}};
  Double_t chi2sum[2]={};
  Double_t tmpchi2sum[2]={};

  for(Int_t i=0;i<3;i++){
    for(Int_t j=0;j<3;j++){
      pos[i][j]=posmin[i][j]; ang[i][j]=angmin[i][j];
    }
  }
  for(Int_t i=0;i<2;i++) chi2sum[i]=999999;
  Int_t nroop=0;
  while(pos[0][0]<=posmax[0][0]){
    pos[0][1]=posmin[0][1]; pos[1][1]=posmin[1][1];
    while(pos[0][1]<=posmax[0][1]){
      pos[0][2]=posmin[0][2]; pos[1][2]=posmin[1][2];
      while(pos[0][2]<=posmax[0][2]){
	ang[0][0]=angmin[0][0]; ang[1][0]=angmin[1][0];
	while(ang[0][0]<=angmax[0][0]){
	  ang[0][1]=angmin[0][1]; ang[1][1]=angmin[1][1];
	  while(ang[0][1]<=angmax[0][1]){
	    ang[0][2]=angmin[0][2]; ang[1][2]=angmin[1][2];
	    while(ang[0][2]<=angmax[0][2]){
	      nroop++;
	      for(Int_t ud=0;ud<2;ud++){
		for(Int_t i=0;i<5;i++) hth2ph[ud][i]->Reset("ICES");
	      }
	      /* ESPRIRDC*/
	      //Y wire const, X change
	      
	      tmpchi2sum[0]=0;tmpchi2sum[1]=0;
	      for(Int_t ny=0;ny<3;ny++){
		for(Int_t ud=0 ; ud<2 ; ud++){
		  for(Int_t i=0;i<2;i++)
		    yw[i]=-1;
		  for(Int_t i=0 ; i<2 ; i++){
		    theta[i]=0.;phi[i]=400;
		  }
		  for(Int_t j=0;j<4;j++){
		    vrx[ud][j]=-9999;
		  }
		  yw[ud]=ywire[ud][ny]; //yw[(ud+1)%2]=-1;
		  
		  if(yw[ud]!=-1){
		    vrx[ud][2] = 43.4-((Int_t)yw[ud]/2+(yw[ud]%2)*0.5 -1)*wspace - rgeom[1];
		    for(Double_t ncm=0;ncm<43.4;ncm=ncm+0.2){
		      vrx[ud][0] = ncm-rgeom[1];
		      TVector3 rvec, rpos, roff, toff;
		      rpos.SetXYZ(0.0,vrx[ud][2],vrx[ud][0]);
		      rpos.RotateX(ang[ud][2]*d2r);
		      roff.SetXYZ(pos[ud][2]-2.0,pos[ud][1],pos[ud][0]);
		      rpos += roff;
		      rpos.RotateZ(ang[ud][1]*d2r);//+12/-28 deg
		      rpos.RotateY(ang[ud][0]*d2r);//4 deg
		      toff.SetXYZ(pos[2][0],pos[2][1],pos[2][2]);
		      rvec=rpos - toff;
		      theta[ud] = rvec.Angle(bvec);
		      phi[ud] = rvec.Phi();
		      hth2ph[ud][2+ny]->Fill(theta[ud]*r2d,phi[ud]*r2d);
		    }//ncm
		  }
		  if(yw[ud]!=-1){
		    hth2ph[ud][2+ny]->Fit(TString::Format("fit%i",5*ud+2+ny),"N0");
		    ffit[ud][2+ny]->GetParameters(&par[ud][2+ny][0]);
		    //calculate chi square
		    chi2[ud][2+ny]=0.;
		    for(Int_t i=0;i<num[ud];i++){
		      if(yw[ud]==ready[ud][i]){
			Double_t tmpchi =readph[ud][i]
			  -(par[ud][2+ny][0]+par[ud][2+ny][1]*readth[ud][i]
			    +par[ud][2+ny][2]*readth[ud][i]*readth[ud][i]); 
			chi2[ud][2+ny] = chi2[ud][2+ny]+pow(tmpchi,2);
		      }
		    }
		  }
		  tmpchi2sum[ud] = tmpchi2sum[ud]+chi2[ud][2+ny]; 	  
		}//ud finish
	      }//ny finish
	      
	      //X 
	      for(Int_t nx=0;nx<2;nx++){
		for(Int_t ud=0 ; ud<2 ; ud++){
		  for(Int_t i=0;i<2;i++)
		    xw[i]=-1;
		  for(Int_t i=0 ; i<2 ; i++){
		    theta[i]=0.;phi[i]=400;
		  }
		  for(Int_t j=0;j<4;j++){
		    vrx[ud][j]=-9999;
		  }
		  xw[ud]=xwire[ud][nx];
		  if(xw[ud]!=-1){
		    vrx[ud][0] = ((Int_t)xw[ud]/2+(xw[ud]%2)*0.5 -1-0.5)*wspace
		      -rgeom[ud];
		    for(Double_t ncm=0;ncm<43.4;ncm=ncm+0.2){
		      vrx[ud][2] = ncm-rgeom[0];
		      TVector3 rvec, rpos, roff, toff;
		      rpos.SetXYZ(0.0,vrx[ud][2],vrx[ud][0]);
		      rpos.RotateX(ang[ud][2]*d2r);
		      roff.SetXYZ(pos[ud][2]-3.0,pos[ud][1],pos[ud][0]);
		      rpos += roff;
		      rpos.RotateZ(ang[ud][1]*d2r);//+12/-28 deg
		      rpos.RotateY(ang[ud][0]*d2r);//4 deg
		      toff.SetXYZ(pos[2][0],pos[2][1],pos[2][2]);
		      rvec=rpos - toff;
		      theta[ud] = rvec.Angle(bvec);
		      phi[ud] = rvec.Phi();
		      //		cout<<"!!! "<<theta[ud]*r2d<<" "<<phi[ud]*r2d<<endl;
		      hth2ph[ud][nx]->Fill(phi[ud]*r2d,theta[ud]*r2d);
		      // if(ud==1&&nx==1){
		      //   cout<<theta[ud]*r2d<<" "<<phi[ud]*r2d<<endl;
		      // }
		    }//ncm
		  }
		  if(xw[ud]!=-1){
		    hth2ph[ud][nx]->Fit(TString::Format("fit%i",5*ud+nx),"");
		    ffit[ud][nx]->GetParameters(&par[ud][nx][0]);
		    //calculate chi square
		    chi2[ud][nx]=0.;
		    for(Int_t i=0;i<num[ud];i++){
		      if(xw[ud]==readx[ud][i]){
			Double_t tmpchi =readth[ud][i]
			  -(par[ud][nx][0]+par[ud][nx][1]*readph[ud][i]
			    +par[ud][nx][2]*readph[ud][i]*readph[ud][i]); 
			chi2[ud][nx] = chi2[ud][nx]+pow(tmpchi,2);
		      }
		    }
		  }
		  tmpchi2sum[ud] = tmpchi2sum[ud]+chi2[ud][nx]; 	  
		}//ud finish
	      }//nx finish
	      for(Int_t ud=0;ud<2;ud++){
		if(tmpchi2sum[ud]<chi2sum[ud]){////!!!!!!
		  chi2sum[ud]=tmpchi2sum[ud];
		  posfin[ud][0]=pos[ud][0];
		  posfin[ud][1]=pos[ud][1];
		  posfin[ud][2]=pos[ud][2];
		  angfin[ud][0]=ang[ud][0];
		  angfin[ud][1]=ang[ud][1];
		  angfin[ud][2]=ang[ud][2];
		  fout
		    <<posfin[0][0]<<" "<<posfin[0][1]<<" "<<posfin[0][2]
		    <<" "<<angfin[0][0]<<" "<<angfin[0][1]<<" "<<angfin[0][2]
		    <<" : "<<chi2sum[0]
		    <<"  ::: "<<posfin[1][0]<<" "<<posfin[1][1]<<" "<<posfin[1][2]
		    <<" "<<angfin[1][0]<<" "<<angfin[1][1]<<" "<<angfin[1][2]
		    <<" : "<<chi2sum[1]<<endl;
		  
		}
		cout<<nroop<<" / "<<nstp<<endl;
		cout<<"   : "<<pos[0][0]<<" "<<pos[0][1]<<" "<<pos[0][2]
		    <<" "<<ang[0][0]<<" "<<ang[0][1]<<" "<<ang[0][2]
		  <<" : "<<tmpchi2sum[0]<<endl;
		cout<<"   : "<<pos[1][0]<<" "<<pos[1][1]<<" "<<pos[1][2]
		    <<" "<<ang[1][0]<<" "<<ang[1][1]<<" "<<ang[1][2]
		    <<" : "<<tmpchi2sum[1]<<endl;
		
		cout<<" :pos[][0] pos[][1] pos[][2] ang[][0] ang[][1] ang[][2] :chisquare"<<endl;
		cout<<"up : "<<posfin[0][0]<<" "<<posfin[0][1]<<" "<<posfin[0][2]
		    <<" "<<angfin[0][0]<<" "<<angfin[0][1]<<" "<<angfin[0][2]
		    <<" : "<<chi2sum[0]<<endl;
		cout<<"dw : "<<posfin[1][0]<<" "<<posfin[1][1]<<" "<<posfin[1][2]
		    <<" "<<angfin[1][0]<<" "<<angfin[1][1]<<" "<<angfin[1][2]
		    <<" : "<<chi2sum[1]<<endl;
	      }
	      ang[0][2]=ang[0][2]+angstp[0][2];ang[1][2]=ang[1][2]+angstp[1][2];
	      if(ang[0][2]>angmax[0][2])
		break;
	    }
	    ang[0][1]=ang[0][1]+angstp[0][1];  ang[1][1]=ang[1][1]+angstp[1][1];
	    if(ang[0][1]>angmax[0][1])
	      break;
	  }
	  ang[0][0]=ang[0][0]+angstp[0][0];  ang[1][0]=ang[1][0]+angstp[1][0];	
	  if(ang[0][0]>angmax[0][0])
	    break;
	}
	pos[0][2]=pos[0][2]+posstp[0][2];  pos[1][2]=pos[1][2]+posstp[1][2];	
	if(pos[0][2]>posmax[0][2])
	  break;
      }
      pos[0][1]=pos[0][1]+posstp[0][1];  pos[1][1]=pos[1][1]+posstp[1][1];	
      if(pos[0][1]>posmax[0][1])
	break;
    }
    pos[0][0]=pos[0][0]+posstp[0][0];  pos[1][0]=pos[1][0]+posstp[1][0];	
    if(pos[0][0]>posmax[0][0])
      break;
  }
    fout.close();
  // for(Int_t ud=0;ud<2;ud++){
  //   cout<<ud<<" : "<<posfin[ud][0]<<" "<<posfin[ud][1]<<" "<<angfin[ud][0]<<" "<<angfin[ud][1]<<" : "<<endl;
  //}
  // readxt[1]=1;readyt[1]=-1;readtht[1]=-9999;readpht[1]=-9999;
  // for(Int_t j=0;j<num[0];j++){
  //   readxt[0]=readx[0][j];
  //   readyt[0]=ready[0][j];
  //   readtht[0]=readth[0][j];
  //   readpht[0]=readph[0][j];
  //   tread->Fill();      
  // }
  // readxt[0]=1;readyt[0]=-1;readtht[0]=-9999;readpht[0]=-9999;
  // for(Int_t j=0;j<num[1];j++){
  //   readxt[1]=readx[1][j];
  //   readyt[1]=ready[1][j];
  //   readtht[1]=readth[1][j];
  //   readpht[1]=readph[1][j];
  //   tread->Fill();      
  // }
  // tread->Write();
  // tana->Write();
  // for(Int_t ud=0;ud<2;ud++){
  //   for(Int_t j=0;j<5;j++)
  //     hth2ph[ud][j]->Write();
  // }
  //  anafile->Close(); 

  /**analysis here**/  

  // TFile *anaread = new TFile(Form("%s",output.c_str()),"READ");
  // TChain *tespri = new TChain("rtree");
  // TString strroot = Form("%s",fname.c_str());  tespri->Add(strroot);   
   // if(ne%100==0){
   //   printf("Event:%10d/%10d\r",ne,neve);
   //   fflush(stdout);    
   // }
   // if(ne%10000==0){
   //   tana->Write();
   // }
 /************* make canvas ****************************/

 /**********************************************/
 

  
  return 0;
}


