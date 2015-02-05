#include "ett.hpp"

void xtics(fstream& plik)
{
	int time_stop = time(NULL);
	int time_start = time_stop-24*3600;
	plik<<"set xrange ["<<time_start<<":"<<time_stop<<"]"<<endl;
	plik<<"set xtics (";
	for(int i=time_start; i<=time_stop; i+=7200)
	{
		struct tm * timeinfo;
		time_t tmp = i;
		timeinfo = localtime(&tmp);
		if(i!=time_start)
			plik<<", ";
		plik<<"\""<<timeinfo->tm_hour<<":"<<timeinfo->tm_min<<"\" "<<i;
	}
	plik<<")"<<endl;
}

int main(int argc, const char **argv) 
{
	root = argv[1];
	vector<pair<int, int> > lista_plikow = pliki();
	cout<<"wygenerowano "<<lista_plikow.size()<<endl;
	map <int, wpis> wpisy;
	for(int i=0; i<lista_plikow.size(); i++)
	{
		getpage_to_plot(time(NULL), lista_plikow[i].first, lista_plikow[i].second, wpisy);
	}
	vector <wpis> tst=transfer(wpisy);
	sort(tst.begin(), tst.end(), srt);
	vector <vector <long long> > daty3;
	string pol1=root+"chart_data.ddd";
	string pol2=root+"chart_data.ustawienia";
	fstream chart_data((pol1).c_str(), ios::trunc | ios::out);
	fstream chart_ust((pol2).c_str(), ios::trunc | ios::out);
	chart_ust<<"set terminal png size 1000,600"<<endl;
	chart_ust<<"set output \""<<root<<"galk.png\""<<endl;
	xtics(chart_ust);
	chart_ust<<"plot";
	for(int i=0; i<15; i++)
	{
		vector <long long> daty2;
		daty2.push_back(tst[i].data);
		for(int j=0; j<tst[i].plusy.size(); j++)
		{
			daty2.push_back(tst[i].plusy[j].data);
		}
		chart_ust<<" \""<<root<<"chart_data.ddd\" using "<<i*2+2<<":"<<i*2+1;
		chart_ust<<" title \'"<<tst[i].nick<<"\' with lines";
		if(i<14) chart_ust<<", \\";
		chart_ust<<endl;
		sort(daty2.begin(), daty2.end());
		daty3.push_back(daty2);
	}
	//chart_data<<endl;
	int s1=0;
	for(int i=0; i<daty3.size(); i++)
	{
		int rrr=daty3[i].size();
		s1=max(s1, rrr);
	}
	for(int i=0; i<s1; i++)
	{
		for(int j=0; j<daty3.size(); j++)
		{
			if(daty3[j].size()>i)
			{
				chart_data<<i<<"	"<<daty3[j][i]<<"	";
			}
		}
		chart_data<<endl;
	}
	string pol="gnuplot "+root+"chart_data.ustawienia";
	system(pol.c_str());
	return 0;
}
