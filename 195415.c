mptctl_event_process(MPT_ADAPTER *ioc, EventNotificationReply_t *pEvReply)
{
	u8 event;

	event = le32_to_cpu(pEvReply->Event) & 0xFF;

	dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT "%s() called\n",
	    ioc->name, __func__));
	if(async_queue == NULL)
		return 1;

	/* Raise SIGIO for persistent events.
	 * TODO - this define is not in MPI spec yet,
	 * but they plan to set it to 0x21
	 */
	if (event == 0x21) {
		ioc->aen_event_read_flag=1;
		dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT "Raised SIGIO to application\n",
		    ioc->name));
		devtverboseprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "Raised SIGIO to application\n", ioc->name));
		kill_fasync(&async_queue, SIGIO, POLL_IN);
		return 1;
	 }

	/* This flag is set after SIGIO was raised, and
	 * remains set until the application has read
	 * the event log via ioctl=MPTEVENTREPORT
	 */
	if(ioc->aen_event_read_flag)
		return 1;

	/* Signal only for the events that are
	 * requested for by the application
	 */
	if (ioc->events && (ioc->eventTypes & ( 1 << event))) {
		ioc->aen_event_read_flag=1;
		dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "Raised SIGIO to application\n", ioc->name));
		devtverboseprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "Raised SIGIO to application\n", ioc->name));
		kill_fasync(&async_queue, SIGIO, POLL_IN);
	}
	return 1;
}