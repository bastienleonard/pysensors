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


class TestFeature(unittest.TestCase):
    def test_equals(self):
        feature = sensors.Feature()
        self.assertEqual(feature, feature)
        self.assertEqual(sensors.Feature(), sensors.Feature())
        c = sensors.get_detected_chips()[0]

        for feature in c.get_features():
            self.assertEqual(feature, feature)

        for f1, f2 in zip(c.get_features(), c.get_features()):
            self.assertEqual(f1, f2)


class TestSubfeature(unittest.TestCase):
    def test_equals(self):
        subfeature = sensors.Subfeature()
        self.assertEqual(subfeature, subfeature)
        self.assertEqual(sensors.Subfeature(), sensors.Subfeature())

        c = sensors.get_detected_chips()[0]
        subfeatures = c.get_all_subfeatures(c.get_features()[0])

        for s1, s2 in zip(subfeatures, list(subfeatures)):
            self.assertEqual(s1, s2)


if __name__ == '__main__':
    unittest.main()
