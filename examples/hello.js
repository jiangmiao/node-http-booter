(function() {
  var HttpBooter, server, util;

  HttpBooter = require(__dirname + '/../').HttpBooter;

  util = require('util');

  server = HttpBooter.createServer(function(req, res, hb) {
    return res.end('hello');
  });

  server.listen(1337);

}).call(this);
