#!/usr/bin/env python3

import argparse
import sys
import pyhilite

p = argparse.ArgumentParser(add_help=False)
p.add_argument('-i', '--input', default='STDIN', help='源代码文件，未指定为默认标准输入')
p.add_argument('-o', '--output', default='STDOUT', help='格式化代码输出文件，未指定为标准输出')
p.add_argument('-l', '--lang', default='', help='源代码的语言，标准输入必须指定，默认python')
p.add_argument('-O', '--outlang', default='html', help='输出格式，默认为HTML格式')
p.add_argument('-h', '--help', action='help', help='显示此帮助信息')

args = p.parse_args()

hilite = pyhilite.SourceHighlight(outlang=args.outlang)

if args.input == 'STDIN' and args.lang != '':
    formatted = hilite.highlight(data=sys.stdin.read(), lang=args.lang)
elif args.input != 'STDIN':
    if not args.lang:
        formatted = hilite.highlight(input=args.input)
    else:
        formatted = hilite.highlight(input=args.input, lang=args.lang)
else:
    p.print_help()
    p.exit()

if args.output == 'STDOUT':
    print(formatted)
else:
    open(args.output, 'w').write(formatted)

