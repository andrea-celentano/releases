// gemc
#include "gRunAction.h"
#include "gRun.h"

// geant4
#include "globals.hh"
#include "G4Threading.hh"
#include "G4MTRunManager.hh"


// c++
using namespace std;

// Constructor for workers
GRunAction::GRunAction(GOptions* opt) : G4UserRunAction(), GFlowMessage(opt, "GRunAction"), gopt(opt)
{
}


// Destructor
GRunAction::~GRunAction()
{
}

// this is not local?
G4Run* GRunAction::GenerateRun()
{
	flowMessage("GenerateRun");

	return new GRun(gopt);
}

// executed after BeamOn
void GRunAction::BeginOfRunAction(const G4Run* aRun)
{

	if(IsMaster()) {
		flowMessage("BeginOfRunAction Master for run id " + to_string(aRun->GetRunID()) + " in g4thread " + to_string(G4Threading::G4GetThreadId()) );
	} else {
		flowMessage("BeginOfRunAction Local for run id " + to_string(aRun->GetRunID()) + " in g4thread " + to_string(G4Threading::G4GetThreadId()) );
	}
	
	
}

void GRunAction::EndOfRunAction(const G4Run* aRun)
{
	const GRun* theRun = static_cast<const GRun*>(aRun);
	if(IsMaster()) {
		flowMessage("EndOfRunAction Master for run id" + to_string(aRun->GetRunID()) + " in g4thread " + to_string(G4Threading::G4GetThreadId()) );
		flowMessage("Total number of events this run: " + to_string(theRun->GetNumberOfEvent()));

	} else {
		flowMessage("EndOfRunAction for run id" + to_string(aRun->GetRunID()) + " in g4thread " + to_string(G4Threading::G4GetThreadId()) );
		flowMessage("Total number of events this thread: " + to_string(theRun->GetNumberOfEvent()));

	}
}
