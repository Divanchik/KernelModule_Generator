#include <stdio.h>

static unsigned long s1, s2, s3, s4, s5, b;

void init_taus(unsigned long s)
{
	int i;
	unsigned long x[5];
	
	i=0;
	while (i<3)
	{
		if (s & 0xfffffff0UL)
		{
			x[i]=s;
			i++;
		}
		s = 1664525UL * s + 1UL;
	}
	s1 = x[0]; s2 = x[1]; s3 = x[2]; s4 = x[3]; s5 = x[4];
}

long taus_31 (void)
{

	b = (((s1 << 13) ^ s1) >> 19);
	s1 = (((s1 & 4294967294UL) << 12) ^ b);
	b = (((s2 << 2) ^ s2) >> 25);
	s2 = (((s2 & 4294967288UL) << 4) ^ b);
	b = (((s3 << 3) ^ s3) >> 11);
	s3 = (((s3 & 4294967280UL) << 17) ^ b);
	b = (((s4 << 4) ^ s4) >> 15);
	s4 = (((s4 & 4294967270UL) << 10) ^ b);
	b = (((s5 << 5) ^ s5) >> 16);
	s5 = (((s5 & 4294967280UL) << 13) ^ b);

	return (long) ((s1 ^ s2 ^ s3 ^ s4 ^ s5) >> 1);
}

int main()
{
	init_taus(3);
	for (int i=0;i<10;i++)
		printf("%ld\n", taus_31());
	return 0;
}