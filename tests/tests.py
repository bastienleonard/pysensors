#! /usr/bin/env python2
# -*- coding: utf-8 -*-

import unittest

import sensors


class TestChipName(unittest.TestCase):
    def test_equals(self):
        chip_name = sensors.ChipName()
        self.assertEqual(chip_name, chip_name)
        self.assertEqual(sensors.ChipName(), sensors.ChipName())
        self.assertEqual(sensors.ChipName('prefix', 10, 2, 15, 'path'),
                         sensors.ChipName('prefix', 10, 2, 15, 'path'))

        for chip_name in sensors.get_detected_chips():
            self.assertEqual(chip_name, chip_name)

        for c1, c2 in zip(sensors.get_detected_chips(),
                          sensors.get_detected_chips()):
            self.assertEqual(c1, c2)

if __name__ == '__main__':
    unittest.main()
