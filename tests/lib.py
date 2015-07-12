# encoding=utf8

from unittest import TestCase, main as unittest_main
import pyhilite


class PyHiLiteTests(TestCase):

    def setUp(self):
        pass

    def test_esc(self):
        h = pyhilite.SourceHighlight(outlang='esc')
        data = open('tests/lib.py', 'r').read()
        formatted = h.highlight(data=data, lang='python')
        self.assertTrue(formatted)
        formatted = h.highlight(input='tests/lib.py')
        self.assertTrue(formatted)


if __name__ == '__main__':
    unittest_main()
