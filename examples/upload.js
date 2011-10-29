(function() {
  var HttpBooter, fs, server, util;

  HttpBooter = require(__dirname + '/../').HttpBooter;

  util = require('util');

  fs = require('fs');

  server = HttpBooter.createServer(function(req, res, hb) {
    return res.end(fs.readFileSync(__dirname + "/upload.html", 'utf-8') + ("<pre>" + (util.inspect(hb)) + "</pre>"));
  });

  server.listen(1337);

}).call(this);
