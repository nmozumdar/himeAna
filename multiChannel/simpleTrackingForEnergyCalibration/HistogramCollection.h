#ifndef HistogramCollection_h
#define HistogramCollection_h

#include "Constants.h"
#include "TH1F.h"
#include "TFile.h"
#include <array>

class HistogramCollection{

	public:
	
	HistogramCollection();

	// write all histograms to a ROOT file
	void write(TFile* f);
	// fill histograms before applying any cuts
	void fillBeforeCuts(int layerID, int multiplicity);
	// fill the histograms showing the deviations of hits to the muon track after the first iteration
	void fillDevsFirstIteration(int layerID, std::vector<float> devs);
	// fill histograms only for the data from the successfully reconstructed muon tracks
	void fillAfterCuts(int layerID, int multiplicity, std::vector<float> devs, bool horizontal, float angle);

	private:
	// Angular distribution of cosmic muons for each layer separately.
	// First histogram is for verical layers,
	// second one for horizontal ones.
	std::array<TH1F, 2> hArrAngularDistribution;
	// angular distribution for all layers together
	TH1F hAngularDistribution;
	// number of hits per layer before applying any cuts
	std::array<TH1F, Constants::nLayers> hArrTotalMultiplicity;
	// number of hits per layer that were used for a successfully reconstructed muon track
	std::array<TH1F, Constants::nLayers> hArrMultiplicityAfterCuts;
	// deviation of all hits to the reconstructed muon track after the first iteration before applying any cuts
	std::array<TH1F, Constants::nLayers> hArrDev;
	// deviation of all hits to the reconstructed muon track
	std::array<TH1F, Constants::nLayers> hArrDevAfterCuts;
};

#endif