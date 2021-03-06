// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef COVERAGE_TABLE_H_INCLUDED
#define COVERAGE_TABLE_H_INCLUDED

#define PIXEL_COVERAGE(a) (((a) & 1) + (((a) >> 1) & 1) + (((a) >> 2) & 1) + (((a) >> 3) & 1) +\
                           (((a) >> 4) & 1) + (((a) >> 5) & 1) + (((a) >> 6) & 1) + (((a) >> 7) & 1))

static const unsigned char coverageTable[256] = {
    PIXEL_COVERAGE(0x00),PIXEL_COVERAGE(0x01),PIXEL_COVERAGE(0x02),PIXEL_COVERAGE(0x03),
    PIXEL_COVERAGE(0x04),PIXEL_COVERAGE(0x05),PIXEL_COVERAGE(0x06),PIXEL_COVERAGE(0x07),
    PIXEL_COVERAGE(0x08),PIXEL_COVERAGE(0x09),PIXEL_COVERAGE(0x0a),PIXEL_COVERAGE(0x0b),
    PIXEL_COVERAGE(0x0c),PIXEL_COVERAGE(0x0d),PIXEL_COVERAGE(0x0e),PIXEL_COVERAGE(0x0f),
    PIXEL_COVERAGE(0x10),PIXEL_COVERAGE(0x11),PIXEL_COVERAGE(0x12),PIXEL_COVERAGE(0x13),
    PIXEL_COVERAGE(0x14),PIXEL_COVERAGE(0x15),PIXEL_COVERAGE(0x16),PIXEL_COVERAGE(0x17),
    PIXEL_COVERAGE(0x18),PIXEL_COVERAGE(0x19),PIXEL_COVERAGE(0x1a),PIXEL_COVERAGE(0x1b),
    PIXEL_COVERAGE(0x1c),PIXEL_COVERAGE(0x1d),PIXEL_COVERAGE(0x1e),PIXEL_COVERAGE(0x1f),
    PIXEL_COVERAGE(0x20),PIXEL_COVERAGE(0x21),PIXEL_COVERAGE(0x22),PIXEL_COVERAGE(0x23),
    PIXEL_COVERAGE(0x24),PIXEL_COVERAGE(0x25),PIXEL_COVERAGE(0x26),PIXEL_COVERAGE(0x27),
    PIXEL_COVERAGE(0x28),PIXEL_COVERAGE(0x29),PIXEL_COVERAGE(0x2a),PIXEL_COVERAGE(0x2b),
    PIXEL_COVERAGE(0x2c),PIXEL_COVERAGE(0x2d),PIXEL_COVERAGE(0x2e),PIXEL_COVERAGE(0x2f),
    PIXEL_COVERAGE(0x30),PIXEL_COVERAGE(0x31),PIXEL_COVERAGE(0x32),PIXEL_COVERAGE(0x33),
    PIXEL_COVERAGE(0x34),PIXEL_COVERAGE(0x35),PIXEL_COVERAGE(0x36),PIXEL_COVERAGE(0x37),
    PIXEL_COVERAGE(0x38),PIXEL_COVERAGE(0x39),PIXEL_COVERAGE(0x3a),PIXEL_COVERAGE(0x3b),
    PIXEL_COVERAGE(0x3c),PIXEL_COVERAGE(0x3d),PIXEL_COVERAGE(0x3e),PIXEL_COVERAGE(0x3f),
    PIXEL_COVERAGE(0x40),PIXEL_COVERAGE(0x41),PIXEL_COVERAGE(0x42),PIXEL_COVERAGE(0x43),
    PIXEL_COVERAGE(0x44),PIXEL_COVERAGE(0x45),PIXEL_COVERAGE(0x46),PIXEL_COVERAGE(0x47),
    PIXEL_COVERAGE(0x48),PIXEL_COVERAGE(0x49),PIXEL_COVERAGE(0x4a),PIXEL_COVERAGE(0x4b),
    PIXEL_COVERAGE(0x4c),PIXEL_COVERAGE(0x4d),PIXEL_COVERAGE(0x4e),PIXEL_COVERAGE(0x4f),
    PIXEL_COVERAGE(0x50),PIXEL_COVERAGE(0x51),PIXEL_COVERAGE(0x52),PIXEL_COVERAGE(0x53),
    PIXEL_COVERAGE(0x54),PIXEL_COVERAGE(0x55),PIXEL_COVERAGE(0x56),PIXEL_COVERAGE(0x57),
    PIXEL_COVERAGE(0x58),PIXEL_COVERAGE(0x59),PIXEL_COVERAGE(0x5a),PIXEL_COVERAGE(0x5b),
    PIXEL_COVERAGE(0x5c),PIXEL_COVERAGE(0x5d),PIXEL_COVERAGE(0x5e),PIXEL_COVERAGE(0x5f),
    PIXEL_COVERAGE(0x60),PIXEL_COVERAGE(0x61),PIXEL_COVERAGE(0x62),PIXEL_COVERAGE(0x63),
    PIXEL_COVERAGE(0x64),PIXEL_COVERAGE(0x65),PIXEL_COVERAGE(0x66),PIXEL_COVERAGE(0x67),
    PIXEL_COVERAGE(0x68),PIXEL_COVERAGE(0x69),PIXEL_COVERAGE(0x6a),PIXEL_COVERAGE(0x6b),
    PIXEL_COVERAGE(0x6c),PIXEL_COVERAGE(0x6d),PIXEL_COVERAGE(0x6e),PIXEL_COVERAGE(0x6f),
    PIXEL_COVERAGE(0x70),PIXEL_COVERAGE(0x71),PIXEL_COVERAGE(0x72),PIXEL_COVERAGE(0x73),
    PIXEL_COVERAGE(0x74),PIXEL_COVERAGE(0x75),PIXEL_COVERAGE(0x76),PIXEL_COVERAGE(0x77),
    PIXEL_COVERAGE(0x78),PIXEL_COVERAGE(0x79),PIXEL_COVERAGE(0x7a),PIXEL_COVERAGE(0x7b),
    PIXEL_COVERAGE(0x7c),PIXEL_COVERAGE(0x7d),PIXEL_COVERAGE(0x7e),PIXEL_COVERAGE(0x7f),
    PIXEL_COVERAGE(0x80),PIXEL_COVERAGE(0x81),PIXEL_COVERAGE(0x82),PIXEL_COVERAGE(0x83),
    PIXEL_COVERAGE(0x84),PIXEL_COVERAGE(0x85),PIXEL_COVERAGE(0x86),PIXEL_COVERAGE(0x87),
    PIXEL_COVERAGE(0x88),PIXEL_COVERAGE(0x89),PIXEL_COVERAGE(0x8a),PIXEL_COVERAGE(0x8b),
    PIXEL_COVERAGE(0x8c),PIXEL_COVERAGE(0x8d),PIXEL_COVERAGE(0x8e),PIXEL_COVERAGE(0x8f),
    PIXEL_COVERAGE(0x90),PIXEL_COVERAGE(0x91),PIXEL_COVERAGE(0x92),PIXEL_COVERAGE(0x93),
    PIXEL_COVERAGE(0x94),PIXEL_COVERAGE(0x95),PIXEL_COVERAGE(0x96),PIXEL_COVERAGE(0x97),
    PIXEL_COVERAGE(0x98),PIXEL_COVERAGE(0x99),PIXEL_COVERAGE(0x9a),PIXEL_COVERAGE(0x9b),
    PIXEL_COVERAGE(0x9c),PIXEL_COVERAGE(0x9d),PIXEL_COVERAGE(0x9e),PIXEL_COVERAGE(0x9f),
    PIXEL_COVERAGE(0xa0),PIXEL_COVERAGE(0xa1),PIXEL_COVERAGE(0xa2),PIXEL_COVERAGE(0xa3),
    PIXEL_COVERAGE(0xa4),PIXEL_COVERAGE(0xa5),PIXEL_COVERAGE(0xa6),PIXEL_COVERAGE(0xa7),
    PIXEL_COVERAGE(0xa8),PIXEL_COVERAGE(0xa9),PIXEL_COVERAGE(0xaa),PIXEL_COVERAGE(0xab),
    PIXEL_COVERAGE(0xac),PIXEL_COVERAGE(0xad),PIXEL_COVERAGE(0xae),PIXEL_COVERAGE(0xaf),
    PIXEL_COVERAGE(0xb0),PIXEL_COVERAGE(0xb1),PIXEL_COVERAGE(0xb2),PIXEL_COVERAGE(0xb3),
    PIXEL_COVERAGE(0xb4),PIXEL_COVERAGE(0xb5),PIXEL_COVERAGE(0xb6),PIXEL_COVERAGE(0xb7),
    PIXEL_COVERAGE(0xb8),PIXEL_COVERAGE(0xb9),PIXEL_COVERAGE(0xba),PIXEL_COVERAGE(0xbb),
    PIXEL_COVERAGE(0xbc),PIXEL_COVERAGE(0xbd),PIXEL_COVERAGE(0xbe),PIXEL_COVERAGE(0xbf),
    PIXEL_COVERAGE(0xc0),PIXEL_COVERAGE(0xc1),PIXEL_COVERAGE(0xc2),PIXEL_COVERAGE(0xc3),
    PIXEL_COVERAGE(0xc4),PIXEL_COVERAGE(0xc5),PIXEL_COVERAGE(0xc6),PIXEL_COVERAGE(0xc7),
    PIXEL_COVERAGE(0xc8),PIXEL_COVERAGE(0xc9),PIXEL_COVERAGE(0xca),PIXEL_COVERAGE(0xcb),
    PIXEL_COVERAGE(0xcc),PIXEL_COVERAGE(0xcd),PIXEL_COVERAGE(0xce),PIXEL_COVERAGE(0xcf),
    PIXEL_COVERAGE(0xd0),PIXEL_COVERAGE(0xd1),PIXEL_COVERAGE(0xd2),PIXEL_COVERAGE(0xd3),
    PIXEL_COVERAGE(0xd4),PIXEL_COVERAGE(0xd5),PIXEL_COVERAGE(0xd6),PIXEL_COVERAGE(0xd7),
    PIXEL_COVERAGE(0xd8),PIXEL_COVERAGE(0xd9),PIXEL_COVERAGE(0xda),PIXEL_COVERAGE(0xdb),
    PIXEL_COVERAGE(0xdc),PIXEL_COVERAGE(0xdd),PIXEL_COVERAGE(0xde),PIXEL_COVERAGE(0xdf),
    PIXEL_COVERAGE(0xe0),PIXEL_COVERAGE(0xe1),PIXEL_COVERAGE(0xe2),PIXEL_COVERAGE(0xe3),
    PIXEL_COVERAGE(0xe4),PIXEL_COVERAGE(0xe5),PIXEL_COVERAGE(0xe6),PIXEL_COVERAGE(0xe7),
    PIXEL_COVERAGE(0xe8),PIXEL_COVERAGE(0xe9),PIXEL_COVERAGE(0xea),PIXEL_COVERAGE(0xeb),
    PIXEL_COVERAGE(0xec),PIXEL_COVERAGE(0xed),PIXEL_COVERAGE(0xee),PIXEL_COVERAGE(0xef),
    PIXEL_COVERAGE(0xf0),PIXEL_COVERAGE(0xf1),PIXEL_COVERAGE(0xf2),PIXEL_COVERAGE(0xf3),
    PIXEL_COVERAGE(0xf4),PIXEL_COVERAGE(0xf5),PIXEL_COVERAGE(0xf6),PIXEL_COVERAGE(0xf7),
    PIXEL_COVERAGE(0xf8),PIXEL_COVERAGE(0xf9),PIXEL_COVERAGE(0xfa),PIXEL_COVERAGE(0xfb),
    PIXEL_COVERAGE(0xfc),PIXEL_COVERAGE(0xfd),PIXEL_COVERAGE(0xfe),PIXEL_COVERAGE(0xff)
};

#endif
