(function() {
  var HttpBooter, fs, htable, server, util;

  HttpBooter = require(__dirname + '/../').HttpBooter;

  util = require('util');

  fs = require('fs');

  htable = {
    '<': '&lt;',
    '>': '&gt;'
  };

  server = HttpBooter.createServer(function(req, res, hb) {
    return res.end(fs.readFileSync(__dirname + "/upload.html", 'utf-8') + ("<pre>" + (util.inspect(hb).replace(/[<>]/g, (function(m) {
      return htable[m];
    }))) + "</pre>"));
  });

  server.listen(1337);

}).call(this);
