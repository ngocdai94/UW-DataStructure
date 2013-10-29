#! /usr/bin/python
#
# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

"""Animate the construction of a red-black tree.

Usage: red-black.py [--norebalance|--rebalance] [files...]

If --norebalance is specified, the tree becomes an ordinary
(unbalanced) binary search tree.  Default is --rebalance.

Run this program with random data (from a text), sorted and
reverse-sorted data, and almost-sorted data (see permute5.py).

If no file arguments are given, stdin is used for data.

Output is the words in sorted order. This program does not bother
counting the repetitions of each word.

A sequence of dot-language files is generated showing the tree at
various stage of construction.
"""


import collections
import os
import re
import sys


__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"


class Error(object):
    pass


class SyntheticLeaf(object):

    def __init__(self):
        self.data = "leaf"
        self.parent = None
        self.left = None
        self.right = None
        self.color = "black"

    def is_leaf(self):
        return True

    def insert(self, data, _tree):
        """Inserting data into a synthetic leaf.

        Per the red-black algorithm, replace the current leaf node
        with a new red node containing the data (and two
        synthetic-leaf children).
        """
        parent = self.parent
        node = Node(data, parent, self, SyntheticLeaf())
        if parent.left == self:
            parent.left = node
        else:
            parent.right = node
        node.left.parent = node
        node.right.parent = node
        return node

    def inorder(self, fn):
        # Placehoder because it will be called by its parent's inorder
        # invocation.
        pass

    def name(self):
        return "%s" % self

    def dot_fmt_node(self):
        return "\"%s\" [label=\"\", fixedsize=true, width=0.2, height=0.2, style=filled, fillcolor=black];" % self


class Node(object):

    def __init__(self, data, parent=None, left=None, right=None):
        self.data = data
        self.parent = parent
        self.left = left
        if left:
            self.left.parent = self
        self.right = right
        if right:
            self.right.parent = self
        self.color = "red"
        self.hilite = True

    def sibbling(self):
        if self.parent:
            if self == self.parent.left:
                return self.parent.right
            else:
                return self.parent.left
        return None

    def grandparent(self):
        if self.parent:
            return self.parent.parent
        return None

    def uncle(self):
        grandparent = self.grandparent()
        if grandparent:
            if self.parent == grandparent.left:
                return grandparent.right
            else:
                return grandparent.left
        return None

    def is_leaf(self):
        return False

    def is_black(self):
        return self.color == "black"

    def is_red(self):
        return self.color == "red"

    def toggle(self):
        if self.color == "red":
            self.color = "black"
        elif self.color == "black":
            self.color = "red"
        else:
            # TODO: proper error class
            raise Error()

    def rotate_right(self):
        if self.left is None:
            # TODO: proper error class
            raise Error()
        child = self.left
        old_right = child.right
        child.right = self
        self.parent = child
        self.left = old_right
        old_right.parent = self
        return child

    def rotate_left(self):
        if self.right is None:
            # TODO: proper error class
            raise Error()
        child = self.right
        old_left = child.left
        child.left = self
        self.parent = child
        self.right = old_left
        old_left.parent = self
        return child

    def rebalance(self, tree):
        if not self.parent:
            self.color = "black"
            tree.make()
            return None

        parent = self.parent
        if parent.color == "black":
            return None

        # Parent is red, so it cannot be the current root.
        # Grandparent must be black.
        grandparent = parent.parent
        uncle = self.uncle()
        if uncle.color == "red":
            # grandparent must be black
            grandparent.color = "red"
            parent.color = "black"
            uncle.color = "black"
            tree.make()
            return grandparent.rebalance(tree)

        # Uncle is black.

        if parent == grandparent.left and self == parent.right:
            # left-right: rotate to left-left
            grandparent.left = parent.rotate_left()
            grandparent.left.parent = grandparent
            (self, parent) = (parent, self)
            tree.make()
        elif parent == grandparent.right and self == parent.left:
            # right-left: rotate to right-right
            grandparent.right = parent.rotate_right()
            grandparent.right.parent = grandparent
            (self, parent) = (parent, self)
            tree.make()

        great_grandparent = grandparent.parent
        grandparent.color = "red"
        parent.color = "black"
        if parent == grandparent.left:
            # left-left
            new_grandparent = grandparent.rotate_right()
        else:
            # right-right
            new_grandparent = grandparent.rotate_left()
        if great_grandparent is None:
            new_grandparent.parent = None
            return new_grandparent

        new_grandparent.parent = great_grandparent
        if grandparent == great_grandparent.left:
            great_grandparent.left = new_grandparent
        else:
            great_grandparent.right = new_grandparent
        tree.make()
        return None

    def insert(self, data, tree):
        if data == self.data:
            self.hilite = True
            tree.make()
            return None
        if data < self.data:
            return self.left.insert(data, tree)
        else:
            return self.right.insert(data, tree)

    def inorder(self, fn):
        if self.left:
            self.left.inorder(fn)
        fn(self.data)
        if self.right:
            self.right.inorder(fn)

    def name(self):
        return self.data

    def dot_fmt_node(self):
        if self.hilite:
            hilite = ", style=filled, fillcolor=yellow"
        else:
            hilite = ""
        node_string = "\"%s\" [label=\"%s\", color=%s%s];" % (self.data, self.data, self.color, hilite)
        self.hilite = False
        return node_string


