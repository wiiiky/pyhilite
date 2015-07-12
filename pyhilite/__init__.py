# encoding=utf8

from . import srchilite
import os

def SourceHighlight(**kargs):
    if 'datadir' not in kargs:
        kargs['datadir'] = os.path.join(os.path.dirname(__file__), 'data')
    return srchilite.SourceHighlight(**kargs)
