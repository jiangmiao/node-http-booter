(function() {
  var http, server, util;

  http = require('http');

  util = require('util');

  server = http.createServer(function(req, res, hb) {
    return res.end('hello');
  });

  server.listen(1337);

}).call(this);
