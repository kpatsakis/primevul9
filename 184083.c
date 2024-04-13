mainloop(void)
{
	struct timeval tvSelectTimeout;

	BEGINfunc
	/* first check if we have any internal messages queued and spit them out. We used
	 * to do that on any loop iteration, but that is no longer necessry. The reason
	 * is that once we reach this point here, we always run on multiple threads and
	 * thus the main queue is properly initialized. -- rgerhards, 2008-06-09
	 */
	processImInternal();

	while(!bFinished){
		/* this is now just a wait - please note that we do use a near-"eternal"
		 * timeout of 1 day if we do not have repeated message reduction turned on
		 * (which it is not by default). This enables us to help safe the environment
		 * by not unnecessarily awaking rsyslog on a regular tick (just think
		 * powertop, for example). In that case, we primarily wait for a signal,
		 * but a once-a-day wakeup should be quite acceptable. -- rgerhards, 2008-06-09
		 */
		tvSelectTimeout.tv_sec = (bReduceRepeatMsgs == 1) ? TIMERINTVL : 86400 /*1 day*/;
		//tvSelectTimeout.tv_sec = TIMERINTVL; /* TODO: change this back to the above code when we have a better solution for apc */
		tvSelectTimeout.tv_usec = 0;
		select(1, NULL, NULL, NULL, &tvSelectTimeout);
		if(bFinished)
			break;	/* exit as quickly as possible - see long comment below */

		/* If we received a HUP signal, we call doFlushRptdMsgs() a bit early. This
 		 * doesn't matter, because doFlushRptdMsgs() checks timestamps. What may happen,
 		 * however, is that the too-early call may lead to a bit too-late output
 		 * of "last message repeated n times" messages. But that is quite acceptable.
 		 * rgerhards, 2007-12-21
		 * ... and just to explain, we flush here because that is exactly what the mainloop
		 * shall do - provide a periodic interval in which not-yet-flushed messages will
		 * be flushed. Be careful, there is a potential race condition: doFlushRptdMsgs()
		 * needs to aquire a lock on the action objects. If, however, long-running consumers
		 * cause the main queue worker threads to lock them for a long time, we may receive
		 * a starvation condition, resulting in the mainloop being held on lock for an extended
		 * period of time. That, in turn, could lead to unresponsiveness to termination
		 * requests. It is especially important that the bFinished flag is checked before
		 * doFlushRptdMsgs() is called (I know because I ran into that situation). I am
		 * not yet sure if the remaining probability window of a termination-related
		 * problem is large enough to justify changing the code - I would consider it
		 * extremely unlikely that the problem ever occurs in practice. Fixing it would
		 * require not only a lot of effort but would cost considerable performance. So
		 * for the time being, I think the remaining risk can be accepted.
		 * rgerhards, 2008-01-10
 		 */
		if(bReduceRepeatMsgs == 1)
			doFlushRptdMsgs();

		if(bHadHUP) {
			doHUP();
			bHadHUP = 0;
			continue;
		}
		// TODO: remove execScheduled(); /* handle Apc calls (if any) */
	}
	ENDfunc
}