from math import sqrt, erfc, exp
from scipy.special import gammainc
import numpy as np

ALPHA = 0.01
PI_K3M8 = [0.2148, 0.3672, 0.2305, 0.1875]
PI_K5M128 = [0.1174, 0.2430, 0.2493, 0.1752, 0.1027, 0.1124]
PI_K6M10000 = [0.0882, 0.2092, 0.2483, 0.1933, 0.1208, 0.0675, 0.0727]


def Frequency(eps: list):
    """Frequency (Monobit) Test"""
    print("Frequency test")
    n = len(eps)
    Sn = eps.count(1) - eps.count(0)
    Sobs = abs(Sn)/sqrt(n)
    Pval = erfc(Sobs/sqrt(2))
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def BlockFrequency(eps: list, M: int):
    """Frequency Test within a Block"""
    print("Block Frequency test")
    n = len(eps)
    N = n//M
    if not(n >= N*M and M >= 20 and M >= .01*n and N < 100):
        print("INITIAL FAILURE")
        return
    pi = [0 for i in range(N)]
    for i in range(N):
        for j in range(M):
            pi[i] += eps[i * M + j]
        pi[i] /= M
    hi = 0
    for i in pi:
        hi += (i - 1/2)**2
    hi *= 4 * M
    Pval = gammainc(N/2, hi/2)
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def Runs(eps: list):
    """Runs Test"""
    print("Runs Test")
    n = len(eps)
    pi = eps.count(1)/n
    tau = 2/sqrt(n)
    if abs(pi - 1/2) >= tau:
        print("INITIAL FAILURE")
        return
    Vobs = 1
    for i in range(n-1):
        Vobs += 0 if eps[i] == eps[i+1] else 1
    Pval = erfc((Vobs - 2 * pi * n * (1 - pi)) /
                (2 * sqrt(2 * n) * pi * (1 - pi)))
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def longest_run(block: list) -> int:
    maxlen = 0
    len = 0
    for i in block:
        if i == 1:
            len += 1
        if i == 0:
            maxlen = max(maxlen, len)
            len = 0
    return maxlen


def LongestRunOfOnes_128(eps: list):
    """Test for the Longest Run of Ones in a Block (n = 128)"""
    print("Longest Run of Ones in a Block (n = 128)")
    if len(eps) < 128:
        print("INITIAL FAILURE")
        return
    M = 8
    K = 3
    N = 16
    v = [0 for i in range(K+1)]
    for i in range(N):
        tmp = longest_run(eps[i*N:i*N+M])
        if tmp <= 1:
            v[0] += 1
        elif tmp >= 4:
            v[3] += 1
        else:
            v[tmp-1] += 1
    hi = 0
    for i in range(K+1):
        hi += (v[i] - N * PI_K3M8[i])**2 / (N * PI_K3M8[i])
    Pval = gammainc(K/2, hi/2)
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def LongestRunOfOnes_6272(eps: list):
    """Test for the Longest Run of Ones in a Block (n = 6272)"""
    print("Longest Run of Ones in a Block (n = 6272)")
    if len(eps) < 6272:
        print("INITIAL FAILURE")
        return
    M = 128
    K = 5
    N = 49
    v = [0 for i in range(K+1)]
    for i in range(N):
        tmp = longest_run(eps[i*N:i*N+M])
        if tmp <= 4:
            v[0] += 1
        elif tmp >= 9:
            v[5] += 1
        else:
            v[tmp-4] += 1
    hi = 0
    for i in range(K+1):
        hi += (v[i] - N * PI_K5M128[i])**2 / (N * PI_K5M128[i])
    Pval = gammainc(K/2, hi/2)
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def LongestRunOfOnes_750000(eps: list):
    """Test for the Longest Run of Ones in a Block (n = 750000)"""
    print("Longest Run of Ones in a Block (n = 750000)")
    if len(eps) < 750000:
        print("INITIAL FAILURE")
        return
    M = 10000
    K = 6
    N = 75
    v = [0 for i in range(K+1)]
    for i in range(N):
        tmp = longest_run(eps[i*N:i*N+M])
        if tmp <= 10:
            v[0] += 1
        elif tmp >= 16:
            v[6] += 1
        else:
            v[tmp-10] += 1
    hi = 0
    for i in range(K+1):
        hi += (v[i] - N * PI_K6M10000[i])**2 / (N * PI_K6M10000[i])
    Pval = gammainc(K/2, hi/2)
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")


def list2matrix(a: list, h: int, w: int):
    tmp = []
    for i in range(h):
        tmp.append(a[i*h:i*h+w])
    return np.array(tmp)


def Rank(eps: list):
    """Binary Matrix Rank Test"""
    # print("Matrix Rank Test")
    n = len(eps)
    M = 32 # matrix height
    Q = 32 # matrix width
    block_size = M*Q
    N = n//block_size
    mats = []
    for i in range(N):
        mats.append(list2matrix(eps[i*block_size:i*block_size+block_size], M, Q))
    R = []
    for i in range(N):
        R.append(np.linalg.matrix_rank(mats[i]))
    Fm = R.count(M)
    Fm1 = R.count(M-1)
    rem = N - Fm - Fm1
    hi = (Fm - 0.2888*N)**2 / (0.2888*N) + (Fm - 0.5776*N)**2 / (0.5776*N) + (rem - 0.1336*N)**2 / (0.1336*N)
    print(Fm, '/', Fm1, '/', rem, ":", hi)
    Pval = exp(-hi/2)
    print("P =", round(Pval, 3), end=' ')
    print("FAILED" if Pval < ALPHA else "PASSED", end="\n\n")