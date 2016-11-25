#!/usr/bin/env python3
from shutil import copyfile


invalid_ints = ["Argh!", "7.", "-7", "7e4", "7 ", " 7", "  7", "7  ", " 7 ",
                "382ahoj", "ahoj732", "ah002oj", "", "\n1", "12 34"]

'''
Desetinný literál (rozsah C-double) také vyjadřuje nezáporná čísla v desítkové sou-
stavě, přičemž literál je tvořen celou a desetinnou částí, nebo celou částí a exponen-
tem, nebo celou a desetinnou částí a exponentem. Celá i desetinná část je tvořena
neprázdnou posloupností číslic. Exponent je celočíselný, začíná znakem ’ e ’ nebo
’ E ’, následuje nepovinné znaménko ’ + ’ (plus) nebo ’ - ’ (mínus) a poslední částí je
neprázdná posloupnost číslic. Mezi jednotlivými částmi nesmí být jiný znak, celou a
desetinnou část odděluje znak ’ . ’ (tečka)
'''

#Valid: 7.0, 7.2e5, 13e7, 13e+7, 13e-7
valid_dbl = [
    ["16", ".", "16"],
    ["16", ".", "16", "e", "16"],
    ["16", ".", "16", "e", "+", "16"],
    ["16", ".", "16", "e", "-", "16"],
    ["16", "e", "16"],
    ["16", "e", "+", "16"],
    ["16", "e", "-", "16"],
]

invalid_dbls = ["Argh!"]
elements = ["16", ".", "e", "+", "-", " "]


def add_elements(n):
    arr = n * ['']
    for i in range(0, len(elements)**n):
        valid = True
        for j in reversed(range(n)):
            arr[j] = elements[i % len(elements)]
            i //= len(elements)
            if j < n - 1 and arr[j] == "16" and arr[j + 1] == "16":
                valid = False
        if not valid:
            continue
        if arr.count(" ") > 1:
            continue
        if arr.count("+") + arr.count("-") > 1:
            continue
        if arr.count(".") > 1:
            continue
        if (arr.count("+") + arr.count("-") + arr.count(".") + arr.count("e") +
                arr.count(" ")) > 2:
            continue
        if arr in valid_dbl:
            continue
        
        invalid_dbls.append(''.join(arr))
    
    
for i in range(1, 7):
    add_elements(i)


for i, data in enumerate(invalid_ints):
    n = str(i + 1)
    f1 = open('invalid-integer-' + n + '.txt', 'w+')
    print(data, file=f1)
    if i > 0:
        copyfile('invalid-integer-1.java', 'invalid-integer-' + n + '.java')
        
for i, data in enumerate(invalid_dbls):
    n = str(i + 1)
    f1 = open('invalid-double-' + n + '.txt', 'w+')
    print(data, file=f1)
    if i > 0:
        copyfile('invalid-double-1.java', 'invalid-double-' + n + '.java')

