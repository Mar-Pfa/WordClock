/*
if (!String.prototype.endsWith) {
  String.prototype.endsWith = function(searchString, position) {
      var subjectString = this.toString();
      if (typeof position !== 'number' || !isFinite(position) || Math.floor(position) !== position || position > subjectString.length) {
        position = subjectString.length;
      }
      position -= searchString.length;
      var lastIndex = subjectString.indexOf(searchString, position);
      return lastIndex !== -1 && lastIndex === position;
  };
}
*/

var ajax = {
    /*
        name = name of the ressource
        cb = callback
        a = action after loading (for automatically executing an action)
    */
    loaddirect: function(name, cb)
    {
        var r = new XMLHttpRequest();
        r.op = {
            _url:name,
            _cb:cb
        };
        r.onreadystatechange = ajax.RCDirect;
        r.open("GET",r.op._url, true);
        r.send(null);
    },
    RCDirect: function()
    {
        if (this.readyState == 4)
        {		
            if (this.op._cb)
            {
                this.op._cb(this.responseText);
            }
        }
    },
    load: function(name, a, cb)
    {               
        if (name.endsWith(".js"))
        {
            var s = document.createElement( 'script' );
            s.setAttribute( 'src', name );
            s.onload=cb;
            document.body.appendChild( s );		

        }
        else
        {
            var r = new XMLHttpRequest();
            r.op = {
                _url:"load?name="+name,
                _data:"",
                _cb:cb,
                _s:0,
                _a:a
            };
            r.onreadystatechange = ajax.RC;
            r.open("GET",r.op._url, true);
            r.send(null);
        }
    },
    RC : function ()
    {
        if (this.readyState == 4)
        {		
            this.op._data+=this.responseText;
            if (this.responseText.length == 4000)
            {                
                // need another request
                var r = new XMLHttpRequest();
                r.op = this.op;
                r.op._s += this.responseText.length;
                r.onreadystatechange = ajax.RC;
                console.log("need another request "+this.op._s);
                r.open("GET",this.op._url+"&start="+this.op._s, true);
                r.send(null);		
            }
            else
            {
                console.log("completed request "+this.op._url+ " with "+this.responseText.length+" chars - total "+this.op._data.length);
                // complete!
                if (this.op._a)
                {
                    // perform an action
                    switch(this.op._a.Type)
                    {
                        case "replace":
                            var e = document.getElementById(this.op._a.Id);
                            e[this.op._a.Tag] = this.op._data;
                            break;
                        case "insert":
                            if (this.op._url.endsWith(".js"))
                            {
                				var script = document.createElement("script");
                				script.text = this.op._data;
				                document.body.appendChild(script);					                
                            } else if (this.op._url.endsWith(".css"))
                            {
                                var css = document.createElement("link");
                                css.setAttribute("rel", "stylesheet");
                                css.setAttribute("type", "text/css");
                                css.setAttribute("href", 'data:text/css,'+escape(this.op._data));
                                var head = document.getElementsByTagName("head")[0];
                                head.appendChild(css); 
                            }
                            break;
                        default:
                            throw "unknown action "+this.op._a.Type;
                    }
                }
                if (this.op._cb)
                {
                    this.op._cb(this.op._data);
                }
            }
        };	
    }
}