rsRetVal MsgEnableThreadSafety(void)
{
	DEFiRet;
	funcLock = MsgLockLockingCase;
	funcUnlock = MsgUnlockLockingCase;
	funcMsgPrepareEnqueue = MsgPrepareEnqueueLockingCase;
	funcDeleteMutex = MsgDeleteMutexLockingCase;
	RETiRet;
}