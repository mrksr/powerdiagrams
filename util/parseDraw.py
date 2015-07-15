#!/usr/bin/env python

import numpy as np
import re

def parseSphere(line):
    vector = np.fromstring(line, sep=' ')
    index = vector[0]
    center = vector[1:-1]
    radius = vector[-1]

    return int(index), center, radius

def parsePoint(line):
    vector = np.fromstring(line, sep=' ')
    index = vector[0]
    point = vector[1:]

    return int(index), point

def parseEdge(line):
    point = re.compile("p(\d+)")
    sphere = re.compile("s(\d+)")
    direction = re.compile("d(.*)")

    points = []
    for match in point.finditer(line):
        points.append(int(match.group(1)))

    spheres = []
    for match in sphere.finditer(line):
        spheres.append(int(match.group(1)))

    dir = direction.search(line)
    if dir:
        dir = np.fromstring(dir.group(1), sep=' ')

    type = line[0]
    if type == 'i':
        return ("internal", points, spheres)
    elif type == 'e':
        return ("extremal", points, spheres, dir)

def parseFile(file):
    spheres = {}
    points = {}
    edges = []

    for line in file:
        if line:
            type, line = line[0], line[1:]
            if type == 's':
                index, center, radius = parseSphere(line)
                spheres[index] = (center, radius)
            elif type == 'p':
                index, point = parsePoint(line)
                points[index] = point
            elif type == 'e':
                edges.append(parseEdge(line))

    return spheres, points, edges
