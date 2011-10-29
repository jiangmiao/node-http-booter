#include "utils.h"

HB_NS_START
HB_UTILS_NS_START

// unescape will modify the data!!!
// For the unescaped string's length always less than original text
// So it should be safe
String & unescape(String & str)
{
	char * st = const_cast<char*>(str.data);
	char * ed = st + str.length;

	char * dest = st;

	while (st != ed)
	{
		if (*st == '%')
		{
			if (st + 2 >= ed)
			{
				*dest++ = '?';
				break;
			}

			st++;
			char h = *st++;
			char l = *st++;

			if (h >= 'A')
				h = h - 'A' + 10;
			else
				h = h - '0';
			if (l >= 'A')
				l = l - 'A' + 10;
			else
				l = l - '0';

			*dest++ =  (h << 4) + l;
		}
		else if (*st == '+')
		{
			*dest++ = ' ';
			st++;
		}
		else
		{
			*dest++ = *st++;
		}
	}
	str.length = dest - str.data;
	return str;
}

HB_UTILS_NS_END;
HB_NS_END
