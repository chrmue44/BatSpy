#ifndef _C_CMD_H_
#define _C_CMD_H_

/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2024 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#include "types.h"

#define CMD_QUEUE_LEN  32


typedef void (*tCmdMethod)(void* pData);
void logStatus();

struct stQueueEntry
{
  enCmd cmd;
  void* pData;
};

struct stCmdTabEntry
{
  enCmd cmd;
  tCmdMethod pFunc;
};

class cCmdExecuter
{
 public:
  cCmdExecuter();
  bool isAvailable() { return m_pRead != m_pWrite; }
  void addToQueue(enCmd cmd, void* pData = nullptr);
  stQueueEntry* getCmd();
  void execCmd();

 private:
  void incQueuePtr(stQueueEntry*& ptr);
  /// excute the next command in the queue

 private:
  stQueueEntry m_queue[CMD_QUEUE_LEN];
  stQueueEntry* m_pRead = &m_queue[0];
  stQueueEntry* m_pWrite = &m_queue[0];
  bool m_queueFull = false;
};


#endif    //#ifndef _C_CMD_H_
