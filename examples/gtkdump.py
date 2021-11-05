#! /usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2011, 2021 Bastien Léonard. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

#    1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.

#    2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.

# THIS SOFTWARE IS PROVIDED BY BASTIEN LÉONARD ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BASTIEN LÉONARD OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

import sensors
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk as gtk
from gi.repository import GObject as gobject


class Window(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, title='Sensors information',
                            default_width=800, default_height=500)
        self.connect('destroy', gtk.main_quit)
        self.store = gtk.TreeStore(str, str, str)
        self.update_store()
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

        def update():
            self.store.clear()
            self.update_store()
            self.tree.expand_all()
            return True

        gobject.timeout_add(1000, update)

    def update_store(self):
        for chip in sensors.get_detected_chips():
            chip_parent = self.store.append(None, [str(chip), '', repr(chip)])

            for feature in chip.get_features():
                feature_parent = self.store.append(
                    chip_parent, [chip.get_label(feature), '', repr(feature)])

                for subfeature in chip.get_all_subfeatures(feature):
                    self.store.append(
                        feature_parent,
                        [
                            subfeature.name,
                            '{0:.2f}'.format(
                                chip.get_value_or_none(subfeature.number)
                            ),
                            repr(subfeature)
                        ]
                    )


def main():
    window = Window()
    gtk.main()


if __name__ == '__main__':
    try:
        main()
    finally:
        sensors.cleanup()
