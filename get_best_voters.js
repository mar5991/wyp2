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

fs.readdir(process.argv[2], function (err, files) {
  if (err) throw err;
  var dat3 = [];
  var licznik = 0;
  for(var i=0; i<files.length; i++)
  {
	fs.readFile(process.argv[2]+"/"+files[i], 'utf8', function (err,data) {
	  if (err) {
	    return console.log(err);
	  }
	  var dat2 = JSON.parse(data);

	  for(var i=0; i<dat2.length; i++)
	  {
	  	var d = new Date(Date.parse(dat2[i].date));
	  	dat2[i].realDate = d;
	  	dat3[dat2[i].id]=dat2[i];
	  }
	  licznik++;
	  if(licznik==files.length)
	  {
	  	var dat4 = [];
	  	var licz = 0;
	  	for(var i in dat3)
	  	{
	  		var ddd = new Date();
	  		if(ddd-Date.parse(dat3[i].date)<1000*60*60*24*1)
	  		{
	  			dat4[licz] = dat3[i];
	  			licz++;
	  		}
	  	}
	  	dat4.sort(function(a,b){a.vote_count-b.vote_count});
	  	var dat5=[];
		for(var i=0; i<Math.min(dat4.length, 40); i++)
		{
			dat5[i]=dat4[i];
		}
		console.log(JSON.stringify(dat5));
	}
	});

}
});
