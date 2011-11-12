#! /usr/bin/env python2
# -*- coding: utf-8 -*-

import sensors
from gi.repository import Gtk as gtk


class Window(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, title='Sensors information',
                            default_width=800, default_height=500)
        self.connect('destroy', gtk.main_quit)
        self.store = gtk.TreeStore(str, str, str)

        for chip in sensors.get_detected_chips():
            chip_parent = self.store.append(None, [str(chip), '', repr(chip)])

            for feature in chip.get_features():
                feature_parent = self.store.append(
                    chip_parent, [chip.get_label(feature), '', repr(feature)])

                for subfeature in chip.get_all_subfeatures(feature):
                    self.store.append(
                        feature_parent,
                        [subfeature.name,
                         '{0:.2f}'.format(chip.get_value(subfeature.number)),
                                       repr(subfeature)])

        self.tree = gtk.TreeView(self.store)
        self.tree.set_search_column(0)
        self.tree.expand_all()
        renderer = gtk.CellRendererText()
        name_column = gtk.TreeViewColumn('Name', renderer, text=0)
        self.tree.append_column(name_column)
        name_column.set_resizable(True)
        value_column = gtk.TreeViewColumn('Value', renderer, text=1)
        self.tree.append_column(value_column)
        value_column.set_resizable(True)
        info_column = gtk.TreeViewColumn('Additional info', renderer, text=2)
        self.tree.append_column(info_column)
        s = gtk.ScrolledWindow()
        self.add(s)
        s.add(self.tree)
        self.show_all()


def main():
    window = Window()
    gtk.main()


if __name__ == '__main__':
    try:
        main()
    finally:
        sensors.cleanup()
