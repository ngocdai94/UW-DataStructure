#! /usr/bin/python

"""Animate the construction of a 2-3 Tree.

Read words (one word per line) from stdin, constructing a 2-3 tree
containing the count of each word.  After the count is tallied, the
counts and words are written to stdout in alphabetical order, in
"diff-c" format.

As a side effect, a series of frames is written as dot files which may
be post-processed by the Graphviz dot program and imagemagick to
generate an animation sequence.

Note that the frame dump turns this elegant O(N log N) algorithm into
a clunky O(N**2) algorithm.

If a command-line single argument is provided, it is expected to be
the basename of the frame files.
"""

import collections
import sys

__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"


class BTree(object):

    class Node(object):
        """2-3 Tree nodes.

        Data is arbitrary object, but must support an __lt__ method
        for comparing elements.

        Node are highlighted as they are modified, and at strategic
        points during construction, a rendering is made using the
        maker object passed in as a parameter to the insert() method.
        """

        def __init__(self, data):
            self.data1 = data
            self.data2 = None
            self.left = None
            self.middle = None
            self.right = None
            self.hilite = False

        def __str__(self):
            # Mainly for debugging.  Need to use __repr__ for the
            # children to avoid recursing down the tree.
            return ("<Node %s: <%s> %s <%s> %s <%s>>" %
                    (self.__repr__(),
                     self.left.__repr__(),
                     self.data1,
                     self.middle.__repr__(),
                     self.data2,
                     self.right.__repr__()))

        def _isleaf(self):
            # In a well-formed 2-3 tree, interior nodes always have a
            # left child.
            return self.left is None

        def height(self, initial=0):
            # Compute height of tree (from leaf upward) using tail
            # recursion.  The accumulator passes the count of nodes
            # downward as a parameter, making the tail call the last
            # operation of the function.
            #
            # Python does not do tail call optimization, but in other
            # languages, the tail call can be tranformed into a loop
            # for efficiency.  This just demonstrates the technique.
            #
            # Only need to step down the left branch because, in a
            # well-formed 2-3 tree, all leaves are at the same depth.
            if self._isleaf():
                return initial
            return self.left.height(initial + 1)

        def _insert_leaf(self, data, maker):
            #sys.stderr.write("    _insert_leaf before: %s\n" % self)
            if self.data2 is None:
                # It fits, so figure out where to put it
                if data < self.data1:
                    self.data2 = self.data1
                    self.data1 = data
                elif self.data1 < data:
                    self.data2 = data
                else:
                    # Data compares equal: return original object.
                    data = self.data1
                new_parent = None
                self.hilite = True
                maker.make_frame()
            else:
                # Split leaf node (unless it's a duplicate).
                #sys.stderr.write("    __insert_leaf: splitting node\n")
                if data < self.data1:
                    new_parent = BTree.Node(self.data1)
                    new_sibling = BTree.Node(self.data2)
                    self.data1 = data
                    self.data2 = None
                    new_parent.left = self
                    new_parent.middle = new_sibling
                    #sys.stderr.write("    _insert_leaf new_sibling: %s\n" % new_sibling)
                elif self.data1 < data and data < self.data2:
                    new_parent = BTree.Node(data)
                    new_sibling = BTree.Node(self.data2)
                    self.data2 = None
                    new_parent.left = self
                    new_parent.middle = new_sibling
                    #sys.stderr.write("    _insert_leaf new_sibling: %s\n" % new_sibling)
                elif self.data2 < data:
                    new_parent = BTree.Node(self.data2)
                    new_sibling = BTree.Node(data)
                    self.data2 = None
                    new_parent.left = self
                    new_parent.middle = new_sibling
                    #sys.stderr.write("    _insert_leaf new_sibling: %s\n" % new_sibling)
                elif data < self.data2:
                    new_parent = None
                    data = self.data1
                else:
                    new_parent = None
                    data = self.data2
                if new_parent:
                    new_parent.hilite = True
                    new_parent.left.hilite = True
                    new_parent.middle.hilite = True
                else:
                    self.hilite = True
                    maker.make_frame()
            #sys.stderr.write("    _insert_leaf new_parent: %s\n" % new_parent)
            #sys.stderr.write("    _insert_leaf after: %s\n" % self)
            return (new_parent, data)

        def _insert_left(self, data, maker):
            #sys.stderr.write("    _insert_left before: %s \n" % self)
            (new_node, data) = self.left.insert(data, maker)
            #sys.stderr.write("        _insert_left new_node: %s\n" % new_node)
            if new_node is None:
                new_parent = None
            else:
                # Temporarily insert new_node for dumping.  It'll get
                # overwritten shortly.
                self.left = new_node
                maker.make_frame()
                if self.data2 is None:
                    new_parent = None
                    self.data2 = self.data1
                    self.data1 = new_node.data1
                    self.right = self.middle
                    self.middle = new_node.middle
                    self.left = new_node.left
                    self.hilite = True
                    maker.make_frame()
                else:
                    # Split node
                    new_parent = BTree.Node(self.data1)
                    self.data1 = self.data2
                    self.data2 = None
                    self.left = self.middle
                    self.middle = self.right
                    self.right = None
                    new_parent.left = new_node
                    new_parent.middle = self
                    new_parent.hilite = True
                    new_parent.left.hilite = True
                    new_parent.middle.hilite = True
                    #sys.stderr.write("        _insert_left new_parent: %s\n" % new_parent)
            #sys.stderr.write("    _insert_left after: %s \n" % self)
            return (new_parent, data)

        def _insert_middle(self, data, maker):
            #sys.stderr.write("    _insert_middle before: %s\n" % self)
            (new_node, data) = self.middle.insert(data, maker)
            #sys.stderr.write("        _insert_middle new_node: %s\n" % new_node)
            if new_node is None:
                new_parent = None
            else:
                # Temporarily insert new_node for dumping.  It'll get
                # overwritten shortly.
                self.middle = new_node
                maker.make_frame()
                if self.data2 is None:
                    new_parent = None
                    self.data2 = new_node.data1
                    self.middle = new_node.left
                    self.right = new_node.middle
                    self.hilite = True
                    maker.make_frame()
                else:
                    # Split node
                    new_parent = new_node
                    new_sibling = BTree.Node(self.data2)
                    self.data2 = None
                    new_sibling.middle = self.right
                    new_sibling.left = new_parent.middle
                    self.right = None
                    self.middle = new_parent.left
                    new_parent.left = self
                    new_parent.middle = new_sibling
                    new_parent.hilite = True
                    new_parent.left.hilite = True
                    new_parent.middle.hilite = True
                    #sys.stderr.write("        _insert_middle new_sibling %s\n" % new_sibling)
            #sys.stderr.write("        _insert_middle new_parent: %s\n" % new_parent)
            #sys.stderr.write("    _insert_middle after: %s\n" % self)
            return (new_parent, data)

        def _insert_right(self, data, maker):
            #sys.stderr.write("    _insert_right before: %s\n" % self)
            (new_node, data) = self.right.insert(data, maker)
            if new_node is None:
                new_parent = None
            else:
                # Temporarily insert new_node for dumping.  It'll get
                # overwritten shortly.
                self.right = new_node
                maker.make_frame()
                # Split node
                new_parent = BTree.Node(self.data2)
                self.data2 = None
                self.right = None
                new_parent.left = self
                new_parent.middle = new_node
                new_parent.hilite = True
                new_parent.left.hilite = True
                new_parent.middle.hilite = True
                #sys.stderr.write("    _insert_right new_node: %s\n" % new_node)
                #sys.stderr.write("    _insert_right new_parent: %s\n" % new_parent)
            #sys.stderr.write("    _insert_right after: %s\n" % self)
            return (new_parent, data)

        def insert(self, data, maker):
            """Insert data into the 2-3 tree.

            Recurse down to the appropriate leaf node to insert data
            (unless found somewhere in the path).

            Returns tuple (new_node, data) where data is the original
            data if added to the tree, or the equal-value object found
            in the tree.  new_node is the overflow information passed
            from the child back to the parent in the case of a node
            split.
            """
            #sys.stderr.write("    insert: %s\n" % self)
            assert self.data1 is not None
            assert (((self.left is None) and (self.middle is None) and (self.right is None)) or
                    ((self.left is not None) and (self.middle is not None)))
            assert (((self.data2 is None) and (self.right is None)) or
                    ((self.data2 is not None) and ((self.right is None) == (self.left is None))))
            if self._isleaf():
                result = self._insert_leaf(data, maker)
            elif data < self.data1:
                result = self._insert_left(data, maker)
            elif self.data1 < data and (self.data2 is None or data < self.data2):
                result = self._insert_middle(data, maker)
            elif self.data2 and self.data2 < data:
                result = self._insert_right(data, maker)
            elif self.data1 < data:
                # data == data2
                self.hilite - True
                result = (None, self.data2)
            else:
                # data == data1
                self.hilite - True
                result = (None, self.data1)
            return result


        def walk(self, action):
            """Tree traversal.

            Perform action on every data object in the tree, in sorted
            order.  This is a modified inorder tree traversal.
            """
            if self.left:
                self.left.walk(action)
            if self.data1:
                # sys.stdout.write("Node(0x%x) " % id(self))
                action(self.data1)
            if self.middle:
                self.middle.walk(action)
            if self.data2:
                # sys.stdout.write("Node(0x%x) " % id(self))
                action(self.data2)
            if self.right:
                self.right.walk(action)

        def name(self):
            """Return string representing node name."""
            # Don't use __str__ because we want to save that method
            # for debugging output.
            label1 = "%s" % self.data1
            if self.data2:
                label2 = "| %s" % self.data2
            else:
                label2 = ""
            return "".join([label1, label2])

        def dot_fmt_node(self):
            """Return a string containg dot-language syntax for node graphic."""
            if self.hilite:
                hilite_code = ", style=filled, fillcolor=yellow"
                self.hilite = False
            else:
                hilite_code = ""
            format_list = ["\"%s\" " % (self.name(),),
                           "[",
                           "shape=Mrecord"
                           ", label=\"%s\"" % (self.name(),),
                           hilite_code,
                           "];"]
            return "".join(format_list)


    def __init__(self):
        self.root = None

    def is_empty(self):
        return self.root is None

    def height(self):
        # Iterative calculation of tree height.  Contrast this with
        # the tail-recursive Node.height() method.
        node = self.root
        n = -1
        while node:
            n += 1
            node = node.left
        return n

    def insert(self, data, maker):
        """Insert data into 2-3 tree.

        maker is an object containing a make_frame method for
        generating a graphic (frame) of the tree at various points
        during constructin.

        Returns original data if added to the tree, or equal-value
        data found in the tree.
        """
        sys.stderr.write("**** Inserting %s\n" % data.word)
        if self.root is None:
            self.root = BTree.Node(data)
            self.root.hilite = True
            maker.make_frame()
        else:
            (new_root, data) = self.root.insert(data, maker)
            #sys.stderr.write("**** Data Inserted: (%s, %s)\n" % (new_root, data))
            if new_root is not None:
                self.root = new_root
                maker.make_frame()
        return data

    def walk(self, action):
        if self.root:
            self.root.walk(action)

    def bylevel(self, action):
        """Perform action on each node in top-to-bottom order."""
        # Breadth-first traversal.
        if self.is_empty():
            return
        worklist = collections.deque()
        worklist.append(self.root)
        while True:
            try:
                node = worklist.popleft()
            except IndexError:
                return
            action(node)
            if node.left:
                worklist.append(node.left)
            if node.middle:
                worklist.append(node.middle)
            if node.right:
                worklist.append(node.right)



