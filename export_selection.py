#!/usr/bin/python
import argparse
import os
from tulip import *

def export_graph(f, prop, out):
    g = tlp.loadGraph(f)

    path, pattern = os.path.split(os.path.realpath(out))

    selection = g.getBooleanProperty(prop)

    ds = tlp.getDefaultPluginParameters('Export image', g)
    ds['Property']              = selection
    ds['dir::Export directory'] = path
    ds['Export pattern']        = pattern
    result, error = g.applyAlgorithm('Export image', ds)
    if result == False:
        print error
    print 'Export done'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Export a BooleanProperty as a binary image.',
            epilog='Add <tulip install dir>/lib/python to the PYTHONPATH environment variable, and <tulip install dir>/lib to the LD_LIBRARY_PATH one.')
    parser.add_argument('--graph', '-g', metavar='file', help='the graph to load', required = True)
    parser.add_argument('--property', '-p', metavar='property', help='the property to export', default = 'viewSelection')
    parser.add_argument('--output', '-o', metavar='file', help='where the segmentation result will be saved', required = True)
    args = parser.parse_args()

    export_graph(args.graph, args.property, args.output)
