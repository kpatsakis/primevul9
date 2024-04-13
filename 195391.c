mptctl_taskmgmt_reply(MPT_ADAPTER *ioc, MPT_FRAME_HDR *mf, MPT_FRAME_HDR *mr)
{
	if (!mf)
		return 0;

	dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		"TaskMgmt completed (mf=%p, mr=%p)\n",
		ioc->name, mf, mr));

	ioc->taskmgmt_cmds.status |= MPT_MGMT_STATUS_COMMAND_GOOD;

	if (!mr)
		goto out;

	ioc->taskmgmt_cmds.status |= MPT_MGMT_STATUS_RF_VALID;
	memcpy(ioc->taskmgmt_cmds.reply, mr,
	    min(MPT_DEFAULT_FRAME_SIZE, 4 * mr->u.reply.MsgLength));
 out:
	if (ioc->taskmgmt_cmds.status & MPT_MGMT_STATUS_PENDING) {
		mpt_clear_taskmgmt_in_progress_flag(ioc);
		ioc->taskmgmt_cmds.status &= ~MPT_MGMT_STATUS_PENDING;
		complete(&ioc->taskmgmt_cmds.done);
		if (ioc->bus_type == SAS)
			ioc->schedule_target_reset(ioc);
		return 1;
	}
	return 0;
}