import random
import sys


n = int(sys.argv[1])
m = int(sys.argv[2])
maxcap = int(sys.argv[3])

print('p max ' + str(n) + ' ' + str(m))

s = random.randrange(n)
t = random.randrange(n)
while s == t:
    t = random.randrange(n)

print('n ' + str(s+1) + ' s')
print('n ' + str(t+1) + ' t')
for i in range(n):
    u = random.randrange(n)
    while i == u:
        u = random.randrange(n)
    cap = random.randrange(maxcap+1)
    print('a ' + str(i+1) + ' ' + str(u+1) + ' ' + str(cap))
for i in range(m-n):
    u = random.randrange(n)
    v = random.randrange(n)
    while u == v:
        v = random.randrange(n)
    cap = random.randrange(maxcap)+1
    print('a ' + str(u+1) + ' ' + str(v+1) + ' ' + str(cap))