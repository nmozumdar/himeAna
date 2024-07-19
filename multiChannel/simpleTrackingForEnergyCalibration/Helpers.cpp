#include "Helpers.h"
#include "Constants.h"



int Helpers::getLayerID(int moduleID){ 
	// moduleID runs over all modules of the detector wall,
	// moduleNumberInThisLayer only over all modules of a single layer.
	int moduleNumberInThisLayer = moduleID % Constants::nModulesPerLayer; 
	// the difference of moduleID and moduleNumberInThisLayer
	// can always be divided by the number of modules per layer without remainder
	return (moduleID - moduleNumberInThisLayer) / Constants::nModulesPerLayer;
}
