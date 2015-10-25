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
	  	if(dat3[dat2[i].id]==undefined || dat3[dat2[i].id].voters_count<dat2[i].voters_count)
	  		dat3[dat2[i].id]=dat2[i];
	  }
	  licznik++;
	  if(licznik==files.length)
	  {
	  	for(var i in dat3)
	  	{
	  		var regg = dat3[i].body.match(/(#[a-z0-9][a-z0-9\-_]*)/ig);
	  		if(regg == null || contains2(regg, ["#debata", "#polityka", "#korwin", "#kopacz"]) 
	  			|| regg.length==0)
	  		{
				//if(dat3[i].embed!=null)
	  			//	console.log(dat3[i].embed.preview+" "+dat3[i].id+".jpg");

	  			if(dat3[i].voters.length>3)
	  			{
	  				console.log("\\subsection*{"+dat3[i].date.split(" ")[1]);
	  				if(dat3[i].author_group==2)
	  					console.log("\\color[RGB]{188,0,0}");
	  				console.log(convertLatex(dat3[i].author)+" \\color{black} \\href{http://www.wykop.pl/wpis/"+dat3[i].id+"}{"+dat3[i].voters.length+"/"+dat3[i].comment_count+"}}");
	  				console.log(convertLatex(dat3[i].body));
		  			if(dat3[i].embed!=null)
		  			{
		  				console.log("\\begin{figure}[H] \\includegraphics[width=8cm]{jpegs/"+dat3[i].id+".jpg} \\end{figure}");
		  			}
		  			if(dat3[i].comments!=null)
		  			{
		  				for(var j=0; j<dat3[i].comments.length; j++)
		  				{
		  					if(dat3[i].comments[j].voters.length>1)
		  					{
			  					console.log("\\subsubsection*{");
			  					if(dat3[i].comments[j].author_group==2)
		  							console.log("\\color[RGB]{188,0,0}");
		  						console.log(convertLatex(dat3[i].comments[j].author)+" \\color{black} "+dat3[i].comments[j].voters.length+"}");
		  						console.log(convertLatex(dat3[i].comments[j].body));
	  						}
		  				}
		  			}	
	  			}
	  		}

	  		//console.log(dat3[i].date+"\n"+dat3[i].body);
	  	}
	  	console.log("\\end{document}");
	  }
	});
}
});




/*
function ost(data, datax, datay, response)
{
    var wynik = [];
    for(var g=0; g<datax.length; g++)
                    {
                        var tmpWynik = {line: datax[g].line, route: []};
                        var lip = 0;
                        for(var h=0; h<datax[g].route.length; h++)
                        {
                            var id = datax[g].route[h].id;
                            datax[g].route[h].lon=data[id].lat;
                            datax[g].route[h].lat=data[id].lon;
                             datax[g].route[h].name=data[id].name;
                             datax[g].route[h].type="stop";
                            tmpWynik.route[lip]=datax[g].route[h];
                            lip++;
                            if(h<datax[g].route.length-1)
                            {
                                if(datay[datax[g].route[h].id+"x"+datax[g].route[h+1].id]!=undefined)
                                {
                                    var foo = datay[datax[g].route[h].id+"x"+datax[g].route[h+1].id];
                                    for(var k=0; k<foo.length; k++)
                                    {
                                        tmpWynik.route[lip]=foo[k];
                                        lip++;
                                    }
                                }
                            }
                        }
                        wynik[g]=tmpWynik;
                    }
					response(wynik);
}

function doMagic(datax, response, client)
{
    var data = [];
    var datay = [];
	var nor = 0;
	var licznik = 0;
    var nor2 = 0;
    var licznik2 = 0;
	for(var i=0; i<datax.length; i++)
		for(var j=0; j<datax[i].route.length; j++)
		{
			nor++;
		}
	for(var i=0; i<datax.length; i++)
	{
		for(var j=0; j<datax[i].route.length; j++)
		{
            console.log("PRZEGLĄD");
		    var queryConfig = {
                    text: 'SELECT * FROM wspolrzedne WHERE id = $1',
                    values: [datax[i].route[j].id],
                    };
			client.query(queryConfig, function(err, rtes) {
				if(rtes.rows.length==1)
				{
                    data[rtes.rows[0].id]={};
					data[rtes.rows[0].id].lat=rtes.rows[0].lat;
					data[rtes.rows[0].id].lon=rtes.rows[0].lon;
					data[rtes.rows[0].id].name=rtes.rows[0].name;
				}
				licznik++;
				if(licznik==nor && licznik2==nor2)
				{
                    ost(data, datax, datay, response);
                }
			});
            if(j<datax[i].route.length-1)
            {
                nor2++;
                var queryConfig = {
                        text: 'SELECT * FROM trasy WHERE fromx = $1 AND tox = $2',
                        values: [datax[i].route[j].id, datax[i].route[j+1].id]
                        };
                client.query(queryConfig, function(err, rtes) {
                    licznik2++;
                    if(rtes.rows.length>0)
                    {
                        datay[rtes.rows[0].fromx+"x"+rtes.rows[0].tox]=[];
                        for(var i=0; i<rtes.rows.length; i++)
                        {
                            datay[rtes.rows[0].fromx+"x"+rtes.rows[0].tox][i]={type:"point", lon: rtes.rows[i].lon, lat: rtes.rows[i].lat};
                        }
                    }
                    if(licznik==nor && licznik2==nor2)
				    {
                        ost(data, datax, datay, response);
                    }
                });
	        }
        }
    }
}

function pad(num, size) {
	    var s = num+"";
	        while (s.length < size) s = "0" + s;
		    return s;
}

var conString = "postgres://postgres:1234@localhost/hafas";

//var Warszawa = JSON.parse(fs.readFileSync('Przystanki.json', 'utf8'));
http.createServer(function (req, res) {


	    // Website you wish to allow to connect
	        res.setHeader('Access-Control-Allow-Origin', '*');

		    // Request methods you wish to allow
		        res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');

			    // Request headers you wish to allow
			        res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');

				    // Set to true if you need the website to include cookies in the requests sent
				        // to the API (e.g. in case you use sessions)
					    res.setHeader('Access-Control-Allow-Credentials', true);


	var url_parts = url.parse(req.url, true);
	res.ttime = parseInt(url_parts.query.time);
	res.setHeader('Content-Type', 'application/json');
	var d = new Date();
	d.setTime(parseInt(url_parts.query.time)*1000);
	res.dateStr = d.getFullYear()+"-"+pad((d.getMonth()+1), 2)+"-"+pad(d.getDate(), 2);
	res.timeStr = (d.getMinutes()+d.getHours()*60)*60;
	console.log(res.dateStr+" "+res.timeStr+" "+d.getHours()+" "+d.getMinutes());
	/*if(url_parts.query.size==undefined)
	{
		var from1 = url_parts.query.from;
		var from = parseInt(from1);
		var to1 = url_parts.query.to;
		var to = parseInt(to1);
		var wynik = [];
		for(var i=from; i<=to; i++)
		{
			wynik[i-from]=Warszawa[i];
		}
		console.log(JSON.stringify(wynik));
		console.log("--------------------");
		res.end(JSON.stringify(wynik));
	}
	else
	{
		res.end("{\"size\":"+Warszawa.length+"}");
	}*/
