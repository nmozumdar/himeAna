#include "Module.h"
using std::array;



Module::Module(){
	id = -1;
}



Module::Module(int id){
	TString moduleIDstr = Convert::toNdigit(id, 3);
	
	TString name;
	TString title;

	const int nBinsToT = 70;
	const float totRangeMin = 0.;
	const float totRangeMax = 35.;

	const int nBinsAngle = 90;
	const float angleRangeMin = 0.;
	const float angleRangeMax = 90.;

	const int nBinsEDep = 100;
	const float eDepRangeMin = 4.;
	const float eDepRangeMax = 20.;

	name = ("hAngVsToT_module_" + moduleIDstr);
	title = ("Angle of incidence (relative to ground) vs. ToT for module " + moduleIDstr);
	hAngVsToT = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsAngle, angleRangeMin, angleRangeMax);
	hAngVsToT.GetXaxis()->SetTitle("ToT (ns)");
	hAngVsToT.GetYaxis()->SetTitle("Angle of incidence (deg.)");

	name = ("hAngVsToT_yx_module_" + moduleIDstr);
	title = ("Angle of incidence (relative to ground) vs. ToT (track in y-x plane) for module " + moduleIDstr);
	hAngVsToT_yx = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsAngle, angleRangeMin, angleRangeMax);
	hAngVsToT_yx.GetXaxis()->SetTitle("ToT (ns)");
	hAngVsToT_yx.GetYaxis()->SetTitle("Angle of incidence (deg.)");

	name = ("hAngVsToT_yz_module_" + moduleIDstr);
	title = ("Angle of incidence (relative to ground) vs. ToT (track in y-z plane) for module " + moduleIDstr);
	hAngVsToT_yz = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsAngle, angleRangeMin, angleRangeMax);
	hAngVsToT_yz.GetXaxis()->SetTitle("ToT (ns)");
	hAngVsToT_yz.GetYaxis()->SetTitle("Angle of incidence (deg.)");

	name = ("hEDepVsToT_module_" + moduleIDstr);
	title = ("Energy deposition vs. ToT for module " + moduleIDstr);
	hEDepVsToT = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsEDep, eDepRangeMin, eDepRangeMax);
	hEDepVsToT.GetXaxis()->SetTitle("ToT (ns)");
	hEDepVsToT.GetYaxis()->SetTitle("Energy deposition (MeV)");

	name = ("hEDepVsToT_yx_module_" + moduleIDstr);
	title = ("Energy deposition vs. ToT (track in y-x plane) for module " + moduleIDstr);
	hEDepVsToT_yx = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsEDep, eDepRangeMin, eDepRangeMax);
	hEDepVsToT_yx.GetXaxis()->SetTitle("ToT (ns)");
	hEDepVsToT_yx.GetYaxis()->SetTitle("Energy deposition (MeV)");

	name = ("hEDepVsToT_yz_module_" + moduleIDstr);
	title = ("Energy deposition vs. ToT (track in y-z plane) for module " + moduleIDstr);
	hEDepVsToT_yz = TH2F(name, title, nBinsToT, totRangeMin, totRangeMax, nBinsEDep, eDepRangeMin, eDepRangeMax);
	hEDepVsToT_yz.GetXaxis()->SetTitle("ToT (ns)");
	hEDepVsToT_yz.GetYaxis()->SetTitle("Energy deposition (MeV)");
}



void Module::write(TFile* f) {
	f->cd();
	hAngVsToT_yx.Write();
	hAngVsToT_yz.Write();
	hAngVsToT.Add(&hAngVsToT_yx);
	hAngVsToT.Add(&hAngVsToT_yz);
	hAngVsToT.Write();
	hEDepVsToT_yz.Write();
	hEDepVsToT_yx.Write();
	hEDepVsToT.Add(&hEDepVsToT_yx);
	hEDepVsToT.Add(&hEDepVsToT_yz);
	hEDepVsToT.Write();
}
