#include "curl.h"
#include "downloader.h"
#include "queue.h"
#include "uploader.h"

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

  FileTransfer::CurlSource src("https://root:qwe123QWE@10.27.11.125/folder/test1/nz_freedos_1-flat.vmdk?dcPath=ha%2ddatacenter&dsName=datastore1");
  FileTransfer::CurlTarget trg("https://root:qwe123QWE@10.27.11.125/folder/test1/test.dat?dcPath=ha-datacenter&dsName=datastore1");

  FileTransfer::Queue q;

  FileTransfer::Downloader dl(src, q);
  FileTransfer::Uploader ul(trg, q, src.GetSize());

  dl.Start();
  ul.Start();

/*  Sleep(20000);

  dl.Cancel();
  ul.Cancel();*/

  bool dlRes = dl.Wait();
  bool ulRes = false;
  if (!dlRes)
    ul.Cancel();
  else
    ulRes = ul.Wait();
  std::cout << dlRes << " : " << ulRes << std::endl;
//  std::cout << ul.Wait() << std::endl;
/*  FileTransfer::Queue::Chunk chunk = q.Pop(100000);
  std::cout << std::string((char*)&chunk[0], chunk.size()) << std::endl;*/

  curl_global_cleanup();

  return 0;
}