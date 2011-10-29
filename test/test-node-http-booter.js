(function() {
  var HttpBooter, data, fs, hb, i, puts, st;

  HttpBooter = require(__dirname + '/../build/default/http-booter').HttpBooter;

  fs = require('fs');

  fs.writeFileSync('pid', process.pid.toString(), 'utf-8');

  puts = console.info;

  data = "POST / HTTP/1.1\r\n" + "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n" + "Host: 127.0.0.1:1337\r\n" + "Accept: */*\r\n" + "Content-Length: 445\r\n" + "Content-Type: multipart/form-data; boundary=----------------------------3d684f973b22\r\n" + "Cookie: a=b;c=d\r\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"foo\"; filename=\"foo\"\r\n" + "Content-Type: application/octet-stream\r\n" + "\r\n" + "foo\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"bar\"; filename=\"bar\"\r\n" + "Content-Type: application/octet-stream\r\n" + "\r\n" + "bar\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"baz\"\r\n" + "\r\n" + "baz\r\n" + "------------------------------3d684f973b22--\r\n";

  st = Date.now();

  for (i = 1; i <= 10000; i++) {
    hb = new HttpBooter;
    hb.write(data);
    hb.succeeded();
    hb.format();
    hb.close();
  }

  this;

  puts(1e4 / (Date.now() - st));

}).call(this);