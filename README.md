About Node Http Booter
======================

Node Http Booter will initialize get, post, cookie, files of request. support postdata and multipart.

Features
========

1. High Performance

    The http parse and analyze is written in C++0x

2. Easy to use

Installation
============

    npm install http-booter

Usage
=====

Basic Usage
-----------

    # examples/simple.js

    var HttpBooter, server, util;

    HttpBooter = require(__dirname + '/../').HttpBooter;

    util = require('util');

    server = HttpBooter.createServer(function(req, res, hb) {
        return res.end(util.inspect(hb));
    });

    server.listen(1337);


very similar http.createServer but the instance is created by HttpBooter and has an addition variable hb, hb will contains extracted get post cookie and files.

after running the example above

Test get and post

    $ curl '127.0.0.1:1337/foo%20bar/?a%5B%5D=%E4%BD%A0%E5%A5%BD,%E4%B8%96%E7%95%8C&a%5B%5D=hello,world&b=good' -d 'c%5B%5D=cc&c%5B%5D=dd&e=f'

    { get: { a: [ '你好,世界', 'hello,world' ], b: 'good' },
      post: { c: [ 'cc', 'dd' ], e: 'f' },
      cookie: {},
      files: {} }

all fields has been uescaped and a[] and c[] has been grouped.

Test cookie and multipart upload

    $ curl '127.0.0.1:1337' -F 'a=@index.js' -H 'Cookie: a=b; c=d'

    { get: {},
      post: {},
      cookie: { a: 'b', c: 'd' },
      files: 
       { a: 
          { filename: 'index.js',
            data: <SlowBuffer 6d 6f 64 75 6c 65 2e 65 78 70 6f 72 74 73 20 3d 20 72 65 71 75 69 72 65 28 27 2e 2f 6c 69 62 2f 68 74 74 70 2d 62 6f 6f 74 65 72 27 29 3b 0a> } } }

Advance Usage
-------------

Performance
===========


