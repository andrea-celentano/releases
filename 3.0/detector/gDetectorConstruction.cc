// gemc
#include "gDetectorConstruction.h"

// mlibrary
#include "g4volume.h"

// geant4
#include "G4SDManager.hh"


GDetectorConstruction::GDetectorConstruction(GOptions* opt, map<string, GDynamic*> *gDigiGlobal) :
G4VUserDetectorConstruction(), GFlowMessage(opt, "GDetectorConstruction"), gopt(opt), gDigitizationGlobal(gDigiGlobal)
{
	// making this explicit in case of access before Construct()
	// (should never happen anyway)
	gsetup  = nullptr;
	g4setup = nullptr;
}

GDetectorConstruction::~GDetectorConstruction() {}

G4VPhysicalVolume* GDetectorConstruction::Construct()
{
	flowMessage("Constructing gemc world");

	// loading gvolumes, material, system parameters
	gsetup = new GSetup(gopt);

	// builiding geant4 volumes
	g4setup = new G4Setup(gsetup, gopt);

	return g4setup->getPhysical(WORLDNAME);
}

void GDetectorConstruction::ConstructSDandField()
{
	// no need to do anything if we're in the main thread
	if (G4Threading::IsMasterThread() ) return;

	int verbosity = gopt->getInt("gsensitivityv");

	flowMessage("Inside SDandField");

	// used to check if a SD if it already exists
	map<string, GSensitiveDetector*> allSensitiveDetectors;

	// building the sensitive detectors
	// this is thread local
	for(auto &s : gsetup->getSetup()) {
		for(auto &gv : s.second->getSytems()) {
			string sensitivity = gv.second->getSensitivity();
			
			// making sure the logical volume exists
			if(g4setup->getLogical(gv.first) == nullptr) {
				G4cerr << FATALERRORL << "  Error: " << gv.first << " logical volume not build? This should never happen." << G4endl;
				exit(99);
			} else if(sensitivity != NOTAPPLICABLE) {
				// checking that we do not already have a GSensitiveDetector
				if(allSensitiveDetectors.find(sensitivity) == allSensitiveDetectors.end()) {
					
					if(verbosity == GVERBOSITY_ALL) {
						G4cout  << " Sensitive detector " << sensitivity << " doesn't exist for " << gv.first << ". Creating it." << G4endl;
					}
					
					allSensitiveDetectors[sensitivity] = new GSensitiveDetector(sensitivity, gopt, gDigitizationGlobal);
					auto sdManager = G4SDManager::GetSDMpointer();
					sdManager->AddNewDetector(allSensitiveDetectors[sensitivity]);

				} else {
					if(verbosity == GVERBOSITY_ALL) {
						G4cout <<  " Sensitive detector " << sensitivity << " exists for " << gv.first << G4endl;
					}
				}
				SetSensitiveDetector(gv.first, allSensitiveDetectors[sensitivity]);
			}
		}
	}
}







