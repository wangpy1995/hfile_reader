//
// Created by wangpengyu6 on 18-8-14.
//

#ifndef FILE_READER_HFILE_READER_H
#define FILE_READER_HFILE_READER_H

#include "hfile.h"
#include <cstdio>
#include <cstdint>

enum Header {
  // Format of header is:
  // 8 bytes - block magic
  // 4 bytes int - onDiskSizeWithoutHeader
  // 4 bytes int - uncompressedSizeWithoutHeader
  // 8 bytes long - prevBlockOffset
  // The following 3 are only present if header contains checksum information
  // 1 byte - checksum type
  // 4 byte int - bytes per checksum
  // 4 byte int - onDiskDataSizeWithHeader
      BLOCK_MAGIC_INDEX = 0,
  ON_DISK_SIZE_WITHOUT_HEADER_INDEX = 8,
  UNCOMPRESSED_SIZE_WITHOUT_HEADER_INDEX = 12,
  PREV_BLOCK_OFFSET_INDEX = 16,
  CHECKSUM_TYPE_INDEX = 24,
  BYTES_PER_CHECKSUM_INDEX = 25,
  ON_DISK_DATA_SIZE_WITH_HEADER_INDEX = 29
};

enum BlockType {
  // Scanned block section

  /** Data block, both versions */
      DATA,

  /** An encoded data block (e.g. with prefix compression), version 2 */
      ENCODED_DATA,

  /** Version 2 leaf index block. Appears in the data block section */
      LEAF_INDEX,

  /** Bloom filter block, version 2 */
      BLOOM_CHUNK,

  // Non-scanned block section

  /** Meta blocks */
      META,

  /** Intermediate-level version 2 index in the non-data block section */
      INTERMEDIATE_INDEX,

  // Load-on-open section.

  /** Root index block, also used for the single-level meta index, version 2 */
      ROOT_INDEX,

  /** File info, version 2 */
      FILE_INFO,

  /** General Bloom filter metadata, version 2 */
      GENERAL_BLOOM_META,

  /** Delete Family Bloom filter metadata, version 2 */
      DELETE_FAMILY_BLOOM_META,

  // Trailer

  /** Fixed file trailer, both versions (always just a magic string) */
      TRAILER,

  // Legacy blocks

  /** Block index magic string in version 1 */
      INDEX_V1
};

enum BlockCategory {
  DATA, META, INDEX, BLOOM, ALL_CATEGORIES, UNKNOWN
};

//存放数据起始位置以及大小
struct BlockRange {
  int64_t offset;
  int32_t data_size;
};

struct FileBlock {
  BlockRange block_range;
  //
  char key[0];
};

void ReadRootIndex(FILE *fp, int32_t num_entries) {
  for (int i = 0; i < num_entries; ++i) {

  }
}

// Data index. We also read statistics about the block index written after
// the root level.
void ReadMultiLevelIndexRoot(const FixedFileTrailer &trailer, std::FILE *fp, int64_t file_size) {
  int32_t trailer_size = GetTrailerSize(trailer.major_version);
  // read root index
  // BlockType::ROOT_INDEX;
  std::fseek(trailer.load_on_open_data_offset);
  for (int i = 0; i < trailer.data_index_count; ++i) {

  }
}

#endif //FILE_READER_HFILE_READER_H
