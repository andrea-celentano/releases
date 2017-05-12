// gemc
#include "GDetectorConstruction.h"
#include "GPrimaryGeneratorAction.h"

// geant4
#include "GActionInitialization.h"
#include "GRunAction.h"

// to be removed
#include "QGS_BIC.hh"


// c++
#include <iostream>
using namespace std;

GActionInitialization::GActionInitialization()
{;}

GActionInitialization::~GActionInitialization()
{;}

void GActionInitialization::Build() const
{
	SetUserAction(new GRunAction);
	SetUserAction(new GPrimaryGeneratorAction);
}

void GActionInitialization::BuildForMaster() const
{
//	G4UserRunAction* run_action = new GRunAction;
//	SetUserAction(run_action);
}

// instantiate run manager and assign number of cores
G4MTRunManager* gRunManager(int nthreads)
{
	int useThreads = nthreads;
	int allThreads = G4Threading::G4GetNumberOfCores();

	if(useThreads == 0) useThreads = allThreads;

	G4MTRunManager *runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(useThreads);

	G4cout << " > gRunManager: using " << useThreads << " threads out of "  << allThreads << " available." << G4endl;

	// GEMC Action
	runManager->SetUserInitialization(new GActionInitialization);

	runManager->SetUserInitialization(new GDetectorConstruction());
	runManager->SetUserInitialization(new QGS_BIC());


	//Initialize G4 kernel
	runManager->Initialize();

	
	return runManager;
}
