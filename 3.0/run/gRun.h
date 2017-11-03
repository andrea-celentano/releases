#ifndef GRUN_H
#define GRUN_H 1

// geant4
#include "G4Run.hh"

// gemc
#include "gLog.h"

// mlibrary
#include "goptions.h"

// In Geant4 a run consists of a sequence of events.
// A run is represented by a G4Run class object. A run starts with BeamOn() method of G4RunManager.
// G4RunManager creates this class
class GRun : public G4Run, public GFlowMessage
{
public:
	GRun(GOptions* gopt);
	virtual ~GRun();
	virtual void RecordEvent(const G4Event*);
	virtual void Merge(const G4Run*);

private:
    G4double fEDep;

};


#endif
