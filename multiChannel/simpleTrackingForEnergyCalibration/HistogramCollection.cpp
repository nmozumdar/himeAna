#include "HistogramCollection.h"
#include "Convert.h"
using std::vector;



HistogramCollection::HistogramCollection(){
	
	// initialize all histograms: set names, titles, axes ranges, ...

	const int nBins_angularDistribution = 90;
	const float rangeMin_angularDistribution = 0.;
	const float rangeMax_angularDistribution = 90.;
	const int nBins_multiplicity = 25;
	const int nBins_dev = 1000;
	const float rangeMin_dev = 0.;
	const float rangeMax_dev = 1000.;

	for(int i = 0; i < 2; i++){

		TString name("hAngularDistribution");
		TString title("Angular distribution of cosmic muons for ");
		if(i){
			name += "Horizontal";
			title += "horizontal layers";
		}
		else{
			name += "Vertical";
			title += "vertical layers";
		}
		hArrAngularDistribution[i] = TH1F(name, title, nBins_angularDistribution, rangeMin_angularDistribution, rangeMax_angularDistribution);
		hArrAngularDistribution[i].GetXaxis()->SetTitle("Angle (degree)");
	}

	hAngularDistribution = TH1F("hAngularDistribution", "Angular distribution of cosmic muons", nBins_angularDistribution, rangeMin_angularDistribution, rangeMax_angularDistribution);
	hAngularDistribution.GetXaxis()->SetTitle("Angle (degree)");


	for(int layer = 0; layer < Constants::nLayers; layer++){

		TString layerStr(Convert::toNdigit(layer, 2));

		TString name("hMultiplicity_layer" + layerStr);
		TString title("Number of hits in layer " + layerStr);
		hArrTotalMultiplicity[layer] = TH1F(name, title, nBins_multiplicity, 0, nBins_multiplicity);
		hArrTotalMultiplicity[layer].GetXaxis()->SetTitle("Multiplicity");

		name = TString("hMultiplicityAfterCuts_layer" + layerStr);
		title = TString("Number of hits in layer " + layerStr + " after all cuts");
		hArrMultiplicityAfterCuts[layer] = TH1F(name, title, nBins_multiplicity, 0, nBins_multiplicity);
		hArrMultiplicityAfterCuts[layer].GetXaxis()->SetTitle("Multiplicity");

		name = TString("hDev_layer" + layerStr);
		title = TString("Distances of all hits to the muon track x direction after the first iteration (Layer " + layerStr + ")");
		hArrDev[layer] = TH1F(name, title, nBins_dev, rangeMin_dev, rangeMax_dev);
		hArrDev[layer].GetXaxis()->SetTitle("\\Delta x \\;\\mbox{(mm)}");

		name = TString("hDevAfterCuts_layer" + layerStr);
		title = TString("Distances of all hits to the final reconstructed muon track in x direction (Layer " + layerStr + ")");
		hArrDevAfterCuts[layer] = TH1F(name, title, nBins_dev, rangeMin_dev, rangeMax_dev);
		hArrDevAfterCuts[layer].GetXaxis()->SetTitle("\\Delta x \\;\\mbox{(mm)}");
	}
}



void HistogramCollection::fillBeforeCuts(int layerID, int multiplicity){
	hArrTotalMultiplicity[layerID].Fill(multiplicity);
}



void HistogramCollection::fillDevsFirstIteration(int layerID, vector<float> devs){
	for(float deviation: devs){
		hArrDev[layerID].Fill(deviation);
	}
}



void HistogramCollection::fillAfterCuts(int layerID, int multiplicity, vector<float> devs, bool horizontal, float angle){
	hArrMultiplicityAfterCuts[layerID].Fill(multiplicity);
	for(float deviation: devs){
		hArrDevAfterCuts[layerID].Fill(deviation);
	}
	hArrAngularDistribution[horizontal].Fill(angle);
}



void HistogramCollection::write(TFile* f){

	// hAngularDistribution is just the sum of the histograms in
	// hArrAngularDistribution
	for(const TH1F& h: hArrAngularDistribution) hAngularDistribution.Add(&h);

	f->cd();
	for(TH1F h: hArrTotalMultiplicity) h.Write();
	for(TH1F h: hArrMultiplicityAfterCuts) h.Write();
	for(TH1F h: hArrDev) h.Write();
	for(TH1F h: hArrDevAfterCuts) h.Write();
	for(TH1F h: hArrAngularDistribution) h.Write();
	hAngularDistribution.Write();
}