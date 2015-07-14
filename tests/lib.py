# encoding=utf8

from unittest import TestCase, main as unittest_main
import pyhilite


class PyHiLiteTests(TestCase):

    def setUp(self):
        pass

    def test_esc(self):
        h = pyhilite.SourceHighlight(outlang='esc')
        with open('tests/lib.py', 'r') as f:
            data = f.read()
            formatted = h.highlight(data=data, lang='python')
            self.assertTrue(formatted)
            formatted = h.highlight(input='tests/lib.py')
            self.assertTrue(formatted)

    def test_checkLangDef(self):
        h = pyhilite.SourceHighlight()
        self.assertTrue(h.checkLangDef('html'))
        self.assertTrue(h.checkLangDef('python'))
        self.assertTrue(h.checkLangDef('cpp'))
        self.assertTrue(h.checkLangDef('log.lang'))
        self.assertTrue(h.checkLangDef('todo.lang'))
        self.assertTrue(h.checkLangDef('xml'))
        self.assertTrue(h.checkLangDef('xorg'))
        self.assertFalse(h.checkLangDef('invalid_lang'))

    def test_checkOutLangDef(self):
        h = pyhilite.SourceHighlight(outlang='esc')
        self.assertTrue(h.checkOutLangDef('esc'))
        self.assertTrue(h.checkOutLangDef('html'))
        self.assertTrue(h.checkOutLangDef('css_common.outlang'))
        self.assertTrue(h.checkOutLangDef('xhtml.outlang'))
        self.assertTrue(h.checkOutLangDef('html_ref'))
        self.assertTrue(h.checkOutLangDef('texinfo.outlang'))
        self.assertTrue(h.checkOutLangDef('latex'))
        self.assertTrue(h.checkOutLangDef('javadoc'))
        self.assertFalse(h.checkOutLangDef('invalid_outlang'))


if __name__ == '__main__':
    unittest_main()
