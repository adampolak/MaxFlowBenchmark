with open('output.txt') as f:
    firstline = next(f)
    dct = {}
    for line in f:
        u, v, cur_f = [int(x) for x in line.split()]

        curvalue = 0
        try:
            curvalue = dct[(u, v)]
        except:
            curvalue = 0
        dct[(u, v)] = curvalue + cur_f
    for key, value in dct.items():
        print(key[0], key[1], value)