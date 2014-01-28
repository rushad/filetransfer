#include "curl.h"
#include "filetransfer.h"

#include <gtest/gtest.h>

#include <iostream>

bool CheckOptions(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string opt(argv[i]);
    if(opt == "--gtest")
    {
      std::cout << "Running tests..." << std::endl;
      testing::InitGoogleTest(&argc, argv);
      RUN_ALL_TESTS();
      return false;
    }
  }

  return true;
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "RUS");

  if(!CheckOptions(argc, argv))
  {
    return -1;
  }

  curl_global_init(CURL_GLOBAL_ALL);

  FileTransfer::FileTransfer ft(
    "https://root:qwe123QWE@10.27.11.125/folder/test1/nz_freedos_1-flat.vmdk?dcPath=ha%2ddatacenter&dsName=datastore1",
    "https://root:qwe123QWE@10.27.11.125/folder/test1/test.dat?dcPath=ha-datacenter&dsName=datastore1");

  ft.Exec();

  curl_global_cleanup();

  return 0;
}