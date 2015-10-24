var exec = require('child_process').exec;
var cmd = "wget -O- -d http://a.wykop.pl/user/login/appkey,n8J9RJlxwa --header=\"apisign: 265ea5b57de80ea74a3f263c54cb4e8b\" --post-data \"login=marc1027&accountkey=QKvKUWyXCYGbuQsFWuTh\"";
var cmd2 = "./wysylanie \"no elo to jest test z wypokapi #test\" ";
var cmd3 = " n8J9RJlxwa a7ZItda0z8";

exec(cmd, function(error, stdout, stderr) {
	var p = JSON.parse(stdout);
	exec(cmd2+p.userkey+cmd3, function(error, stdout, stderr) {
		console.log(stdout);
	});
});
/*
wget -d http://a.wykop.pl/user/login/appkey,n8J9RJlxwa --header="apisign: 265ea5b57de80ea74a3f263c54cb4e8b" --post-data "login=marc1027&accountkey=QKvKUWyXCYGbuQsFWuTh"

var fs = require('fs')

function contains(a, obj) {
    var i = a.length;
    while (i--) {
       if (a[i] === obj) {
           return true;
       }
    }
    return false;
}

function convertLatex(x)
{
	var wynik = "";
	for(var i=0; i<x.length; i++)
	{
		if(x[i]=='#' || x[i]=='_' || x[i]=='&' || x[i]=='$' || x[i]=='^' || x[i]=='%')
			wynik+="\\";
		if(x[i]!='\n')
			wynik+=x[i];
		else
			wynik+=" \\newline ";
	}
	///|[\xC0-\xDF][\x80-\xBF]||[\xE0-\xEF][\x80-\xBF]{2}
	///[|
	var re = /(?![\x00-\x7F]|[ąćęłńóśźżĄĆĘŁŃÓŚŹŻ])./g;
	wynik = wynik.replace(re, "")
	return wynik;
}

function contains2(a, tabobj) {
	for(var i=0; i<tabobj.length; i++)
	{
		if(contains(a, tabobj[i]))
			return true;
	}
	return false;
}


fs.readFile(process.argv[2], 'utf8', function (err,data) {
		if (err) {
		    return console.log(err);
		}

	}
});

*/