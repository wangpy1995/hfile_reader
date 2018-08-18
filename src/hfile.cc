//
// Created by wangpengyu6 on 18-8-14.
//

#include "hfile.h"

#include <cstring>
#include "hbase/util/bytes.h"

static const int32_t *ComputeTrailerSizeByVersion() {
  int32_t *version_to_size = new int[HFile::max_format_version + 1];
  version_to_size[2] = 212;
  for (int32_t version = 3;
       version <= HFile::max_format_version;
       ++version) {
    version_to_size[version] = 4 * 1024;
  }
  return version_to_size;
}

static const int32_t *trailer_size = ComputeTrailerSizeByVersion();

static inline int32_t GetTrailerSize(int32_t version) {
  return trailer_size[version];
}

static const int32_t GetMaxTrailerSize() {
  int32_t max_size = 0;
  for (int32_t version = HFile::min_format_version;
       version <= HFile::max_format_version;
       ++version) {
    max_size = std::max(GetTrailerSize(version), max_size);
  }
  return max_size;
}

static const int32_t max_trailer_size = GetMaxTrailerSize();

const void DeserializeFromProtoBuf(
    const char *bytes, int32_t pos,
    int32_t trailer_size,
    struct FixedFileTrailer *trailer) {
  using CodedInputStream =  google::protobuf::io::CodedInputStream;
  hbase::pb::FileTrailerProto trailer_proto;
  CodedInputStream input(
      reinterpret_cast<const uint8_t *>(bytes + pos), trailer_size);
  uint32_t message_size;
  input.ReadVarint32(&message_size);
  if (trailer_proto.ParsePartialFromCodedStream(&input)) {
    if (trailer_proto.has_file_info_offset()) {
      trailer->file_info_offset
          = trailer_proto.file_info_offset();
    }
    if (trailer_proto.has_load_on_open_data_offset()) {
      trailer->load_on_open_data_offset
          = trailer_proto.load_on_open_data_offset();
    }
    if (trailer_proto.has_uncompressed_data_index_size()) {
      trailer->uncompressed_data_index_size
          = trailer_proto.uncompressed_data_index_size();
    }
    if (trailer_proto.has_total_uncompressed_bytes()) {
      trailer->total_uncompressed_bytes
          = trailer_proto.total_uncompressed_bytes();
    }
    if (trailer_proto.has_data_index_count()) {
      trailer->data_index_count = trailer_proto.data_index_count();
    }
    if (trailer_proto.has_meta_index_count()) {
      trailer->meta_index_count = trailer_proto.meta_index_count();
    }
    if (trailer_proto.has_entry_count()) {
      trailer->entry_count = trailer_proto.entry_count();
    }
    if (trailer_proto.has_num_data_index_levels()) {
      trailer->num_data_index_levels = trailer_proto.num_data_index_levels();
    }
    if (trailer_proto.has_first_data_block_offset()) {
      trailer->first_data_block_offset
          = trailer_proto.first_data_block_offset();
    }
    if (trailer_proto.has_last_data_block_offset()) {
      trailer->last_data_block_offset = trailer_proto.last_data_block_offset();
    }
    // 无法反射comparator
    if (trailer_proto.has_comparator_class_name()) {
      trailer->comparator_class_name
          = trailer_proto.comparator_class_name();
    }

    if (trailer_proto.has_compression_codec()) {
      trailer->compression_codec = trailer_proto.compression_codec();
    }
    if (trailer_proto.has_encryption_key()) {
      trailer->encryption_key = trailer_proto.encryption_key();
    }
  }
}

const void DeserializeFromWritable(
    const char *bytes, int32_t pos, int32_t trailer_size,
    struct FixedFileTrailer *trailer) {
  int32_t offset = 0;
  trailer->file_info_offset = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->load_on_open_data_offset = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->data_index_count = Bytes::ToInt<int>(bytes, offset);
  offset += sizeof(int);
  trailer->uncompressed_data_index_size = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->meta_index_count = Bytes::ToInt<int>(bytes, offset);
  offset += sizeof(int);

  trailer->total_uncompressed_bytes = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->entry_count = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->num_data_index_levels = Bytes::ToInt<int>(bytes, offset);
  offset += sizeof(int);
  trailer->compression_codec = Bytes::ToInt<int>(bytes, offset);
  offset += sizeof(int);
  trailer->first_data_block_offset = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
  trailer->last_data_block_offset = Bytes::ToInt<int64_t>(bytes, offset);
  offset += sizeof(int64_t);
//    std::string encryption_key_;
  trailer->comparator_class_name = bytes + offset;
}

const void ReadFromStream(std::FILE *file, int64_t file_size,
                          struct FixedFileTrailer *trailer) {
  int32_t buffer_size = max_trailer_size;
  if (file_size < buffer_size) {
    buffer_size = file_size;
  }
  std::fseek(file, -buffer_size, SEEK_CUR);

  char *bytes = static_cast<char *>(malloc(sizeof(char) * buffer_size));
  auto in = std::fread(bytes, 1, buffer_size, file);

  //版本信息 4B
  int32_t pos = in - sizeof(int);
  int32_t version = Bytes::ToInt<int>(bytes + pos);
  trailer->major_version = version & 0x00ffffff;
  trailer->minor_version = version >> 24;

  int32_t trailer_size = GetTrailerSize(trailer->major_version);
  pos = buffer_size - trailer_size;
  //  trailer校验信息 8B "TRABLK\"$";
  static constexpr int64_t magic = 0x24224b4c42415254;
  bool valid = magic == *(reinterpret_cast<int64_t *> (bytes + pos));
  // 校验合法
  if (valid) {
    pos += 8;
    if (trailer->major_version > 2
        || (trailer->major_version == 2 && trailer->minor_version >= 2)) {
      DeserializeFromProtoBuf(bytes, pos, trailer_size, trailer);
    } else {
      DeserializeFromWritable(bytes, pos, trailer_size, trailer);
    }
  }
  free(bytes);
}
