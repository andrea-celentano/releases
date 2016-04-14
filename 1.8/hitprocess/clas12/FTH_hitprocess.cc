// %%%%%%%%%%
// G4 headers
// %%%%%%%%%%
#include "G4UnitsTable.hh"
#include "G4Poisson.hh"
#include "Randomize.hh"

// %%%%%%%%%%%%%
// gemc headers
// %%%%%%%%%%%%%
#include "FTH_hitprocess.h"

PH_output FTH_HitProcess :: ProcessHit(MHit* aHit, gemc_opts)
{
 PH_output out;
 out.identity = aHit->GetId();
 HCname = "FTH Hit Process";

 // %%%%%%%%%%%%%%%%%%%
 // Raw hit information
 // %%%%%%%%%%%%%%%%%%%
 int nsteps = aHit->GetPos().size();

 // Get Total Energy deposited
 double Etot = 0;
 vector<G4double> Edep = aHit->GetEdep();
 for(int s=0; s<nsteps; s++) Etot = Etot + Edep[s];

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

 // use Crystal ID to define IDX and IDY
 int ID    = out.identity[0].id;
 int Layer = out.identity[1].id;

 // initialize ADC and TDC
 int ADC = 0;
 int TDC = 8191; 

 out.dgtz.push_back(ID);
 out.dgtz.push_back(Layer);
 out.dgtz.push_back(ADC);
 out.dgtz.push_back(TDC);


 return out;
}

vector<identifier>  FTH_HitProcess :: ProcessID(vector<identifier> id, G4Step* aStep, detector Detector, gemc_opts Opt)
{
	id[id.size()-1].id_sharing = 1;
 return id;
}











