#! /usr/bin/python

"""Binary Heap animation.

Generates svg files (frames) of each step of constructing end
destructing a binary heap.

Standard input is a file containing one word per line.

Standard out is the input words in alphabetical order.
"""


import math
import sys


__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"


class Header(object):
    """Emit SVG header."""

    def emit(self, out):
        out.write("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n")
        out.write("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20001102//EN\"\n")
        out.write("\"http://www.w3.org/TR/2000/CR-SVG-20001102/DTD/svg-20001102.dtd\">\n")


class Body(object):
    """Body of the SVG document being generated.

    Body contains a list of items to be emitted.
    """

    def __init__(self):
        self.items = []
        self.width="100%"
        self.height="100%"

    def emit(self, out):
        out.write("<svg width=\"%s\" height=\"%s\">\n" %
                  (self.width, self.height))
        for element in self.items:
            element.emit(out)
        out.write("</svg>\n")


class Rectangle(object):
    """Emit a standard-size box at given coordinates.

    Color can be adjusted by setting the fill and stroke fields.

    Coordinates are also used to calculate spacing and positioning of
    the tree edges.
    """

    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.width = 100;
        self.height = 50;
        self.rx = "50%"
        self.ry = "50%"
        self.stroke = "black"
        self.fill = "none"

    def top(self):
        return (self.x + 50, self.y)

    def bottom(self):
        return (self.x + 50, self.y + 50)

    def center(self):
        return (self.x + 50, self.y + 25)

    def emit(self, out):
        out.write("<rect x=\"%d\" y=\"%d\"\n" % (self.x, self.y))
        out.write("      width=\"%d\" height=\"%d\"\n" % (self.width, self.height))
        out.write("      rx=\"%s\" ry=\"%s\"\n" % (self.rx, self.ry))
        out.write("      stroke=\"%s\" fill=\"%s\" />\n" % (self.stroke, self.fill))


class Line(object):
    """Edge between parent/child nodes."""

    def __init__(self, start, end):
        self.start = start
        self.end = end

    def emit(self, out):
        out.write("<line\n")
        out.write("      x1=\"%d\" y1=\"%d\"\n" % self.start)
        out.write("      x2=\"%d\" y2=\"%d\"\n" % self.end)
        out.write("      stroke=\"black\"/>\n")


class Label(object):
    """Text label to be placed in the middle of a node (Rectangle).

    Center is a pair of (x, y) coordinates.
    """

    def __init__(self, text, center, color="black"):
        self.text = text
        (self.x, self.y) = center
        self.color = color

    def emit(self, out):
        out.write("<text \n")
        out.write("  style=\"text-anchor:middle;\"\n")
        out.write("  x=\"%d\" y=\"%d\"\n" % (self.x, self.y))
        out.write("  stroke=\"%s\" fill=\"%s\"\n" % (self.color, self.color))
        out.write("  >%s</text>\n" % self.text)


def parent(n):
    return (n - 1) // 2


def left(n):
    return 2 * n + 1


def right(n):
    return 2 * n + 2


def depth(n):
    """Tree depth (distance from root), used to calculate node spacing."""
    return int(math.log(n + 1, 2))


class Node(object):

    def __init__(self, word):
        self.word = word;
        self.color = None
        self.highlight = False


