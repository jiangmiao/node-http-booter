(function() {
  var HttpBooter, fs, server, util;

  fs = require('fs');

  HttpBooter = require('../index').HttpBooter;

  util = require('util');

  fs.writeFileSync('pid', process.pid.toString(), 'utf-8');

  server = HttpBooter.createServer(function(req, res, hb) {
    return res.end(util.inspect(hb));
  });

  server.listen(1337);

}).call(this);
