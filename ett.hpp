#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include <map>
#include <climits>
#include <ctime>
#include <set>
#include <sstream>
#include "curl_easy.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "md5.h"
using curl::curl_easy;
using namespace rapidjson;
using namespace std;

string root;

pair <int, int> liczby(string txt)
{
	string por=".txt";
	bool ok=1;
	if(txt.length()<por.length())
		ok=0;
	for(int i=1; i<=por.length(); i++)
	{
		if(por[por.length()-i]!=txt[txt.length()-i])
			ok=0;
	}
	pair <int, int> wynik(-1, -1);
	if(ok)
	{
		stringstream foo;
		foo<<txt;
		foo>>wynik.first;
		char dupa;
		foo>>dupa;
		foo>>wynik.second;
	}
	return wynik;
}

vector <pair<int, int> > pliki()
{
	vector <pair<int, int> > wynik;
	DIR *dp;
	dirent *d;
	dp = opendir(root.c_str());
	while((d = readdir(dp) )!= NULL)
	{
		string dd = d->d_name;
		auto ldd=liczby(dd);
		if(ldd.first!=-1)
			wynik.push_back(ldd);
	}
	return wynik;
}
bool alfanummalpa(char x)
{
	if(x>=58 && x<=64)
		return false;
	if(x>=91 && x<=96)
		return false;
	if(x>=45 && x<=122)
		return true;
	return false;
}

bool alfanumtag(char x)
{
	if(x>=58 && x<=64)
		return false;
	if(x>=91 && x<=96)
		return false;
	if(x>=48 && x<=122)
		return true;
	return false;
}

int partialint(int l, int r, string xxx)
{
	stringstream foo;
	for(int i=l; i<=r; i++)
	{
		foo<<xxx[i];
	}
	int wynik;
	foo>>wynik;
	return wynik;
}

string cdata(long long datac)
{
	time_t tix=datac;
	struct tm* ptm;
	ptm = gmtime(&tix);
	stringstream foo;
	foo<<ptm->tm_year+1900<<"-";
	foo<<ptm->tm_mon+1<<"-";
	foo<<ptm->tm_mday<<" ";
	foo<<ptm->tm_hour<<":";
	foo<<ptm->tm_min<<":";
	foo<<ptm->tm_sec;
	return foo.str();
}

bool czynocna(long long time)
{
	time_t tix=time;
	struct tm* ptm;
	ptm = gmtime(&tix);
	if(ptm->tm_hour>6)
		return false;
	return true;
}

long long datac(string date)
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	int year=partialint(0, 3, date);
	int month=partialint(5, 6, date);
	int day=partialint(8, 9, date);
	int hour=partialint(11, 12, date);
	int min=partialint(14, 15, date);
	int sec=partialint(17, 18, date);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;
	timeinfo->tm_hour = hour%24;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;
	int wynik=0;
	time_t timer = time(0);
	return mktime(timeinfo);
}

vector <string> filtrujMalpy(string content)
{
	vector <string> wynik;
	for(int i=0; i<content.length(); i++)
	{
		if(content[i]=='@')
		{
			i++;
			string tmp;
			bool ok=1;
			for(;(i<content.length() && ok); i++)
			{
				if(alfanummalpa(content[i]))
				{
					tmp+=content[i];
				}
				else
				{
					ok=0;
					i--;
				}
			}
			wynik.push_back(tmp);
		}
	}
	return wynik;
}

vector <string> filtrujTagi(string content)
{
	vector <string> wynik;
	for(int i=0; i<content.length(); i++)
	{
		if(content[i]=='#')
		{
			i++;
			string tmp;
			bool ok=1;
			for(;(i<content.length() && ok); i++)
			{
				if(alfanumtag(content[i]))
				{
					tmp+=content[i];
				}
				else
				{
					ok=0;
					i--;
				}
			}
			wynik.push_back(tmp);
		}
	}
	return wynik;
}

struct voter
{
	string nick;
	long long data;
	voter(Value& dupa)
	{
		nick = dupa["author"].GetString();
		data=datac(dupa["date"].GetString());
	}
};

