(function() {
  var HttpBooter, http, i, net;

  HttpBooter = require(__dirname + '/../build/default/http-booter').HttpBooter;

  http = require('http');

  net = require('net');

  i = HttpBooter;

  HttpBooter = (function() {

    function HttpBooter() {
      this.internal = new i();
    }

    HttpBooter.prototype.sync = function(req) {
      var v;
      this.internal.write("" + req.method + " " + req.url + " HTTP/1.1\r\n");
      if (v = req.headers.cookie) this.internal.write("Cookie: " + v + "\r\n");
      if (v = req.headers['content-length']) {
        this.internal.write("Content-Length: " + v + "\r\n");
      }
      if (v = req.headers['content-type']) {
        this.internal.write("Content-Type: " + v + "\r\n");
      }
      return this.internal.write("\r\n");
    };

    HttpBooter.prototype.succeeded = function() {
      return this.internal.succeeded();
    };

    HttpBooter.prototype.format = function() {
      this.get = this.internal._get;
      this.post = this.internal._post;
      this.cookie = this.internal._cookie;
      return this.files = this.internal._files;
    };

    HttpBooter.prototype.write = function(data) {
      return this.internal.write(data);
    };

    HttpBooter.prototype.close = function() {
      if (this.internal) {
        this.internal.close();
        return delete this.internal;
      }
    };

    HttpBooter.prototype.bad = function() {
      return this.internal.bad();
    };

    HttpBooter.sync = function(req) {
      var hb;
      hb = new HttpBooter();
      hb.sync(req);
      return hb;
    };

    HttpBooter.createServer = function(callback) {
      var server;
      return server = http.createServer(function(req, res) {
        var hb;
        var _this = this;
        hb = HttpBooter.sync(req);
        req.hb = hb;
        req.on('data', function(data) {
          return hb.internal.write(data);
        });
        return req.on('end', function() {
          if (hb.succeeded()) {
            hb.format();
            hb.close();
            return callback(req, res, hb);
          } else {
            hb.end();
            return res.end('error');
          }
        });
      });
    };

    HttpBooter.createSocket = function(callback) {
      var server;
      server = net.createServer(function(socket) {
        var hb;
        hb = new HttpBooter();
        socket.on('data', function(data) {
          hb.write(data);
          if (hb.succeeded()) {
            hb.format();
            return callback(socket, hb);
          } else if (hb.bad()) {
            hb.close();
            return socket.end();
          }
        });
        socket.on('timeout', function() {});
        socket.on('close', function() {
          return hb.close();
        });
        socket.on('end', function() {});
        return socket.on('error', function() {});
      });
      return server;
    };

    return HttpBooter;

  })();

  exports.HttpBooter = HttpBooter;

}).call(this);
