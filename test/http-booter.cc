#include <assert.h>
#include "../src/http-booter.h"

using namespace hb;

int main()
{
	{
		HttpBooter hb;
		hb.write("GET / HTTP/1.1\r\n\r\n");
		assert(hb.succeeded());
		assert(hb.method_id ==  HttpBooter::METHOD_GET);
		assert(hb.http_version == "1.1");
		assert(hb.request_uri == "/" );
	}

	{
		HttpBooter hb;
		hb.write("GET /?a=hello+%E4%BD%A0%E5%A5%BD&b=world HTTP/1.1\r\n\r\n");
		assert(hb.succeeded());
		assert(hb.method_id ==  HttpBooter::METHOD_GET);
		assert(hb.http_version == "1.1");
		assert(hb.get.size() == 2);
		assert(hb.get[0].first == "a");
		assert(hb.get[0].second == "hello 你好");
	}

	{
		char data[] = "POST /?a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C HTTP/1.1\r\n"
		              "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n"
		              "Host: 127.0.0.1:1337\r\n"
		              "Accept: */*\r\n"
		              "Content-Length: 41\r\n"
		              "Content-Type: application/x-www-form-urlencoded\r\n"
		              "\r\n"
		              "a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C";

		HttpBooter hb;
		hb.write(data);
		assert(hb.succeeded());
		assert(hb.get.size() == 2);
		assert(hb.get[0].first == "a");
		assert(hb.get[0].second == "你好");
		assert(hb.post.size() == 2);
		assert(hb.post[0].first == "a");
		assert(hb.post[0].second == "你好");
	}

	{
		char data[] = "POST / HTTP/1.1\r\n"
		              "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n"
		              "Host: 127.0.0.1:1337\r\n"
		              "Accept: */*\r\n"
		              "Content-Length: 445\r\n"
		              "Content-Type: multipart/form-data; boundary=----------------------------3d684f973b22\r\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"foo\"; filename=\"foo\"\r\n"
		              "Content-Type: application/octet-stream\r\n"
		              "\r\n"
		              "foo\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"bar\"; filename=\"bar\"\r\n"
		              "Content-Type: application/octet-stream\r\n"
		              "\r\n"
		              "bar\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"baz\"\r\n"
		              "\r\n"
		              "baz\r\n"
		              "------------------------------3d684f973b22--\r\n";

		HttpBooter hb;
		hb.write(data);
		assert(hb.succeeded());
		hb.dump();
	}
	return 0;
}
