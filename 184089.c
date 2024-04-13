DEFFUNC_llExecFunc(flushRptdMsgsActions)
{
	action_t *pAction = (action_t*) pData;
	assert(pAction != NULL);
	
	BEGINfunc
	LockObj(pAction);
	/* TODO: time() performance: the call below could be moved to
	 * the beginn of the llExec(). This makes it slightly less correct, but
	 * in an acceptable way. -- rgerhards, 2008-09-16
	 */
	if (pAction->f_prevcount && time(NULL) >= REPEATTIME(pAction)) {
		DBGPRINTF("flush %s: repeated %d times, %d sec.\n",
		    module.GetStateName(pAction->pMod), pAction->f_prevcount,
		    repeatinterval[pAction->f_repeatcount]);
		actionWriteToAction(pAction);
		BACKOFF(pAction);
	}
	UnlockObj(pAction);

	ENDfunc
	return RS_RET_OK; /* we ignore errors, we can not do anything either way */
}