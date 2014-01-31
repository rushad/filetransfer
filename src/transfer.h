#pragma once

namespace FileTransfer
{
  enum TransferState 
  { 
    STATE_SUCCESS, 
    STATE_CANCELLED, 
    STATE_TIMEOUT, 
    STATE_ERROR 
  };
}