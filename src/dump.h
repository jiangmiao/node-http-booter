#ifndef HTTP_BOOTER_IO_H
#define HTTP_BOOTER_IO_H

void dump(const StringPairs & pairs)
{
	for (auto & pair : pairs)
	{
		pair.first.show();
		printf(": ");
		pair.second.show();
		puts("");
	}
}

#endif
