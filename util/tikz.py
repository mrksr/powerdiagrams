#!/usr/bin/env python
import fileinput
import numpy as np

from parseDraw import parseFile

def printArray(arr):
    strings = map(lambda x: "{:8.4}".format(x), arr)
    return ",".join(strings)

# I assume 2d here
def tikz(spheres, points, edges):
    def printInternal(pts, sphrs):
        [a, b] = pts
        caption = ", ".join(map(str, sphrs))
        print(
            "\\draw[internal edge] (p{}) edge node[internal spheres] {{${{{}}}$}} (p{});".format(
                a,
                caption,
                b
            ))

    def printExternal(pts, sphrs, direction):
        [a] = pts
        caption = ", ".join(map(str, sphrs))
        print(
            "\\draw[extremal edge] (p{0}) edge node[extremal spheres] {{${{{1}}}$}} ($(p{0}) + 10*({2})$);".format(
                a,
                caption,
                printArray(direction)
            ))

    print("% \\path[clip, use as bounding box] (???) rectangle (???);")
    print()

    for index, sphere in spheres.items():
        center, radius = sphere
        print("\\node[sphere center] (s{0}) at ({1}) {{${0}$}};".format(index, printArray(center)))
        print("\\draw[sphere radius] (s{}) circle[radius={}];".format(index, radius))

    print()

    for index, point in points.items():
        print("\\coordinate (p{1}) at ({0});".format(printArray(point), index))

    print()

    for edge in edges:
        if edge[0] == 'internal':
            printInternal(*edge[1:])
        else:
            printExternal(*edge[1:])

    print()

    for index, point in points.items():
        print("\\node[point] () at (p{}) {{}};".format(index))

if __name__ == '__main__':
    tikz(*parseFile(fileinput.input()))
