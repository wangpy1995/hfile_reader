//
// Created by wangpengyu6 on 18-8-14.
//

#ifndef FILE_READER_BYTES_H
#define FILE_READER_BYTES_H
#include <stdint.h>

class Bytes {
 public:
  // 整型转换  big-endian
  template<typename base_type>
  static base_type ToInt(const int8_t *bytes,
                         int offset = 0,
                         const int length = sizeof(base_type)) {
    base_type n = 0;
    for (int i = offset; i < (offset + length); i++) {
      n <<= 8;
      n ^= bytes[i] & 0xFF;
    }
    return n;
  }
  template<typename base_type>
  static base_type Read(FILE *fp,
                        int64_t offset = 0,
                        const int32_t length = sizeof(base_type)) {

  }
};

#endif // FILE_READER_BYTES_H
