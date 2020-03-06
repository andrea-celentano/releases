// gemc utilities
#include "utilities.h"

// mlibrary options constructors
#include "gsplash.h"
#include "gsystem.h"
#include "g4display.h"
#include "g4volume.h"
#include "gSensitiveDetector.h"
#include "gruns.h"
#include "gmedia.h"

// define all gemc options
map<string, GOption> defineOptions()
{
	map<string, GOption> optionsMap;

	// GUI options
	optionsMap["geometry"] = GOption("Window Geometry", "1400x1200", "gui");


	optionsMap["nthreads"] = GOption("Number of threads to use", 0, "control");
	optionsMap["nthreads"].addHelp("0: use all available threads (default)\n");

	optionsMap["g4command"] = GOption("Execute G4 command.", "no", "geant4", "true");
	optionsMap["g4command"].addHelp("Example:\n");
	optionsMap["g4command"].addHelp(string(GOPTIONITEM) + "/vis/scene/add/axes 0 0 0 20 cm\n");
	optionsMap["g4command"].addHelp("This option can be repeated.\n");

	optionsMap["gemcv"] = GOption("Gemc general Verbosity", 0, "verbosity");
	optionsMap["gemcv"].addHelp("Possible values:\n");
	optionsMap["gemcv"].addHelp(GVERBOSITY_SILENT_D);
	optionsMap["gemcv"].addHelp(GVERBOSITY_SUMMARY_D);
	optionsMap["gemcv"].addHelp(GVERBOSITY_DETAILS_D);
	optionsMap["gemcv"].addHelp(GVERBOSITY_ALL_D);

	optionsMap["threadLog"] = GOption("Display messages from this thread only", 0, "verbosity");
	optionsMap["threadLog"].addHelp("Default is thread id 0. Possible values:\n");
	optionsMap["threadLog"].addHelp(string(GOPTIONITEM) + "#n thread id\n");
	optionsMap["threadLog"].addHelp(string(GOPTIONITEM) + "-1 dsplay all\n");

	optionsMap["softwareVersion"] = GOption("softwareVersion", GEMC_VERSION, "version");

	// mlibrary GSplash default option
	optionsMap += GSplash::defineOptions();
	optionsMap += GSetup::defineOptions();
	optionsMap += G4Setup::defineOptions();
	optionsMap += G4Display::defineOptions();
	optionsMap += GSensitiveDetector::defineOptions();
	optionsMap += GRuns::defineOptions();
	optionsMap += GFlowMessage::defineOptions();
	optionsMap += GMedia::defineOptions();

	return optionsMap;
}