class WordCounter(object):
    """Object to hold a word and its count."""
    # This interface is not especially pythonic (contrast this with
    # the dictionary-based implementation of wordcount).  The
    # trade-off is simplicity of the implementation for complexity of
    # the interface, which is generally a bad bargain.
    #
    # The choice here is to demonstrate the implementation.

    def __init__(self, word):
        self.word = word
        self.count = 0

    def __lt__(self, other):
        return self.word < other.word

    def increment(self):
        self.count += 1
        return self

    def __str__(self):
        # String format is intended for the node label.
        return str("%s (%s)" % (self.word, self.count))

    def emit(self):
        """Action function for tree walker.

        Print count and word in "diff -c" format.
        """
        print  "%7d %s" % (self.count, self.word)




class DotMaker(object):
    """Generate dot-language files during tree construction."""

    class Dot(object):

        def __init__(self, tree, out=None):
            self.tree = tree
            self.out = out or sys.stdout

        def generate(self):
            def emit_node(node):
                self.out.write("   %s\n" % node.dot_fmt_node())

            def emit_links(node):
                if node.left:
                    self.out.write("   \"%s\" -> \"%s\" [label=\"l\"];\n" % (node.name(), node.left.name()))
                if node.middle:
                    self.out.write("   \"%s\" -> \"%s\" [label=\"m\"];\n" % (node.name(), node.middle.name()))
                if node.right:
                    self.out.write("   \"%s\" -> \"%s\" [label=\"r\"];\n" % (node.name(), node.right.name()))

            self.out.write("digraph {\n")
            self.tree.bylevel(emit_node)
            self.tree.bylevel(emit_links)
            self.out.write("}\n");

    def __init__(self, tree, basename="frame-"):
        self.basename = basename
        self.count = 0
        self.tree = tree

    def make_frame(self):
        self.count += 1
        out = open("%s%03d.dot" % (self.basename, self.count), "w")
        DotMaker.Dot(self.tree, out).generate()


if __name__ == "__main__":
    if len(sys.argv) == 2:
        frame_base = sys.argv[1]
    else:
        frame_base = "out/frame-"

    tree = BTree()
    maker = DotMaker(tree, frame_base)

    for line in sys.stdin:
        #sys.stderr.write("#### inserting %s\n" % line[:-1])
        tree.insert(WordCounter(line.strip()), maker).increment()
        maker.make_frame();
    # sys.stdout.write("#### final output\n")
    tree.walk(WordCounter.emit)
