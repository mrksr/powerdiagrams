class PowerDiagram:
    """Builds a power diagram represented as an incidence lattice from a set of
    spheres.

    A power diagram can be defined using a set of spheres in R^d. These spheres
    uniquely define a cell complex in R^d where every point x is attributed to the
    sphere with highest pull
        pow(x, (center, radius)) = |x - center|^2 - radius^2
    To find these attributions efficiently, only the adjacency-structure of the
    different resulting polytopes has to be determined.
    To achieve this, the spheres are mapped to specific hyperplanes in R^(d+1)
    whose intersections define the power lines of the spheres. To filter the
    power lines superseded by more powerful spheres, only those projected to the
    surface of a certain hull are considered.
    Those remaining power lines are represented as an incidence lattice and form
    an alternative characterization of power diagrams.
    """
    def convertToLattice(self, spheres):
        def poles(spheres):
            def pole(sphere):
                (center, radius) = sphere
                pole[1:d] = center
                pole[d+1] = center**2 - radius**2

            return [pole(sphere) for sphere in spheres]

        def convexHull(points):
            return magic(points)

        def separate(hull):
            top, bottom = IncidenceLattice()

            for facet in hull.facets():
                plane = hull.containingHyperplane(facet)
                # TODO: How to formalize this?
                if plane.isRisingTowardsAxis(d+1):
                    top.add(facet)
                else:
                    bottom.add(facet)

            return (top, bottom)

        def replaceByDual(lattice):
            def polar(hyperplane):
                #TODO: How to get this in a good manner?
                # If we have a nice x_{d+1) = a_1 x_1 + ... representation,
                # this is not a problem. Otherwise this might be a bit ugly?
                return polarProjection(hyperplane)

            result = lattice.revertEdges()

            for facet in lattice.facets():
                coordinates = polar(lattice.containingHyperplane(facet))
                result.vertexCorrespondingTo(facet).coordinates = coordinates

            return result

        def projectToH0(bottomDual):
            for vertex in lattice.vertices():
                vertex.coordinates.deleteDimension(d+1)

            return powerDiagram

        d = dimensionality(spheres)
        # poles are points
        poles = poles(spheres)
        # hull is an IncidenceLattice
        hull = convexHull(points)
        # top, bottom are IncidenceLattices
        (top, bottom) = separate(hull)

        # bottom for power diagram, top for maximal power diagram
        bottomDual = replaceByDual(bottom)
        powerDiagram = projectToH0(bottomDual)

        return powerDiagram


class IncidenceLattice:
    """ A DAG storing incidences of a polytope.

    An incidence Lattice is a DAG storing the incidence relationships of a
    polytope. Two faces A and B are incident if
        - dim(A) = dim(B) - 1
        - B is contained in A
    To give the polytope a position, the coordinates of it's 0-faces (or points)
    can be stored.
    """
    def facets(self):
        return leafs

    def vertices(self):
        return roots

    def containingHyperplane(facet):
        return Hyperplane(verticesOf(facet))