class Tree(object):

    def __init__(self, maker, do_rebalance=True):
        self.root = None
        self.maker = maker
        self.do_rebalance = do_rebalance

    def make(self):
        self.maker.make(self)

    def insert(self, data):
        if not self.root:
            self.root = Node(data, None, SyntheticLeaf(), SyntheticLeaf())
            self.root.color = "black"
        else:
            inserted = self.root.insert(data, self)
            if inserted:
                self.make()
                if self.do_rebalance:
                    new_root = inserted.rebalance(self)
                    if new_root:
                        self.root = new_root
                    self.make()

    def inorder(self, fn):
        if self.root:
            self.root.inorder(fn)

    def bylevel(self, fn):
        """Breadth-first (level order) traversal of the nodes."""
        if not self.root:
            return
        worklist = collections.deque()
        worklist.append(self.root)
        while True:
            try:
                node = worklist.popleft()
            except IndexError:
                return
            fn(node)
            if node.left:
                worklist.append(node.left)
            if node.right:
                worklist.append(node.right)


class DotMaker(object):

    class Dot(object):

        def __init__(self, tree, out=None):
            if not out:
                out = sys.stdout
            self.tree = tree
            self.out = out

        def generate(self):
            def emit_links(node):
                if node.left:
                    self.out.write("   \"%s\" -> \"%s\";\n" % (node.name(), node.left.name()))
                if node.right:
                    self.out.write("   \"%s\" -> \"%s\";\n" % (node.name(), node.right.name()))
            def emit_parent_links(node):
                if node.parent:
                    self.out.write("   \"%s\" -> \"%s\" [color=green];\n" % (node.name(), node.parent.name()))
            self.out.write("digraph {\n")
            self.tree.bylevel(lambda node: self.out.write("   %s\n" % node.dot_fmt_node()))
            self.tree.bylevel(emit_links)
            self.tree.bylevel(emit_parent_links)
            self.out.write("}\n");

    def __init__(self, basename="frame-"):
        self.basename = basename
        self.count = 0

    def make(self, tree):
        self.count += 1
        out = open("%s%03d.dot" % (self.basename, self.count), "w")
        dot = DotMaker.Dot(tree, out)
        dot.generate()


def run(basename=None, do_rebalance=True, source=None):
    if not basename:
        try:
            os.mkdir("out")
        except OSError:
            # Don't care if the directory already exists.  If it
            # doesn't and we can't create it here, opening the output
            # file will fail.
            pass
        basename = "out/frame-"
    source = source or sys.stdin
    tree = Tree(DotMaker(basename), do_rebalance)
    for line in source:
        for word in re.split("[^A-Za-z]+", line.strip()):
            if word:
                tree.insert(word)
                tree.make()
    tree.inorder(lambda word: sys.stdout.write("%s\n" % word))


def usage():
    sys.stderr.write("Usage: ")
    sys.stderr.write(sys.argv[0])
    sys.stderr.write(" [--norebalance|--rebalance] [files...]\n")
    sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        do_rebalance = True
        if sys.argv[1].startswith('-'):
            if sys.argv[1] == '--norebalance':
                do_rebalance = False
            elif sys.argv[1] == "--rebalance":
                pass
            else:
                usage()
            args = sys.argv[2:]
        else:
            args = sys.argv[1:]
        if args:
            for arg in args:
                basename = arg + ["-unbalanced", "-balanced"][do_rebalance] + "-frame"
                with open(arg, "r") as f:
                    run(basename=basename, do_rebalance=do_rebalance, source=f)
        else:
            run(do_rebalance=do_rebalance)
    else:
        run()
