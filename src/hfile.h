//
// Created by wangpengyu6 on 18-8-14.
//

#ifndef FILE_READER_HFILE_READER_H
#define FILE_READER_HFILE_READER_H

#include <string>
#include <algorithm>
#include <fstream>
#include "hbase/proto/HFile.pb.h"

struct HFile {
  static constexpr int32_t min_format_version = 2;
  static constexpr int32_t max_format_version = 3;
};

struct FixedFileTrailer {
  int32_t major_version;
  int32_t minor_version;
  int32_t compression_codec;
  int32_t data_index_count;
  int32_t meta_index_count;
  int64_t file_info_offset;
  int64_t load_on_open_data_offset;
  int64_t uncompressed_data_index_size;
  int64_t total_uncompressed_bytes;
  int64_t entry_count;
  int64_t first_data_block_offset;
  int64_t last_data_block_offset;
  int64_t num_data_index_levels;
  std::string encryption_key;
  std::string comparator_class_name;
};

//static const int32_t *trailer_size;
//static const int32_t max_trailer_size;
static inline int32_t GetTrailerSize(int32_t version);
const void ReadFromStream(std::FILE *file, int64_t file_size,
    struct FixedFileTrailer *trailer);

#endif   //FILE_READER_HFILE_READER_H
