#include <stdio.h>
static unsigned long s[5], b;

void init_taus(unsigned long j)
{
	int i = 0;
	while (i < 5)
	{
		if (j & 0xfffffff0UL)
		{
			s[i]=j;
			i++;
		}
		j = 1664525UL * j + 1UL;
	}
}

long taus_31 (void)
{
	b = (((s[0] << 13) ^ s[0]) >> 19);
	s[0] = (((s[0] & 4294967294UL) << 12) ^ b);
	b = (((s[1] << 2) ^ s[1]) >> 25);
	s[1] = (((s[1] & 4294967288UL) << 4) ^ b);
	b = (((s[2] << 3) ^ s[2]) >> 11);
	s[2] = (((s[2] & 4294967280UL) << 17) ^ b);
	b = (((s[3] << 4) ^ s[3]) >> 15);
	s[3] = (((s[3] & 4294967270UL) << 10) ^ b);
	b = (((s[4] << 5) ^ s[4]) >> 16);
	s[4] = (((s[4] & 4294967280UL) << 13) ^ b);

	return (long) ((s[0] ^ s[1] ^ s[2] ^ s[3] ^ s[4]) >> 1);
}
