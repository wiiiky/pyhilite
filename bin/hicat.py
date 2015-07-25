#!/usr/bin/env python3
# 能高亮代码的cat


import pyhilite
import argparse


parser = argparse.ArgumentParser(add_help=False)
parser.add_argument('-n', '--number', action='store_true', help='是否显示行号')
parser.add_argument('files', metavar='file', nargs='+', help='文件名')
parser.add_argument('-h', '--help', action='help', help='显示此帮助信息')

args = parser.parse_args()

hilite = pyhilite.SourceHighlight(outlang='esc')
if args.number:
    hilite.setGenerateLineNumbers(True)


def cat(filename):
    try:
        print(hilite.highlight(input=filename))
    except ValueError as e:
        with open(filename) as f:
            print(f.read())

n = len(args.files)
if n == 1:
    cat(args.files[0])
else:
    for i in range(0, n):
        f = args.files[i]
        print('%s:' % f)
        cat(f)
        if i < n - 1:
            print()
