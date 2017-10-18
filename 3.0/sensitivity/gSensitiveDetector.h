#ifndef  GSENSITIVEDETECTOR_H
#define  GSENSITIVEDETECTOR_H 1

// gemc
#include "gLog.h"

// geant4
#include "G4VSensitiveDetector.hh"

// mlibrary
#include "goptions.h"
#include "gvolume.h"
#include "gtouchable.h"
#include "gdynamic.h"

// c++
#include <vector>
using namespace std;


class GSensitiveDetector : public G4VSensitiveDetector, public GFlowMessage
{
public:
	GSensitiveDetector(string name, GOptions* gopt, map<string, GDynamic*> *gDigiGlobal);

	// geant4 methods
	virtual void Initialize(G4HCofThisEvent* g4hc);                            ///< Beginning of sensitive Hit
	virtual G4bool ProcessHits(G4Step* thisStep, G4TouchableHistory* g4th);    ///< Process Step
	virtual void EndOfEvent(G4HCofThisEvent* g4hc);                            ///< End of sensitive Hit

private:
	int verbosity;

	// map of touchable associated with each volume registered with the sensitive detector
	// it is stored here so it can be retrieved

	// a touchable has been hit if it's present in the set
	// the set is reset each event
	set<GTouchable*> touchableSet;

	// the GSensitiveDetector is built before the digitization, so we need
	// a pointer to global digitization map so we can pick gDigiLocal at initialization
	// it will be loaded later with the plugins
	map<string, GDynamic*> *gDigitizationGlobal;
	// gDigiLocal is thread local, picked form gDigitizationGlobal
	GDynamic *gDigiLocal;

	// this map is used to retrieve the touchable set for each volume during processHit.
	map<string, GTouchable*> *gTouchableMap;
	
private:
	// skip ProcessHit
	// decides if the hit should be processed or not
	bool skipProcessHit(double energy);
	bool loadDigitizationPlugin();
	
	// retrieve touchable in ProcessHit
	GTouchable* getGTouchable(G4VTouchable *geant4Touchable);
	
public:
	// GSensitiveDetector options - defined in utilities.cc
	static map<string, GOption> defineOptions();

};

#endif
