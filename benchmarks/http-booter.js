(function() {
  var HttpBooter, all_data, data, fs, hb, i, name, puts, st, _i, _len, _ref;

  HttpBooter = require(__dirname + '/../build/default/http-booter').HttpBooter;

  fs = require('fs');

  fs.writeFileSync('pid', process.pid.toString(), 'utf-8');

  puts = console.info;

  all_data = [];

  data = "GET /?a=hello+%E4%BD%A0%E5%A5%BD&b=world HTTP/1.1\r\n\r\n";

  all_data.push(['getonly', data]);

  data = "POST /?a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C HTTP/1.1\r\n" + "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n" + "Host: 127.0.0.1:1337\r\n" + "Accept: */*\r\n" + "Content-Length: 41\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "\r\n" + "a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C";

  all_data.push(['postdata', data]);

  data = "POST / HTTP/1.1\r\n" + "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n" + "Host: 127.0.0.1:1337\r\n" + "Accept: */*\r\n" + "Content-Length: 445\r\n" + "Content-Type: multipart/form-data; boundary=----------------------------3d684f973b22\r\n" + "Cookie: a=b;c=d\r\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"foo\"; filename=\"foo\"\r\n" + "Content-Type: application/octet-stream\r\n" + "\r\n" + "foo\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"bar\"; filename=\"bar\"\r\n" + "Content-Type: application/octet-stream\r\n" + "\r\n" + "bar\n" + "\r\n" + "------------------------------3d684f973b22\r\n" + "Content-Disposition: form-data; name=\"baz\"\r\n" + "\r\n" + "baz\r\n" + "------------------------------3d684f973b22--\r\n";

  all_data.push(['multipart', data]);

  for (_i = 0, _len = all_data.length; _i < _len; _i++) {
    data = all_data[_i];
    _ref = data, name = _ref[0], data = _ref[1];
    puts(name);
    st = Date.now();
    for (i = 1; i <= 10000; i++) {
      hb = new HttpBooter;
      hb.write(data);
      hb.succeeded();
      hb.format();
      hb.close();
    }
    this;
    puts(Math.round(1e4 * 1e3 / (Date.now() - st)) + " reqs / s");
  }

}).call(this);