class Tree(object):
    """Binary heap (tree).

    max_nodes must be provided to the constructor to get correct
    spacing between the nodes.

    depth is the depth of the tree; height is the graphical height of
    a single node.

    node_list: tree data
    frame: counter used to generate the svg file name.
    """

    def __init__(self, max_nodes, node_height, node_width, vertical_space=None, horizontal_space=None):
        self.max_depth = int(math.log(max_nodes)) + 1
        self.node_height = node_height
        self.node_width = node_width
        if vertical_space is None:
            # vertical_space = node_height // 2;
            vertical_space = node_height
        self.vertical_space = vertical_space
        if horizontal_space is None:
            horizontal_space = node_height
        self.horizontal_space = horizontal_space
        self.node_list = []
        self.frame = 0

    def open_svg(self, basename="svg/tree-"):
        """Open a new output file for the generated SVG frame."""
        f = open("%s-%03d.svg" % (basename, self.frame), "w")
        self.frame += 1
        return f

    def emit_file(self, basename="tree-"):
        f = self.open_svg()
        self.emit(f)
        f.close()

    def sift_up(self, n):
        """See: http://en.wikipedia.org/wiki/Binary_heap."""
        self.emit_file()
        new_node = self.node_list[n]
        new_node.highlight = True
        self.emit_file()
        while n > 0:
            sys.stderr.write("  sifting up node %d (%s)\n" %
                             (n, self.node_list[n].word))
            p = parent(n)
            candidate = self.node_list[p]
            candidate.color = "red"
            # sys.stderr.write("    parent node %d (%s)\n" %
            #                  (p, candidate.word))
            # sys.stderr.write("    parent: %d (%s)\n" %
            #                  (p, self.node_list[p].word))
            self.emit_file()
            if self.node_list[n].word < self.node_list[p].word:
                sys.stderr.write("    swapping: \"%s\" and \"%s\"\n" %
                                 (self.node_list[n].word, self.node_list[p].word))
                (self.node_list[p], self.node_list[n]) = (self.node_list[n], self.node_list[p])
                # self.emit_file()
                candidate.color = None
            else:
                candidate.color = None
                break
            n = p
        new_node.highlight = False
        self.emit_file()

    def sift_down(self, n):
        """See: http://en.wikipedia.org/wiki/Binary_heap."""
        # sys.stderr.write("  sifting down node %d (%s)\n" %
        #                  (n, self.node_list[n].word))
        count = len(self.node_list)
        # sys.stderr.write("    %d nodes in tree\n" % count)
        root = self.node_list[n]
        root.highlight = True
        self.emit_file()
        while True:
            smallest = n
            child = left(n)
            if child < count and self.node_list[child].word < self.node_list[smallest].word:
                smallest = child
            child = right(n)
            if child < count and self.node_list[child].word < self.node_list[smallest].word:
                smallest = child
            if smallest == n:
                root.highlight = False
                return
            # sys.stderr.write("    swapping node %d (%s) and %d (%s)\n" %
            #                  (n, self.node_list[n].word,
            #                   smallest, self.node_list[smallest].word))
            candidate = self.node_list[smallest]
            candidate.color = "blue"
            self.emit_file()
            (self.node_list[n], self.node_list[smallest]) = (self.node_list[smallest], self.node_list[n])
            self.emit_file()
            candidate.color = None
            n = smallest

    def is_empty(self):
        return not bool(self.node_list)

    def insert(self, word):
        """Insert a "word" into the heap."""
        n = len(self.node_list)
        node = Node(word)
        self.node_list.append(Node(word))
        self.sift_up(n)

    def pop(self):
        """Remove the minimum-value "word" from the tree, restoring heapness."""
        node = self.node_list[0]
        # TODO: substitute with statement for the
        # highlight/emit/unhilight pattern.
        node.highlight = True;
        self.emit_file()
        node.highlight = False;
        if len(self.node_list) <= 1:
            self.node_list.pop()
        else:
            self.node_list[0] = self.node_list[-1]
            self.node_list.pop();
            self.emit_file()
            self.sift_down(0)
        # sys.stderr.write("popped %s\n" % node.word)
        return node

    def emit(self, out):
        """Generate SVG frame for entire tree."""
        Header().emit(out)

        body = Body()

        # Compute the size of the graphic based on the number of nodes
        # (maximum tree depth), the size of each bubble and the
        # spacing between nodes.  The height will be proportional to
        # the tree depth (plus margin on either side); the width is
        # proportional to the number of nodes on the bottom row.
        body.height = (self.max_depth + 1) * (self.node_height + self.vertical_space) + self.vertical_space
        body.width = (self.node_width + self.horizontal_space) * 2**(self.max_depth)

        # bubbles is a list of Rectangle objects; labels is a list of
        # the corresponding words.  bubbles and labels are printed
        # separately (it works out because they rely on x, y
        # coordinates for positioning
        bubbles = list()
        labels = list()

        for n, node in enumerate(self.node_list):
            sys.stderr.write("// emitting node %d (%s)\n" % (n, node.word))
            # Figure out the positioning of the node based on the
            # level (depth) and position within the level.
            current_depth = depth(n)
            sys.stderr.write("//   current depth: %d\n" % current_depth)
            # y coordinate is proportional to the current depth (plus margin).
            y = current_depth * (self.node_height + self.vertical_space) + self.vertical_space
            # x coordinate is a little more complicated.  We need to
            # determine the number of nodes in the current generation
            # to the left of the node to be printed.  x-coordinate
            # value will be proportional.  Also need to factor in the
            # padding between nodes (which depends on the number of
            # nodes in the current level, which in turn depends on the
            # depth).
            padding = (((self.node_width + self.horizontal_space) *
                        2 ** (self.max_depth - current_depth)) -
                       self.node_width)
            sys.stderr.write("//   padding: %d\n" % padding)
            generation_left = n + 1 - 2 ** current_depth
            sys.stderr.write("//   generation_left: %d\n" % generation_left)
            x = generation_left * (padding + self.node_width) + padding // 2
            sys.stderr.write("//   x: %d\n" % x)
            bubble = Rectangle(x, y)
            label = Label(node.word, bubble.center())
            if node.highlight:
                # sys.stderr.write("**** emitting node with hightlight\n")
                bubble.fill = "yellow"
                bubble.stroke = node.color or "red"
            elif node.color:
                # sys.stderr.write("**** emitting node with color %s\n" % node.color)
                bubble.stroke = node.color
                label.color = node.color
            bubbles.append(bubble)
            labels.append(label)
        body.items.extend(bubbles)
        body.items.extend(labels)
        # Calculate the edge between each node and its parent.
        for n, node in enumerate(bubbles):
            if n > 0:
                body.items.append(Line(node.top(), bubbles[parent(n)].bottom()))
        # Previous loop as a generator expression demonstrates that
        # you can go too far with that sort of thing (yes, it works,
        # try it!):
        # body.items.extend(Line(node.top(), bubbles[parent(n)].bottom())
        #                   for n, node in enumerate(bubbles) if n > 0)
        body.emit(out)


if __name__ == "__main__":
    # Need to read all words (lines) into memory first because the
    # number of nodes determines the diagram size.
    lines = sys.stdin.readlines()
    tree = Tree(len(lines), 50, 100)
    for line in lines:
        # sys.stderr.write("Inserting word %s at node %d\n" %
        #                  (line, len(tree.node_list)))
        tree.insert(line.strip())

    svg = open("data.svg", "w")
    tree.emit(svg)

    # sys.stderr.write("tree emptiness: %s" % tree.is_empty())
    while not tree.is_empty():
        node = tree.pop();
        print node.word;
