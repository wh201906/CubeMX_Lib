from math import log10
import matplotlib.pyplot as plt


# for power(intensity)
def db2ratio_p(db):
    return 10**(db / 10.0)


def ratio2db_p(ratio):
    return 10 * log10(ratio)


# for field(amplitude)
def db2ratio_a(db):
    return 10**(db / 20.0)


def ratio2db_a(ratio):
    return 20 * log10(ratio)


ADF4351 = [-4, +5]
PE4302 = [i * -0.5 for i in range(2**6)]

dbmList = [(a + p, a, p) for a in ADF4351 for p in PE4302]
WList = [(db2ratio_p(d[0]) * 0.001, d[1], d[2]) for d in dbmList]

ohm = 50

mVrmsList = [((p[0] * ohm) ** 0.5 * 1000, p[1], p[2]) for p in WList]
mVppList = [(v[0] * 2 ** 0.5, v[1], v[2]) for v in mVrmsList]
mVppList = sorted(mVppList)

# print(dbmList)
# print(WList)
# print(mVrmsList
print(mVppList)

x = [i for i in range(len(mVppList))]
plt.scatter(x, list(zip(*mVppList))[0], s=0.1)
plt.show()

# 5dbm - (-4dbm) = 9db, which is a mutiple of 0.5db
# use -4dbm/5dbm to reach the lowest/highest output power