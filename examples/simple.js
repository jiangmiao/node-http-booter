(function() {
  var HttpBooter, server, util;

  HttpBooter = require(__dirname + '/../').HttpBooter;

  util = require('util');

  server = HttpBooter.createServer(function(req, res, hb) {
    return res.end(util.inspect(hb));
  });

  server.listen(1337);

}).call(this);
