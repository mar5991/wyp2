#include "ett.hpp"

int main(int argc, char** argv)
{
	string body=argv[1];
	string userkey=argv[2];
	string appkey=argv[3];
	string sekret=argv[4];
	string obrazek="";
	if(argc>5)
		obrazek=argv[5];
	send_message(body, obrazek, userkey, appkey, sekret);
}
