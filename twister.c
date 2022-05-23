#define P 624
#define Q 397
#define MATRIX_A 0x9908b0dfUL // постоянный вектор a
#define UPPER_MASK 0x80000000UL // наиболее значимые (w-r) бит
#define LOWER_MASK 0x7fffffffUL // последние значимые r бит

static unsigned long mt[P]; // массив состояния вектора
static int mti = P + 1; // mti==P+1 означает, что mt[P] не инициализирован

// инициализация mt[P] с начальным значением a
void init_genrand(unsigned long seed)
{
    mt[0] = seed & 0xffffffffUL;
    // for (mti=1;mti<P;mti++)
    mti = 1;
    while(mti < P)
    {
        mt[mti] = (1664525UL * mt[mti-1] + 1UL);
        mt[mti] &= 0xffffffffUL;
        mti++;
    }
}

unsigned long genrand(void)
{
    unsigned long y;
    static unsigned long mag01[2] = {0x0UL, MATRIX_A};
    if (mti >= P)
    {
        int kk;
        if (mti == P + 1)
            init_genrand(5489UL);
        // for (kk = 0;kk < P - Q;kk++)
        kk = 0;
        while (kk < P - Q)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk+Q] ^ (y>>1) ^ mag01[y & 0x1UL];
            kk++;
        }
        // for (;kk < P - 1;kk++)
        while (kk < P - 1)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk+(Q-P)] ^ (y>>1) ^ mag01[y & 0x1UL];
            kk++;
        }
        y = (mt[P-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[P-1] = mt[Q-1] ^ (y>>1) ^ mag01[y & 0x1UL];
        mti = 0;
    }
    y = mt[mti++];
    // темперирование
    y ^= (y>>11);
    y ^= (y<<7) & 0x9d2c5680UL;
    y ^= (y<<15) & 0xefc60000UL;
    y ^= (y>>18);
    return y;
}

long genrand_31(void)
{
    return (long)(genrand()>>1);
}
