def THDcalc():
    w = eval(input())
    print(w)

    harmonyPower = 0
    basePower = w[0]**2
    for h in w[1:]:
        harmonyPower = harmonyPower + h**2
    print('THD:', (harmonyPower / basePower)**0.5)


def squareTHD():
    n = int(input())
    basePower = 1
    harmonyPower = 0
    for i in range(2, n + 1):
        if (i % 2 == 1):
            harmonyPower = harmonyPower + (1 / i)**2
            print('THD(' + str(i) + '):',
                  str((harmonyPower / basePower)**0.5 * 100) + '%')


squareTHD()