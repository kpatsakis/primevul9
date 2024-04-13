msg_t *MsgAddRef(msg_t *pM)
{
	assert(pM != NULL);
#	ifdef HAVE_ATOMIC_BUILTINS
		ATOMIC_INC(&pM->iRefCount, NULL);
#	else
		MsgLock(pM);
		pM->iRefCount++;
		MsgUnlock(pM);
#	endif
	/* DEV debugging only! dbgprintf("MsgAddRef\t0x%x done, Ref now: %d\n", (int)pM, pM->iRefCount);*/
	return(pM);
}