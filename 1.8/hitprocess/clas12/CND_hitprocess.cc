// %%%%%%%%%%
// G4 headers
// %%%%%%%%%%
#include "G4UnitsTable.hh"
#include "G4Poisson.hh"
#include "Randomize.hh"


// %%%%%%%%%%%%%
// gemc headers
// %%%%%%%%%%%%%
#include "CND_hitprocess.h"


PH_output CND_HitProcess :: ProcessHit(MHit* aHit, gemc_opts Opt)
{
	string hd_msg        = Opt.args["LOG_MSG"].args + " CND Hit Process " ;
	double HIT_VERBOSITY = Opt.args["HIT_VERBOSITY"].arg;
	PH_output out;
	out.identity = aHit->GetId();
	HCname = "CND Hit Process";
	
	// %%%%%%%%%%%%%%%%%%%
	// Raw hit information
	// %%%%%%%%%%%%%%%%%%%
	int nsteps = aHit->GetPos().size();
	
	// Get Total Energy deposited
	double Etot = 0;
	vector<G4double> Edep = aHit->GetEdep();
	for(int s=0; s<nsteps; s++) Etot = Etot + Edep[s];
	
	// Charge for each step
	vector<int> charge = aHit->GetCharges();

	// average global, local positions of the hit
	double x, y, z;
	double lx, ly, lz;
	x = y = z = lx = ly = lz = 0;
	vector<G4ThreeVector> pos  = aHit->GetPos();
	vector<G4ThreeVector> Lpos = aHit->GetLPos();
	
	if(Etot>0)
		for(int s=0; s<nsteps; s++)
		{
			x  = x  +  pos[s].x()*Edep[s]/Etot;
			y  = y  +  pos[s].y()*Edep[s]/Etot;
			z  = z  +  pos[s].z()*Edep[s]/Etot;
			lx = lx + Lpos[s].x()*Edep[s]/Etot;
			ly = ly + Lpos[s].y()*Edep[s]/Etot;
			lz = lz + Lpos[s].z()*Edep[s]/Etot;
		}
		else
		{
			x  = pos[0].x();
			y  = pos[0].y();
			z  = pos[0].z();
			lx = Lpos[0].x();
			ly = Lpos[0].y();
			lz = Lpos[0].z();
		}
		
	// average time
	double time = 0;
	vector<G4double> times = aHit->GetTime();
	for(int s=0; s<nsteps; s++) time = time + times[s]/nsteps;
	
	//        cout << "\t Time: " <<  time << endl;


	// Energy of the track
	double Ene = aHit->GetE();
		
	out.raws.push_back(Etot);
	out.raws.push_back(x);
	out.raws.push_back(y);
	out.raws.push_back(z);
	out.raws.push_back(lx);
	out.raws.push_back(ly);
	out.raws.push_back(lz);
	out.raws.push_back(time);
	out.raws.push_back((double) aHit->GetPID());
	out.raws.push_back(aHit->GetVert().getX());
	out.raws.push_back(aHit->GetVert().getY());
	out.raws.push_back(aHit->GetVert().getZ());
	out.raws.push_back(Ene);
	out.raws.push_back((double) aHit->GetmPID());
	out.raws.push_back(aHit->GetmVert().getX());
	out.raws.push_back(aHit->GetmVert().getY());
	out.raws.push_back(aHit->GetmVert().getZ());
	
	
	// %%%%%%%%%%%%
	// Digitization
	// %%%%%%%%%%%%
	int layer  = out.identity[0].id;
	int paddle = out.identity[1].id;
	

        // Digitization Parameters
        double light_yield=10000/MeV;             // number of optical photons pruced in the scintillator per MeV of deposited energy
        double att_length=3*m;                    // light attenuation length
        double sensor_surface=pow(2.5*cm,2)*pi;   // area of photo sensor
        double light_coll=sensor_surface/         // ratio of photo_sensor area over paddle section ~ light collection efficiency
	                            ((aHit->GetDetector().dimensions[0]+
	                              aHit->GetDetector().dimensions[1])*
	                            2*aHit->GetDetector().dimensions[4]);
        double sensor_qe=0.2;                     // photo sensor quantum efficiency
        double sensor_gain=0.08;                  // gain of the photo sensor in pC/(#p.e.); it defines the conversion from photoelectrons to charge:
                                                  // for a pmt gain of 10^6, this factor is equal to 10^6*1.6*10^-19 C = 0.16 pC/(#p.e.)
                                                  // assuming the signals are split into two going to QDC and scriminators it becomes 0.08 pC/(#p.e.)
        double adc_conv=10;                       // conversion factor from pC to ADC (typical sensitivy of CAEN VME QDC is of 0.1 pC/ch)
        double adc_ped=0;                         // ADC Pedestal
        double veff=16*cm/ns;                     // light velocity in scintillator
	double u_veff=16*cm/ns;                   // light velocity in uturn connector material
	double sigmaT=0.16*ns/sqrt(MeV);          // time smearing factor (estimated from tests at Orsay)
	//        double sigmaT=0.2*ns;                     // time smearing factor
        double tdc_conv=1/0.050/ns;               // TDC conversion factor
	double u_length = 8*cm;                   // Path length through the u-turn connector between paddles
	double bend_loss = 0.5;                   // Factor of energy loss in the u-turn bit of the scintillator


        // initialize ADC and TDC
        double etotL = 0;
        double etotR = 0;
        double timeL = 0;
        double timeR = 0;
        int ADCL = 0;
        int ADCR = 0;
        int TDCL = 4096;
        int TDCR = 4096;

	double etotB = 0; // signal propagating to backward end of paddle hit happened in
	double etotF = 0; // signal propagating to forward end of the paddle the hit happened in, round u-turn and along neighbouring paddle
	double timeB = 0;
	double timeF = 0; 
	int ADCB = 0;
	int ADCF = 0;
	int TDCB = 4096; 
	int TDCF = 4096; 
	
	// cout << "\n Detector dimensions: " << endl;
       	// for (int d = 0; d < 5; d++){
	//   cout << " Dimension " << d << ": " << aHit->GetDetector().dimensions[d] << endl;
	// }
	
	//	cout << "\t Paddle positions: " << aHit->GetDetector().pos[0] << ", " << aHit->GetDetector().pos[1] << ", " << aHit->GetDetector().pos[2] << endl;

        // Get the paddle length: in CND paddles are along y
        double length = aHit->GetDetector().dimensions[2];
	
        // Get info about detector material to eveluate Birks effect
	
        vector<int> pids = aHit->GetPIDs(); 
	
        double birks_constant=aHit->GetDetector().GetLogical()->GetMaterial()->GetIonisation()->GetBirksConstant();
	
	//	cout << " Birks constant is: " << birks_constant << endl;
	//	cout << aHit->GetDetector().GetLogical()->GetMaterial()->GetName() << endl;
	
	
	int dum[4] = {0,0,0,0};
	
        if(Etot>0)
	{
	  
	  //	  cout << "-------------------------------------------------------------------" << endl;
	  
	  for(int s=0; s<nsteps; s++)
	    { 
	      // Distances from left, right
	      double dLeft  = length + Lpos[s].y();
	      double dRight = length - Lpos[s].y();
	      
	      // Distances from forward, back PMT
	      double dBack  = length + Lpos[s].y();
	      double dFwd = 3*length - Lpos[s].y();
	      
	      // cout << "\n Distances: " << endl;
	      //cout << "\t dLeft, dRight, dBack, dFwd: " << dLeft << ", " << dRight << ", " << dBack << ", " << dFwd << endl; 
	      
	      // apply Birks effect
	      double stepl = 0.;	     
	      
	      if (s == 0){
		stepl = sqrt(pow((Lpos[s+1].x() - Lpos[s].x()),2) + pow((Lpos[s+1].y() - Lpos[s].y()),2) + pow((Lpos[s+1].z() - Lpos[s].z()),2));
	      }
	      else {
		stepl = sqrt(pow((Lpos[s].x() - Lpos[s-1].x()),2) + pow((Lpos[s].y() - Lpos[s-1].y()),2) + pow((Lpos[s].z() - Lpos[s-1].z()),2)); 
	      }
	      
	      double Edep_B = BirksAttenuation(Edep[s],stepl,charge[s],birks_constant);
	      
	      // cout << "\t Birks Effect: " << " Edep=" << Edep[s] << " StepL=" << stepl
	      //	  << " PID =" << pids[s] << " charge =" << charge[s] << " Edep_B=" << Edep_B << endl;
	      
	      if (light_coll > 1) light_coll = 1.;     // To make sure you don't miraculously get more energy than you started with
	      
	      etotL = etotL + Edep_B/2 * exp(-dLeft/att_length) * light_coll;
	      etotR = etotR + Edep_B/2 * exp(-dRight/att_length) * light_coll;
	      
	      etotB = etotB + Edep_B/2 * exp(-dBack/att_length) * light_coll; 
	      etotF = etotF + Edep_B/2 * exp(-dFwd/att_length) * bend_loss * light_coll; 
	      
	      //  cout << "step: " << s << " etotL, etotR, etotB, etotF: " << etotL << ", " << etotR << ", " << etotB << ", " << etotF << endl; 
	      
	      // timeL= timeL + (times[s] + dLeft/veff) / nsteps;
	      // timeR= timeR + (times[s] + dRight/veff) / nsteps;
	      
	      if ((dum[0] == 0) && (etotL >= 0.)){
		timeL = times[s] + dLeft/veff; 
		dum[0] = 1;
	      }
	      if ((dum[1] == 0) && (etotR >= 0.)){
		timeR = times[s] + dRight/veff; 
		dum[1] = 1;
	      }
	      
	      if ((dum[2] == 0) && (etotB >= 0.)){
		timeB = times[s] + dBack/veff;
		dum[2] = 1;
	      }
	      if ((dum[3] == 0) && (etotF >= 0.)){
		timeF = times[s] + dFwd/veff + u_length/u_veff;
		dum[3] = 1;
	      }
	      
	      // cout << "\t timeL, timeR, timeB, timeF: " << timeL << ", " << timeR << ", " << timeB << ", " << timeF << endl;
	      
	      //	     cout << "\n Edep: "  << Edep[s]/MeV << " time: " << times[s]/ns << " Lpos-x: " 
	      //	  << Lpos[s].x()/cm << " Lpos-y: " << Lpos[s].y()/cm << " Lpos - z: " << Lpos[s].z()/cm << " pos-x: "
	      //	  << pos[s].x()/cm << " pos-y: "  << pos[s].y()/cm  << " pos-z: " << pos[s].z()/cm << " etotL: "
	      //	  << etotL/MeV << " etotR: " << etotR/MeV  << " timeL: " << timeL/ns << " timeR: " << timeR/ns << endl;
	      
	    }
	  
	  //  cout << "***********************" << endl;	   
	  // cout << "etotL, etotR, etotB, etotF: " << etotL << ", " << etotR << ", " << etotB << ", " << etotF << endl;  
	  // cout << "timeL, timeR, timeB, timeF: " << timeL << ", " << timeR << ", " << timeB << ", " << timeF << endl;
	  
	  
	  TDCL=(int) ((timeL+G4RandGauss::shoot(0.,sigmaT/sqrt(etotL))) * tdc_conv);
	  TDCR=(int) ((timeR+G4RandGauss::shoot(0.,sigmaT/sqrt(etotR))) * tdc_conv);
	  if(TDCL<0) TDCL=0;
	  if(TDCR<0) TDCR=0;
	  
	  ADCL=(int) (G4Poisson(etotL*light_yield*sensor_qe)*sensor_gain*adc_conv + adc_ped);
	  ADCR=(int) (G4Poisson(etotR*light_yield*sensor_qe)*sensor_gain*adc_conv + adc_ped);
	  
	  TDCB=(int) ((timeB + G4RandGauss::shoot(0.,sigmaT/sqrt(etotB))) * tdc_conv);
	  TDCF=(int) ((timeF + G4RandGauss::shoot(0.,sigmaT/sqrt(etotF))) * tdc_conv);
	  if(TDCB<0) TDCB=0;
	  if(TDCF<0) TDCF=0;
	  
	  // cout << "time right: " << TDCR / tdc_conv << " time left: " << TDCL/tdc_conv << endl;
	  // cout << "time forward: " << TDCF/tdc_conv << " time backwards: " << TDCB/tdc_conv << endl;
	  
	  ADCB=(int) (G4Poisson(etotB*light_yield*sensor_qe)*sensor_gain*adc_conv + adc_ped);
	  ADCF=(int) (G4Poisson(etotF*light_yield*sensor_qe)*sensor_gain*adc_conv + adc_ped);
	  
	  //cout << "energy right: " << ADCR / (adc_conv*sensor_gain*sensor_qe*light_yield) << " E left: " << ADCL / (adc_conv*sensor_gain*sensor_qe*light_yield) << endl; 
	  //cout << "energy forw: " << ADCF / (adc_conv*sensor_gain*sensor_qe*light_yield) << " E back: " << ADCB / (adc_conv*sensor_gain*sensor_qe*light_yield) << endl;
	  
	  //cout << " Light collection: " << light_coll << endl;
	  
        }  // closes (Etot > 0) loop
	
	// cout << "___________x________x__________x_____________x_____________" << endl;
	
        if(HIT_VERBOSITY>4) {
	  
	  cout << "WOOF woof WOOF woof WOOF woof WOOF woof WOOF!!!" << endl;
	  
	  cout <<  hd_msg << " layer: " << layer    << ", paddle: " << paddle  << " x=" << x/cm << " y=" << y/cm << " z=" << z/cm << endl;
	  cout <<  hd_msg << " Etot=" << Etot/MeV << " time=" << time/ns << endl;
	  cout <<  hd_msg << " TDCL=" << TDCL     << " TDCR=" << TDCR    << " ADCL=" << ADCL << " ADCR=" << ADCR << endl;
	  cout <<  hd_msg << " TDCB=" << TDCB     << " TDCF=" << TDCF    << " ADCB=" << ADCB << " ADCF=" << ADCF << endl;
        }

        out.dgtz.push_back(layer);
        out.dgtz.push_back(paddle);
        out.dgtz.push_back(ADCL);
        out.dgtz.push_back(ADCR);
        out.dgtz.push_back(TDCL);
        out.dgtz.push_back(TDCR);
	out.dgtz.push_back(ADCB);
	out.dgtz.push_back(ADCF);
	out.dgtz.push_back(TDCB);
	out.dgtz.push_back(TDCF);
		
	return out;
}


vector<identifier>  CND_HitProcess :: ProcessID(vector<identifier> id, G4Step *step, detector Detector, gemc_opts Opt)
{
	id[id.size()-1].id_sharing = 1;
	return id;
}



double CND_HitProcess::BirksAttenuation(double destep,double stepl,int charge,double birks)
{
 //Example of Birk attenuation law in organic scintillators.
 //adapted from Geant3 PHYS337. See MIN 80 (1970) 239-244
 //
 // Taken from GEANT4 examples advanced/amsEcal and extended/electromagnetic/TestEm3
 //
 double response = destep;
 if (birks*destep*stepl*charge != 0.)
   {
     response = destep/(1. + birks*destep/stepl);
   }
 return response;
}



