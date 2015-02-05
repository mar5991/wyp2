#include "ett.hpp"
int main(int argc, const char **argv) 
{
	string code = argv[4];
	string secret = argv[5];
	root = argv[3];
	int tim = time(NULL);
	stringstream trr1;
	trr1<<argv[1];
	int time1;
	trr1>>time1;
	stringstream trr2;
	trr2<<argv[2];
	int time2;
	trr2>>time2;
	int time1_page = binsearch(tim, tim-time2*3600, code, secret);
	int time2_page = binsearch(tim, tim-time1*3600, code, secret);
	cout<<time1_page<<" "<<time2_page<<endl;
	for(int i=time1_page; i>=time2_page; i--)
		getpagetofile(tim, i, code, secret);
	return 0;
}