struct komentarz
{
	string content;
	string nick;
	long long id;
	long long data;
	vector <string> tagi;
	vector <voter> plusy;
	vector <string> wolani;
	komentarz(Value& dupa)
	{
		id = dupa["id"].GetInt();
		content = dupa["body"].GetString();
		nick = dupa["author"].GetString();
		Value& ctt = dupa["voters"];
		for(int i=0; i<ctt.Size(); i++)
		{
			plusy.push_back(voter(ctt[i]));
		}
		wolani=filtrujMalpy(content);
		tagi=filtrujTagi(content);
		data=datac(dupa["date"].GetString());
	}
	bool tagwystepuje(string tag)
	{
		for(int i=0; i<tagi.size(); i++)
		{
			if(tagi[i]==tag)
				return true;
		}
		return false;
	}
};

long long mindata;

struct wpis
{
	string content;
	string nick;
	long long id;
	long long data;
	long long data_download;
	vector <string> tagi;
	vector <voter> plusy;
	vector <string> wolani;
	vector <komentarz> komentarze;
	int plusy_z_komentarzy()
	{
		int wynik=0;
		for(int i=0; i<komentarze.size(); i++)
		{
			wynik+=komentarze[i].plusy.size();
		}
		return wynik;
	}
	wpis(Value& dupa, long long down, bool comments=1)
	{
		data_download = down;
		id = dupa["id"].GetInt();
		content = dupa["body"].GetString();
		nick = dupa["author"].GetString();
		Value& ctt = dupa["voters"];
		for(int i=0; i<ctt.Size(); i++)
		{
			plusy.push_back(voter(ctt[i]));
		}
		wolani=filtrujMalpy(content);
		tagi=filtrujTagi(content);
		data=datac(dupa["date"].GetString());
		mindata=min(data, mindata);
		if(comments)
		{
			Value& ctt2 = dupa["comments"];
			for(int i=0; i<ctt2.Size(); i++)
			{
				komentarze.push_back(komentarz(ctt2[i]));
			}
		}
	}
	bool tagwystepuje(string tag)
	{
		for(int i=0; i<tagi.size(); i++)
		{
			if(tagi[i]==tag)
				return true;
		}
		return false;
	}
	wpis() {}
};
class IStreamWrapper {
public:
typedef char Ch;
IStreamWrapper(std::istream& is) : is_(is) {
}
Ch Peek() const { // 1
int c = is_.peek();
return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
}
Ch Take() { // 2
int c = is_.get();
return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
}
size_t Tell() const { return (size_t)is_.tellg(); } // 3
Ch* PutBegin() { assert(false); return 0; }
void Put(Ch) { assert(false); }
void Flush() { assert(false); }
size_t PutEnd(Ch*) { assert(false); return 0; }
private:
IStreamWrapper(const IStreamWrapper&);
IStreamWrapper& operator=(const IStreamWrapper&);
std::istream& is_;
};
void getpagetofile(int tim, int page, string appkey, string sekret)
{
	stringstream url;
	url<<"http://a.wykop.pl/search/entries/appkey,"<<appkey<<",output,clear";
	url<<",page,"<<page;
	stringstream pagename;
	pagename<<root<<tim<<"-"<<page<<".txt";
	string header1=md5(sekret+url.str());
	string header="apisign: "+header1;
	stringstream polecenie;
	polecenie<<"wget --header=\""<<header<<"\" "<<url.str()<<" -O "<<pagename.str();
	system(polecenie.str().c_str());
}

void send_message(string body, string obrazek, string userkey, string appkey, string sekret)
{
	cout<<obrazek<<endl;
	stringstream url;
	url<<"http://a.wykop.pl/entries/add/appkey,"<<appkey<<",userkey,"<<userkey;
	stringstream pagename;
	pagename<<"xxx.atxt";
	string bo=body+","+obrazek;
	cout<<bo<<endl;
	string header1=md5(sekret+url.str()+bo);
	if(obrazek=="")
		header1=md5(sekret+url.str()+body);
	string header="apisign: "+header1;
	stringstream polecenie;
	stringstream post;
	post<<"body="<<body;
	if(obrazek!="")
		post<<"&embed="<<obrazek;
	polecenie<<"wget --header=\""<<header<<"\" --post-data \""<<post.str()<<"\" "<<url.str()<<" -O "<<pagename.str();
	cout<<polecenie.str()<<endl;
	cout<<post.str()<<endl;
	system(polecenie.str().c_str());
}

