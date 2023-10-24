K = 30
i = [1] * K
w = [1] * K
b = float(0xab)

print(i)
print(w)

x = 0
for j in range(0, K):
    x += (i[j] * w[j])
x += b
print(b)
print(x)
