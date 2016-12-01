#!/usr/bin/env python3
# coding=utf-8

from sys import argv, exit

# Information about types of tokens:

tokens = [
    {'name': "+",  'p': 4},
    {'name': "-",  'p': 4},
    {'name': "/",  'p': 3},
    {'name': "*",  'p': 3},
    {'name': "==", 'p': 7},
    {'name': "!=", 'p': 7},
    {'name': "<",  'p': 6},
    {'name': ">",  'p': 6},
    {'name': "<=", 'p': 6},
    {'name': ">=", 'p': 6},
    {'name': "&&", 'p': 10},
    {'name': "||", 'p': 11},
    {'name': "!",  'p': 2},
    {'name': "(",  'p': 0},
    {'name': ")",  'p': 0},
    {'name': "i",  'p': 0},
    {'name': "$",  'p': 0},
]

assoc = {
    2: '<',
    3: '>',
    4: '>',
    6: '',
    7: '',
    10: '>',
    11: '>'
}

'''
Ruční dodefinování některých buňek tabulky. Buňky, které nejsou vyplněny podle
priority či asociativity budou vyplněny prvním vyhovujícím pravidlem níže.

Každé pravidlo je tvořeno trojicí, kde první položka obsahuje řádek tabulky
(symbol na zásobníku), druhá sloupeček tabulky (čtený token) a třetí obsah této
buňky. Pokud je řádek nebo sloupce prázdný, budou jím vyplněny všechny řádky
nebo sloupce.
'''
other_rules = [
    ('i', 'i', '' ),
    ('i', '(', '' ),
    ('i', '' , '>'),
    (')', 'i', '' ),
    ('',  'i', '<'),
    ('(', ')', '='),
    ('(', '$', '' ),
    ('(', '',  '<'),
    (')', '(', '' ),
    (')', 'i', '' ),
    (')', '',  '>'),
    ('i', '(', '' ),
    ('',  '(', '<'),
    ('$', ')', '' ),
    ('',  ')', '>'),
    ('$', '$', '' ),
    ('$', '' , '<'),
    ('' , '$', '>'),
]

md_values = {
    '>': '&gt;',
    '<': '__&lt;__',
    '' : '',
    '=': '='
}

c_values = {
    '>': 'PREC_GREATER',
    '<': 'PREC_LOWER',
    '' : 'PREC_INVALID',
    '=': 'PREC_EQUAL'
}


# Parse sys.argv:
if len(argv) != 2:
    print("Usage: " + argv[0] + " MODE\n\nModes:")
    print("* txt - Prints text output")
    print("* md  - Prints Markdown output (for wiki page)")
    print("* c   - Prints C output (can be used in the source)")
    exit(1)
mode = argv[1]
if mode not in ["txt", "md", "c"]:
    print("Error: Invalid mode {}".format(mode))
    exit(1)


def print_header():
    if mode in ["txt", "md"]:
        out = "    "
        for t in tokens:
            out += "| " + t['name'].ljust(2)

        print(out)
        print("----{}".format(
            "{}---".format('|' if mode == 'md' else '+') * len(tokens))
        )
    else:
        print("T_precedence_state precedence_table[] = {")

def start_row(stack):
    if mode == "txt":
        out = " {:3}".format(stack['name'])
    elif mode == "md":
        out = "__{}__".format(stack['name'])
    else:
        out = "    "
    print(out, end="")

def get_result(stack, scanner):
    if stack['p'] > 0 and scanner['p'] > 0:
        if stack['p'] > scanner['p']:
            return '<'
        if stack['p'] < scanner['p']:
            return '>'

        # Stejna priorita
        return assoc[stack['p']]

    for st, sc, res in other_rules:
        if st in ["", stack['name']] and sc in ["", scanner['name']]:
            return res

    return '??'

def print_cell(stact, token_i, scanner):
    out = get_result(stack, scanner)
    if mode == 'txt':
        print('| {:2}'.format(out), end="")
    if mode == 'md':
        print('|{}'.format(md_values[out]), end="")
    if mode == 'c':
        endl = "\n    " if token_i % 5 == 4 else ""
        print('{}, '.format(c_values[out]), end=endl)

def end_row(last):
    if mode == 'c' and not last:
        print()
    print()

def print_footer():
    if mode == 'c':
        print('};')

print_header()

for r_id, stack in enumerate(tokens):
    start_row(stack)
    for i, token in enumerate(tokens):
        print_cell(stack, i, token)
    end_row(r_id == len(tokens) - 1)

print_footer()
