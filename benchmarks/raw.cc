#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/http-booter.h"

using namespace std;
using namespace hb;

#include <time.h>
int main()
{
	// start
	const char *files[] = {"benchmarks/postdata.txt", "benchmarks/multipart.txt", "benchmarks/getonly.txt"};
	for(auto & file_name : files) 
	{
		puts(file_name);
		FILE * file = fopen(file_name, "rb");
		char buf[10240];
		int n = fread(buf, 1, sizeof(buf), file);

		clock_t st = clock();
		int times = 50000;
		for(int i=0; i<times; ++i) 
		{
			HttpBooter hb;
			hb.write(buf, n);
		}
		printf("%.2lf reqs / s\n", (double)times * CLOCKS_PER_SEC / (clock() - st));
	}
	puts("done");
	return 0;
}
