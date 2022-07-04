with open('LB07-bunny-sml.max') as f:
    dct = {}
    S = 0
    T = 0
    v_num = 0
    for line in f:
        if line[0] == 'p':
            p, mx, vn, en = [x for x in line.split()]
            v_num = int(vn)
            continue
        if line[0] == 'n':
            n, u, t = [x for x in line.split()]
            u = int(u)
            if t == 's':
                S = u
            else:
                T = u
            continue
        elif line[0] != 'a':
            continue
        a, u, v, cur_f = [x for x in line.split()]

        u = int(u)
        v = int(v)
        cur_f = int(cur_f)
        if cur_f == 0:
            continue

        curvalue = 0
        try:
            curvalue = dct[(u, v)]
        except:
            curvalue = 0
        dct[(u, v)] = curvalue + cur_f
    print('p max ' + str(v_num) + ' ' + str(len(dct)))
    print('n ' + str(S) + ' s')
    print('n ' + str(T) + ' t')
    for key, value in dct.items():
        print('a ' + str(key[0]) + ' ' + str(key[1]) + ' ' + str(value))
