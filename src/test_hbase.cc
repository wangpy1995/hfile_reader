//
// Created by wangpengyu6 on 18-8-13.
//

#include "test_hbase.h"

#include <vector>
#include <iostream>
#include <string>
#include <tuple>

#include "hfile.h"
#include "hbase/util/bytes.h"

TEST(hbase, scan) {
  namespace t_transport = apache::thrift::transport;
  namespace t_protocol =  apache::thrift::protocol;
  namespace hbase =   apache::hadoop::hbase::thrift2;
  // conf
  std::string host("10.3.67.124");
  int port = 9090;
  // connection
  boost::shared_ptr<t_transport::TSocket> socket(
      new t_transport::TSocket(host, port));
  boost::shared_ptr<t_transport::TTransport> transport(
      new t_transport::TBufferedTransport(socket));
  boost::shared_ptr<t_protocol::TProtocol> protocol(
      new t_protocol::TBinaryProtocol(transport));
  transport->open();
  hbase::THBaseServiceClient client(protocol);


  // table info
  std::string table_name = "SNAP_IMAGE_INFO";
  std::vector<hbase::TResult> results;

  // scan
  int nums = 1000;
  apache::hadoop::hbase::thrift2::TScan scan;
  const std::string start_row("2018060");
  const std::string stop_row("2018061");
  scan.__set_startRow(start_row);
  scan.__set_stopRow(stop_row);

  // set column
  hbase::TColumn face_time;
  face_time.__set_family("info");
  face_time.__set_qualifier("face_time");
  hbase::TColumn age_range;
  age_range.__set_family("info");
  age_range.__set_qualifier("age_range");
  std::vector<hbase::TColumn> columns({face_time, age_range});
  scan.__set_columns(columns);

  // get result
//    int scan_id = client.openScanner(table_name, scan);
  client.getScannerResults(results, table_name, scan, nums);
//    client.closeScanner(scan_id);
  transport->close();
  if (!results.empty()) {
    for (int i = 0; i < nums; ++i) {
      auto result = results[i];
      std::cout << "row: " << result.row << "\n";
      auto cell = result.columnValues;
      std::cout << cell[0].family << ":" << cell[0].qualifier << "  \t"
                << Bytes::ToInt<int32_t>(cell[0].value.c_str())
                << std::endl;
      std::cout << cell[1].family << ":" << cell[1].qualifier << "  \t"
                << Bytes::ToInt<int64_t>(cell[1].value.c_str())
                << std::endl;
    }
  }
}

TEST(hbase, file) {
  FixedFileTrailer file_trailer;
  std::FILE *fp = std::fopen("hfile/276bb59aae484deda66683525eb91308", "r");
  std::fseek(fp, 0, SEEK_END);
  int64_t size = std::ftell(fp);
  ReadFromStream(fp, size, &file_trailer);
  std::fclose(fp);
}

