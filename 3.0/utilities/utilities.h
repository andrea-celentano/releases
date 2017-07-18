#ifndef GUTILITIES_H
#define GUTILITIES_H 1

// general utility functions needed by gemc

// mlibrary
#include "goptions.h"
#include "gruns.h"

// qt
#include <QApplication>

// c++
#include <map>
using namespace std;

// this gemc version
#define GEMC_VERSION "3.0.beta"

// loads the option map
map<string, GOption> defineOptions();

// distinguishing between graphical and batch mode
QCoreApplication* createQtApplication(int &argc, char *argv[], bool gui);

// loading a qt resource
int loadQResource(char* argv[], string resourceName);


// retrieve and define batch commands
// these include possible options/gcard commands
vector<string> batchCommands(GOptions* gopt);

// retrieve and define gui commands
vector<string> interactiveCommands(GOptions* gopt);

// apply initial UIM commands coming from:
// - goptions
// - batch
// - gui (if needed)
void applyInitialUIManagerCommands(GOptions* gopt);

// instantiate gruns and beamOn requested n. events for each run
void gBeamOn(GOptions *gopts);

#endif
