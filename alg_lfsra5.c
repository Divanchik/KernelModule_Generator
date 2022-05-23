#include <stdio.h>
unsigned l1, l2, l3;
unsigned l1_mask = 0x7ffffU;
unsigned l2_mask = 0x3fffffU;
unsigned l3_mask = 0x7fffffU;

unsigned _get(unsigned r, unsigned i) { return (r & (1 << i)) >> i; }
void _set1(unsigned *r, unsigned i) { *r = *r | (1 << i); }
void _set0(unsigned *r, unsigned i) { if (_get(*r, i)) *r = *r - (1 << i); }

unsigned f_l1() { return _get(l1, 18) ^ _get(l1, 17) ^ _get(l1, 16) ^ _get(l1, 13); }
unsigned f_l2() { return _get(l2, 21) ^ _get(l2, 20); }
unsigned f_l3() { return _get(l3, 22) ^ _get(l3, 21) ^ _get(l3, 20) ^ _get(l3, 7); }

unsigned tick()
{
    unsigned x = _get(l1, 8), y = _get(l2, 10), z = _get(l3, 10);
    unsigned F = (x & y) | (x & z) | (y & z);

    unsigned f = 0;
    unsigned res1 = 0;
    unsigned res2 = 0;
    unsigned res3 = 0;
    if (x == F)
    {
        res1 = _get(l1, 18);  // get result
        f = f_l1();           // calc feedback
        l1 <<= 1;             // shift
        if (f) _set1(&l1, 0); // write feedback
        l1 &= l1_mask;        // fix overflow
    }
    if (y == F)
    {
        res2 = _get(l2, 21);  // get result
        f = f_l2();           // calc feedback
        l2 <<= 1;             // shift
        if (f) _set1(&l2, 0); // write feedback
        l2 &= l2_mask;        // fix overflow
    }
    if (z == F)
    {
        res3 = _get(l3, 18);  // get result
        f = f_l3();           // calc feedback
        l3 <<= 1;             // shift
        if (f) _set1(&l3, 0); // write feedback
        l3 &= l3_mask;        // fix overflow
    }
    return res1 ^ res2 ^ res3;
}

int main()
{
    l1 = 38550811;
    l2 = 4604513;
    l3 = 17932230;
    for (int i=0;i<10;i++)
        printf("%ld\n", get_rand());
    return 0;
}
