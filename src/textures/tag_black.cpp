#include "../texture.hpp"

byte tagBlackBytes[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1e, 0x3f, 0x88, 0xb1, 0x00, 0x00, 0x00,
  0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0xa6, 0x18, 0x00, 0x00, 0xa6,
  0x18, 0x01, 0x7e, 0x75, 0xc9, 0x8d, 0x00, 0x00, 0x05, 0xd9, 0x49, 0x44,
  0x41, 0x54, 0x68, 0x43, 0xed, 0x9a, 0x4b, 0x6c, 0x54, 0x55, 0x18, 0xc7,
  0xbf, 0xa1, 0x53, 0xa6, 0x8f, 0x01, 0x4b, 0x79, 0x88, 0xa2, 0xd4, 0x12,
  0x2a, 0x30, 0x14, 0x1a, 0x07, 0xda, 0xb8, 0xd1, 0xa0, 0xc4, 0xc4, 0x44,
  0x17, 0x6a, 0xd8, 0xba, 0x61, 0xe1, 0x42, 0x12, 0x0d, 0x0b, 0x23, 0x1a,
  0x83, 0x71, 0xe1, 0x42, 0x37, 0xd8, 0xa8, 0xb8, 0x54, 0x63, 0xd4, 0x85,
  0x31, 0x2c, 0x7c, 0x10, 0x12, 0x05, 0x41, 0x4a, 0x09, 0x2e, 0x50, 0x12,
  0xe3, 0x23, 0x9a, 0x62, 0xb4, 0x20, 0x31, 0x08, 0x5a, 0xa1, 0x74, 0xa8,
  0xad, 0xbf, 0xff, 0xcc, 0x9d, 0x32, 0xcc, 0xdc, 0xb6, 0x67, 0xee, 0xbd,
  0x73, 0x6b, 0x9a, 0x9e, 0xe4, 0xe4, 0xde, 0x73, 0xef, 0xb9, 0xdf, 0xf9,
  0xff, 0xbf, 0xd7, 0xf9, 0xe6, 0xb4, 0x89, 0xf1, 0xf1, 0x71, 0x9b, 0x0d,
  0x6d, 0xde, 0x6c, 0x20, 0x21, 0x0e, 0x73, 0x44, 0xfe, 0x6f, 0x96, 0x9c,
  0x35, 0x16, 0x49, 0xfa, 0x69, 0x36, 0x91, 0x48, 0xc4, 0xaa, 0xf0, 0x66,
  0xb3, 0x86, 0x9c, 0xd9, 0x96, 0x7f, 0xcd, 0xb6, 0xb1, 0x70, 0x57, 0xda,
  0xac, 0xfd, 0xaa, 0xd9, 0x95, 0x11, 0xb3, 0x33, 0x20, 0x39, 0x51, 0x67,
  0xf6, 0x5e, 0x23, 0xd7, 0xbf, 0xcc, 0x46, 0x05, 0xcc, 0x37, 0x41, 0xe9,
  0x61, 0x79, 0x8f, 0x93, 0x45, 0x83, 0x59, 0x2b, 0x40, 0x5f, 0x00, 0xf0,
  0x39, 0x61, 0xf4, 0x3a, 0x9c, 0x4c, 0x5d, 0xe3, 0x31, 0xde, 0x0d, 0xa0,
  0xf1, 0xc7, 0x16, 0x9a, 0xa5, 0x8a, 0x44, 0xca, 0x31, 0xfb, 0x5a, 0x24,
  0x2e, 0x22, 0x4d, 0x66, 0xcd, 0x68, 0xfd, 0x69, 0x10, 0x3f, 0xc1, 0x9a,
  0x29, 0x00, 0x7f, 0x47, 0xff, 0x94, 0xfe, 0x33, 0x63, 0xb9, 0x45, 0x66,
  0xcc, 0xec, 0x01, 0xd8, 0xb4, 0x31, 0xe7, 0xc5, 0xcb, 0x66, 0xc3, 0xad,
  0x66, 0xef, 0x8a, 0x5c, 0x05, 0xc6, 0x99, 0xb2, 0x48, 0x17, 0x48, 0xd0,
  0xe2, 0x23, 0xa0, 0xbd, 0x20, 0xed, 0x13, 0xac, 0x07, 0xe6, 0x9b, 0x65,
  0x17, 0x99, 0x61, 0xa0, 0x42, 0x6b, 0x61, 0x0a, 0xcf, 0xee, 0xe2, 0x5d,
  0xbf, 0xac, 0xc3, 0xdc, 0x53, 0x8c, 0xd7, 0xf9, 0x62, 0x9e, 0x29, 0x22,
  0xb8, 0x49, 0x23, 0x00, 0xdf, 0xf7, 0x00, 0x9e, 0x04, 0xe0, 0x46, 0x62,
  0xa5, 0xa2, 0xdd, 0x84, 0x65, 0x78, 0xb7, 0x15, 0x12, 0x83, 0xbc, 0x1c,
  0x85, 0xe5, 0xb3, 0xb1, 0x13, 0x59, 0x03, 0x88, 0xa5, 0x68, 0x15, 0xcd,
  0xce, 0xaf, 0xa7, 0x2f, 0xa0, 0x2f, 0x63, 0xac, 0xe7, 0xc4, 0xc6, 0x6d,
  0x72, 0x25, 0x59, 0x03, 0x70, 0xbb, 0x96, 0x14, 0x5c, 0xc9, 0xb7, 0xf1,
  0x5d, 0x0a, 0xd2, 0x6f, 0x8a, 0x34, 0xd7, 0xfd, 0x7e, 0x44, 0x6a, 0x12,
  0x23, 0x5b, 0x59, 0xb1, 0xcf, 0xec, 0xd6, 0x9f, 0xcc, 0xb6, 0xe3, 0xdf,
  0xab, 0x18, 0xd6, 0x73, 0x9d, 0x77, 0x09, 0x20, 0xf4, 0xdc, 0x9f, 0x66,
  0x07, 0x01, 0x2f, 0x12, 0xad, 0xa0, 0xcf, 0x71, 0xff, 0xc3, 0x90, 0xd9,
  0x7a, 0x34, 0x9f, 0x06, 0xe8, 0x20, 0x11, 0xfd, 0x3b, 0x19, 0x8a, 0xc4,
  0x55, 0x68, 0x27, 0xcd, 0x46, 0x20, 0xaf, 0xb8, 0x11, 0xdb, 0x5b, 0xfc,
  0xd8, 0x46, 0x4e, 0x44, 0xbe, 0x7f, 0x14, 0xf0, 0xa0, 0x78, 0x89, 0x88,
  0x7c, 0x88, 0xe1, 0xc4, 0x1a, 0x25, 0x55, 0x5d, 0x33, 0x80, 0x07, 0x18,
  0xd7, 0x01, 0x4c, 0x8f, 0x13, 0xe4, 0xd5, 0x1d, 0xcc, 0xdf, 0xc6, 0xf8,
  0x2c, 0xf7, 0x6f, 0xe1, 0x7a, 0xaf, 0xfe, 0xed, 0x91, 0x59, 0xcd, 0x04,
  0xc8, 0x92, 0xa1, 0xcd, 0x48, 0x10, 0xbe, 0x44, 0x22, 0xdd, 0x10, 0x71,
  0x21, 0xfb, 0xde, 0xac, 0x03, 0x12, 0x7b, 0x00, 0xf5, 0x70, 0x29, 0x89,
  0x32, 0x2d, 0x2a, 0x3e, 0x72, 0x80, 0x1e, 0x81, 0x45, 0x3d, 0x73, 0xc1,
  0x6d, 0xe7, 0xb9, 0x5f, 0xc2, 0xfd, 0x06, 0x32, 0xd4, 0xa3, 0xa0, 0xc6,
  0x2b, 0xaf, 0x35, 0xe6, 0xe6, 0x5d, 0xaf, 0x78, 0x2d, 0xb7, 0x4a, 0x64,
  0x44, 0x84, 0x64, 0xd8, 0x6c, 0x2d, 0x24, 0x5e, 0x01, 0xcc, 0x83, 0x05,
  0x25, 0x4e, 0xda, 0x16, 0xcb, 0xa5, 0x78, 0xab, 0x8c, 0x95, 0x84, 0xc0,
  0x0a, 0x80, 0x1c, 0xe3, 0xd9, 0x79, 0xef, 0x8b, 0xe5, 0xc8, 0x20, 0xce,
  0xaf, 0x35, 0xe6, 0xe4, 0x2d, 0x8b, 0x6b, 0x9e, 0xf1, 0x93, 0x1a, 0x09,
  0x91, 0x4d, 0x48, 0x66, 0x3f, 0xc8, 0x78, 0x24, 0xee, 0x67, 0x38, 0xa5,
  0x5c, 0x40, 0x29, 0x95, 0x5e, 0xa0, 0xff, 0x26, 0x50, 0x8c, 0x57, 0xe2,
  0x32, 0x47, 0xf8, 0xe8, 0x6d, 0x86, 0xda, 0x08, 0x17, 0xf2, 0x6c, 0x82,
  0x48, 0xb6, 0x80, 0x5c, 0xba, 0xd2, 0x5c, 0x91, 0xaf, 0x68, 0xa1, 0x89,
  0xc8, 0x7f, 0x4f, 0x11, 0xa8, 0x90, 0xe8, 0x45, 0x8b, 0xf7, 0x4d, 0x47,
  0x42, 0x08, 0xb4, 0x1f, 0x50, 0x86, 0x0c, 0x72, 0x25, 0x1f, 0xe4, 0xdb,
  0x32, 0x00, 0x8e, 0x02, 0xe6, 0x03, 0x59, 0x85, 0xfb, 0x14, 0x9d, 0x04,
  0x57, 0x68, 0x03, 0x94, 0x30, 0x8c, 0xdb, 0xbd, 0xe1, 0xaf, 0x91, 0x13,
  0x51, 0xde, 0x47, 0xa5, 0x9d, 0xa8, 0x50, 0x24, 0xee, 0x75, 0x21, 0xe1,
  0x81, 0xb8, 0xc4, 0xc6, 0x77, 0x55, 0x75, 0x14, 0x1d, 0x6f, 0x29, 0x64,
  0x28, 0xee, 0xff, 0xe1, 0x22, 0x97, 0xab, 0x03, 0xf8, 0xe2, 0x22, 0x60,
  0x5c, 0xf6, 0x66, 0xc6, 0x9d, 0x32, 0x88, 0x97, 0xb2, 0x2b, 0xb8, 0x04,
  0xce, 0x5a, 0x52, 0x17, 0x69, 0x54, 0x81, 0xd9, 0xcb, 0x22, 0x5b, 0x0a,
  0x38, 0x9c, 0x5b, 0x06, 0xb5, 0xb6, 0xb0, 0xb7, 0x7c, 0xc4, 0x17, 0x57,
  0xb0, 0xc4, 0x69, 0x94, 0x92, 0xa3, 0x04, 0xe9, 0x56, 0xc0, 0x7b, 0x52,
  0x16, 0xed, 0xe5, 0xe6, 0x71, 0x3a, 0x6b, 0x64, 0x55, 0xa6, 0xb0, 0xc0,
  0x30, 0x73, 0xbf, 0xf6, 0x5d, 0x25, 0xc8, 0xce, 0x7e, 0x3b, 0x92, 0xc8,
  0xf5, 0x1b, 0x11, 0x7a, 0x88, 0x5b, 0xd5, 0x3d, 0xc5, 0x62, 0xcf, 0xf5,
  0x3a, 0x84, 0x06, 0x77, 0xe1, 0x5e, 0x2d, 0xec, 0x0f, 0x52, 0x7d, 0x1d,
  0xf2, 0xee, 0x44, 0xde, 0x57, 0x45, 0x59, 0xdc, 0xef, 0xdd, 0x40, 0xac,
  0x51, 0x5b, 0x25, 0xc9, 0x1a, 0xaf, 0xe9, 0x39, 0xcf, 0xbe, 0xa1, 0x0a,
  0x6e, 0x8b, 0x64, 0x67, 0x5f, 0x8f, 0x44, 0x84, 0x89, 0xc4, 0x17, 0x01,
  0x49, 0xe4, 0xc9, 0xa2, 0xdd, 0x21, 0x64, 0x7c, 0x02, 0xc8, 0x97, 0xe9,
  0x6f, 0x30, 0xfe, 0xb1, 0x54, 0x1e, 0xef, 0xde, 0x81, 0x60, 0x3d, 0x04,
  0x3b, 0x78, 0xf7, 0xad, 0xde, 0x31, 0x6f, 0x8f, 0xea, 0xaf, 0xd0, 0x44,
  0x32, 0x48, 0x23, 0xbb, 0x74, 0x21, 0x30, 0x14, 0x09, 0x17, 0x0b, 0x42,
  0x64, 0x1f, 0x69, 0xaa, 0x81, 0xb5, 0xe4, 0x5d, 0xda, 0x73, 0xce, 0xe1,
  0x8a, 0x72, 0xe1, 0x8a, 0x9f, 0x1d, 0x22, 0xe6, 0x9c, 0xb5, 0xe4, 0x02,
  0xa7, 0x21, 0x41, 0x9a, 0xed, 0xc5, 0x67, 0xef, 0xae, 0x32, 0x26, 0x9c,
  0x83, 0xa7, 0x64, 0x62, 0x23, 0xe0, 0x17, 0x60, 0xbe, 0x7b, 0x78, 0x96,
  0xe4, 0x7e, 0x3f, 0xd6, 0x39, 0x31, 0x99, 0x20, 0xa7, 0x60, 0x5f, 0xc9,
  0xd7, 0xbf, 0xe0, 0x4e, 0x4a, 0xb1, 0x31, 0x91, 0x10, 0xde, 0x26, 0x6d,
  0x28, 0x68, 0xfa, 0xb0, 0x88, 0x00, 0xf4, 0xf5, 0x1e, 0xb3, 0xcb, 0x07,
  0x27, 0x63, 0x32, 0x5d, 0xb0, 0xb7, 0x49, 0x0a, 0xd9, 0x29, 0x44, 0x60,
  0xbb, 0x26, 0x80, 0xeb, 0xe6, 0x29, 0xf0, 0x71, 0xe3, 0xa5, 0xb8, 0x57,
  0x92, 0xa4, 0x90, 0x26, 0xe8, 0x27, 0xb2, 0x62, 0xd5, 0x31, 0xc2, 0xc7,
  0x46, 0xb9, 0xdd, 0x89, 0x50, 0x29, 0x22, 0x48, 0x76, 0x0a, 0x44, 0xc2,
  0xcb, 0x50, 0x87, 0x48, 0xc9, 0x2a, 0xdf, 0x2a, 0x5a, 0x55, 0x44, 0x6e,
  0xe0, 0x73, 0x7c, 0x32, 0x03, 0x89, 0xcf, 0xe2, 0x26, 0xc1, 0x7a, 0x23,
  0x78, 0xc1, 0x33, 0x64, 0x2d, 0xdf, 0x18, 0x76, 0x26, 0xa2, 0x5a, 0x00,
  0x12, 0x6b, 0x90, 0x72, 0x80, 0xdb, 0xe2, 0x21, 0x40, 0x60, 0xed, 0xba,
  0x64, 0xa9, 0x92, 0x39, 0xfa, 0xd9, 0xfb, 0x21, 0x9e, 0x70, 0x5d, 0xd1,
  0x58, 0x6a, 0x16, 0x67, 0x22, 0x08, 0x69, 0x47, 0x98, 0x76, 0xdd, 0xb8,
  0x49, 0x8c, 0xc9, 0x03, 0x50, 0xe2, 0x5a, 0xa5, 0xfa, 0xaa, 0xe2, 0xda,
  0x8f, 0x9d, 0x36, 0x28, 0x84, 0xe8, 0x0c, 0x29, 0x2e, 0x2b, 0x68, 0x1d,
  0x91, 0x38, 0xac, 0xdf, 0xee, 0xca, 0x92, 0x53, 0x35, 0x67, 0x8b, 0x10,
  0x64, 0x67, 0x67, 0x80, 0xc4, 0x97, 0x6c, 0x78, 0x77, 0x74, 0x4c, 0x43,
  0x42, 0xaf, 0x9d, 0x89, 0xa0, 0x15, 0x7e, 0x26, 0xc7, 0x66, 0x0d, 0x95,
  0x1e, 0x47, 0x71, 0xe7, 0xec, 0x0a, 0xc7, 0xc2, 0xd3, 0x99, 0x08, 0x19,
  0x63, 0x3b, 0x49, 0xfb, 0x8f, 0x18, 0xc8, 0xe8, 0x14, 0xb1, 0x0f, 0x4b,
  0x6c, 0xa2, 0x10, 0x75, 0xae, 0x9e, 0x9d, 0x89, 0xb0, 0x01, 0xa5, 0x20,
  0xf3, 0x24, 0x92, 0xa9, 0xd4, 0x6b, 0x66, 0x19, 0xc5, 0xc4, 0x31, 0xac,
  0xbf, 0x99, 0xe3, 0x1e, 0x67, 0x12, 0x55, 0xb9, 0x96, 0x26, 0x43, 0xa6,
  0x11, 0x32, 0x4f, 0xb1, 0xc2, 0xc5, 0x1a, 0x90, 0x91, 0x25, 0xfa, 0xc9,
  0x4e, 0xdd, 0x54, 0x0e, 0x55, 0x91, 0xa8, 0x9a, 0x88, 0x3e, 0x20, 0xe8,
  0x9b, 0xb4, 0x31, 0xb1, 0x12, 0xc7, 0x4c, 0x91, 0x59, 0x46, 0x24, 0x8e,
  0x23, 0xb7, 0x87, 0x9f, 0x03, 0x55, 0x93, 0x08, 0x44, 0xc4, 0xb3, 0x4c,
  0x33, 0xc1, 0xf8, 0x1c, 0x2b, 0x72, 0xcc, 0x14, 0x9a, 0x8c, 0xdc, 0xe9,
  0x38, 0xee, 0xd4, 0xc3, 0x59, 0x4f, 0x20, 0x12, 0x81, 0x89, 0xe8, 0x43,
  0x7c, 0x38, 0x8d, 0x06, 0x77, 0x73, 0x1b, 0x86, 0x4c, 0x9e, 0x04, 0xee,
  0xd4, 0xa3, 0xf3, 0x5c, 0x87, 0x2c, 0x1b, 0x7e, 0x43, 0xf4, 0x93, 0xe0,
  0x1d, 0x67, 0x3e, 0x1f, 0x90, 0x4c, 0xde, 0x9d, 0x64, 0x89, 0xe5, 0x21,
  0x49, 0x84, 0xb2, 0x48, 0x91, 0x58, 0x40, 0x32, 0xb2, 0x44, 0x3f, 0x16,
  0xed, 0xa6, 0x10, 0x0d, 0x65, 0x89, 0x22, 0x0e, 0xe7, 0xf4, 0x3b, 0x95,
  0xd9, 0xc9, 0xf9, 0x69, 0x62, 0x66, 0xb7, 0x63, 0xcc, 0x88, 0x44, 0x1f,
  0xee, 0xb4, 0x99, 0x42, 0x34, 0x12, 0x12, 0x91, 0x58, 0xa4, 0x48, 0x10,
  0xcd, 0x2a, 0x66, 0x94, 0x00, 0xa6, 0xca, 0x66, 0x22, 0x71, 0x04, 0x12,
  0x59, 0x7e, 0x1c, 0x45, 0x46, 0x22, 0x52, 0x22, 0x12, 0xa6, 0x3f, 0x9b,
  0x79, 0xa9, 0x59, 0x47, 0x98, 0xe5, 0xc5, 0xa5, 0x4a, 0xf1, 0xcf, 0x71,
  0xc5, 0xae, 0xfc, 0x39, 0x67, 0xc4, 0x2d, 0x12, 0xd7, 0x2a, 0xc5, 0xc4,
  0xa6, 0xa9, 0x7d, 0x66, 0x67, 0x59, 0x39, 0xa3, 0xa3, 0xcf, 0x8f, 0xb1,
  0xc4, 0xba, 0x1b, 0x23, 0x26, 0x10, 0x69, 0x8c, 0x94, 0x63, 0xf3, 0xca,
  0x19, 0xd5, 0x66, 0xaa, 0x98, 0x55, 0x00, 0xee, 0xa3, 0x00, 0x5c, 0x55,
  0x2b, 0x12, 0x93, 0xb9, 0x56, 0x62, 0xee, 0x9f, 0x6a, 0x6a, 0xe4, 0x36,
  0x41, 0xc5, 0x3a, 0x1f, 0xd0, 0x05, 0x5d, 0x20, 0xae, 0xef, 0xe6, 0x88,
  0xc4, 0xa5, 0x69, 0xd7, 0x75, 0xfe, 0x03, 0x83, 0x45, 0x14, 0xe5, 0x22,
  0xda, 0x7e, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
  0x42, 0x60, 0x82
};