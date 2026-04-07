void writecsv()
{

	ofstream outfile("2026-03-05.csv");
	int j = 1;
	double x = -471.5, y = -471.5, z = 0.;
	bool h = false;
	for(int i = 0 ; i < 288; i++)
	{
		double ang;
		int pad = (i)%24;
		if(j%2)
		{
			h=true;
			y = 0.;
			x = -471.5+(pad)*41.;
			z = 0.+(j-1)*21.;
			ang = 0;
		}
		else
		{
			h=true;
			y = -471.5+(pad)*41.;
			x = 0.;
			z = 0.+(j-1)*21.;
			ang = 1;
		}

		outfile<<i<<";"<<x<<";"<<y<<";"<<z<<";"<<ang<<endl;
		if((pad+1) == 24)
		{
			j++;
		}
	}
	outfile.close();
}