int getpagefromfile(int akt_time, int kand_time, int page, map<int, wpis>& wpisy)
{
	string lol;
	stringstream pagename;
	pagename<<root<<akt_time<<"-"<<page<<".txt";
	std::ifstream t(pagename.str().c_str());
	std::string str;

	t.seekg(0, std::ios::end);   
	lol.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	lol.assign((std::istreambuf_iterator<char>(t)),
	            std::istreambuf_iterator<char>());
	stringstream stt;
	stt<<lol;
	IStreamWrapper is(stt);
	Document d;
	d.ParseStream(is);
	int s1=d.Size();
	for (int i = s1-1; i >=0 ; i--)
	{
		wpis nowy(d[i], kand_time);
		if(wpisy.find(nowy.id)!=wpisy.end())
		{
			if(wpisy.find(nowy.id)->second.data_download<kand_time)
			{
				wpisy[nowy.id]=nowy;
			}
		}
		else
			wpisy[nowy.id]=nowy;
	}
	auto it1 = wpisy.begin();
	auto it2 = wpisy.end();
	it2--;
	if(wpisy.size()==0)
		return -1;
	if(it2->second.data<kand_time)
		return -1;
	if(it1->second.data>kand_time)
		return 1;
	return 0;
}


int getpage_to_plot(int akt_time, int old_time, int page, map<int, wpis>& wpisy)
{
	cout<<old_time<<" "<<page<<endl;
	string lol;
	stringstream pagename;
	pagename<<root<<old_time<<"-"<<page<<".txt";
	std::ifstream t(pagename.str().c_str());
	std::string str;

	t.seekg(0, std::ios::end);   
	lol.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	lol.assign((std::istreambuf_iterator<char>(t)),
	            std::istreambuf_iterator<char>());
	stringstream stt;
	stt<<lol;
	IStreamWrapper is(stt);
	Document d;
	d.ParseStream(is);
	int s1=d.Size();
	for (int i = s1-1; i >=0 ; i--)
	{
		wpis nowy(d[i], old_time, 0);
		if(nowy.data>(akt_time-24*60*60))
		{
			if(wpisy.find(nowy.id)!=wpisy.end())
			{
				if(wpisy.find(nowy.id)->second.data_download<old_time)
				{
					wpisy[nowy.id]=nowy;
				}
			}
			else
				wpisy[nowy.id]=nowy;
		}
	}
}



int getpage_bin(int akt_time, int kand_time, int page, string key, string secret)
{
	map <int, wpis> wpisy;
	getpagetofile(akt_time, page, key, secret);
	int wynik = getpagefromfile(akt_time, kand_time, page, wpisy);
	return wynik;
}
vector <wpis> transfer(map<int, wpis> wpisy)
{
	vector <wpis> wynik;
	auto it1=wpisy.begin();
	while(it1!=wpisy.end())
	{
		wynik.push_back(it1->second);
		it1++;
	}
	return wynik;
}
bool srt(const wpis& alfa, const wpis& beta)
{
	if(alfa.plusy.size()>beta.plusy.size())
		return true;
	return false;
}
int binsearch(int akt_time, int kand_time, string code, string secret)
{
	int left = 1;
	int right = 1000;
	bool process = 1;
	while(process)
	{
		if(right-left<2)
		{
			process=0;
			int wyn_right = getpage_bin(akt_time, kand_time, right, code, secret);
			int wyn_left = getpage_bin(akt_time, kand_time, left, code, secret);
			if(wyn_left==0)
				return left;
			if(wyn_right==0)
				return right;
			return left;
		}
		int srodek = (left+right)/2;
		int wyn_srodek = getpage_bin(akt_time, kand_time, srodek, code, secret);
		if(wyn_srodek==0)
			return srodek;
		if(wyn_srodek==1)
			left=srodek+1;
		if(wyn_srodek==-1)
			right=srodek-1;
	}
	return 1;
}