/*

//url_parts.query.id

if(url_parts.query.delim!=undefined)
{
		client.query(' SELECT * FROM kalendarz INNER JOIN kursy AS kursy1 ON kalendarz.type_day = kursy1.dzien  INNER JOIN wspolrzedne ON wspolrzedne.id=kursy1.stopid WHERE (day+kursy1.time>\''+res.ttime+'\') AND kursy1.stopid LIKE \''+url_parts.query.from.substr(0,4)+'__\' ORDER BY kursy1.time+kalendarz.day LIMIT 10;', function(err, rtes) {
		console.log(err);
		if(rtes!=undefined)
		{
		var wynik = [];
		for(var i=0; i<rtes.rows.length; i++)
		{
			var foop = {};
			foop.linia=rtes.rows[i].line;
			foop.id=rtes.rows[i].stopid;
			foop.idname=rtes.rows[i].name;
			foop.time=rtes.rows[i].time+rtes.rows[i].day;
			wynik[i]=foop;
		}

		res.end(JSON.stringify(wynik));
		done();
		}
		});
}
else if (url_parts.query.to!=undefined)
{
	var path = "http://127.0.0.1/hafas/?from="+url_parts.query.from+"&to="+url_parts.query.to+"&time="+url_parts.query.time;
	if(path.length>0)
	http.get(path, function(response) {
		response.on('data', function (chunk) {
			var chunk2 = JSON.parse(chunk);
			doMagic(chunk2, function(wyn) {res.end(JSON.stringify(wyn))}, client);
			      });
									        });
}
else
{
		client.query(' SELECT DISTINCT kursy2.*, kalendarz.day FROM kalendarz INNER JOIN kursy AS kursy1 ON kalendarz.type_day = kursy1.dzien INNER JOIN kursy AS kursy2 ON kursy1.line=kursy2.line AND kursy1.kurs_id=kursy2.kurs_id WHERE (day+kursy1.time=\''+res.ttime+'\') AND kursy1.line=\''+url_parts.query.line+'\' AND kursy1.stopid=\''+url_parts.query.from+'\' ORDER BY kursy2.postoj_id;', function(err, rtes) {
			console.log(err);
            if(rtes!=undefined)
			{
			var elem = {};
			elem.line=url_parts.query.line;
			elem.route = [];
			var rrow = rtes.rows;
			for(var i=0; i<rrow.length; i++)
			{
				elem.route[i]={};
				elem.route[i].id = rrow[i].stopid;
				elem.route[i].time = rrow[i].time+rrow[i].day;
			}
            console.log(elem);
	        doMagic([elem], function(wyn) {res.end(JSON.stringify(wyn))}, client);
			done();
			}
		});
}

});
*/