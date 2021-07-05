w = eval(input())
print(w)

harmonyPower = 0
basePower = w[0]**2
for h in w[1:]:
    harmonyPower = harmonyPower + h**2
print('THD:', (harmonyPower / basePower)**0.5)
