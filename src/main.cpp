#include "observer.h"
#include "single_filetransfer.h"

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

class MyObserver : public FileTransfer::Observer
{
public:
  virtual void UpdateProgress(const unsigned dlPerc, const unsigned ulPerc)
  {
    std::cout << dlPerc << " : " << ulPerc << std::endl;
  }
};

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "RUS");

  if(!CheckOptions(argc, argv))
  {
    return -1;
  }

  MyObserver obs;
  FileTransfer::SingleFileTransfer ft(
    "https://root:qwe123QWE@10.27.11.125/folder/test1/nz_freedos_1-flat.vmdk?dcPath=ha%2ddatacenter&dsName=datastore1",
    "https://root:qwe123QWE@10.27.11.125/folder/test1/test.dat?dcPath=ha-datacenter&dsName=datastore1",
    &obs);

  ft.Start();
  ft.Wait();

  return 0;
}