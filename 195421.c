mptctl_do_taskmgmt(MPT_ADAPTER *ioc, u8 tm_type, u8 bus_id, u8 target_id)
{
	MPT_FRAME_HDR	*mf;
	SCSITaskMgmt_t	*pScsiTm;
	SCSITaskMgmtReply_t *pScsiTmReply;
	int		 ii;
	int		 retval;
	unsigned long	 timeout;
	unsigned long	 time_count;
	u16		 iocstatus;


	mutex_lock(&ioc->taskmgmt_cmds.mutex);
	if (mpt_set_taskmgmt_in_progress_flag(ioc) != 0) {
		mutex_unlock(&ioc->taskmgmt_cmds.mutex);
		return -EPERM;
	}

	retval = 0;

	mf = mpt_get_msg_frame(mptctl_taskmgmt_id, ioc);
	if (mf == NULL) {
		dtmprintk(ioc,
			printk(MYIOC_s_WARN_FMT "TaskMgmt, no msg frames!!\n",
			ioc->name));
		mpt_clear_taskmgmt_in_progress_flag(ioc);
		retval = -ENOMEM;
		goto tm_done;
	}

	dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT "TaskMgmt request (mf=%p)\n",
		ioc->name, mf));

	pScsiTm = (SCSITaskMgmt_t *) mf;
	memset(pScsiTm, 0, sizeof(SCSITaskMgmt_t));
	pScsiTm->Function = MPI_FUNCTION_SCSI_TASK_MGMT;
	pScsiTm->TaskType = tm_type;
	if ((tm_type == MPI_SCSITASKMGMT_TASKTYPE_RESET_BUS) &&
		(ioc->bus_type == FC))
		pScsiTm->MsgFlags =
				MPI_SCSITASKMGMT_MSGFLAGS_LIPRESET_RESET_OPTION;
	pScsiTm->TargetID = target_id;
	pScsiTm->Bus = bus_id;
	pScsiTm->ChainOffset = 0;
	pScsiTm->Reserved = 0;
	pScsiTm->Reserved1 = 0;
	pScsiTm->TaskMsgContext = 0;
	for (ii= 0; ii < 8; ii++)
		pScsiTm->LUN[ii] = 0;
	for (ii=0; ii < 7; ii++)
		pScsiTm->Reserved2[ii] = 0;

	switch (ioc->bus_type) {
	case FC:
		timeout = 40;
		break;
	case SAS:
		timeout = 30;
		break;
	case SPI:
		default:
		timeout = 10;
		break;
	}

	dtmprintk(ioc,
		printk(MYIOC_s_DEBUG_FMT "TaskMgmt type=%d timeout=%ld\n",
		ioc->name, tm_type, timeout));

	INITIALIZE_MGMT_STATUS(ioc->taskmgmt_cmds.status)
	time_count = jiffies;
	if ((ioc->facts.IOCCapabilities & MPI_IOCFACTS_CAPABILITY_HIGH_PRI_Q) &&
	    (ioc->facts.MsgVersion >= MPI_VERSION_01_05))
		mpt_put_msg_frame_hi_pri(mptctl_taskmgmt_id, ioc, mf);
	else {
		retval = mpt_send_handshake_request(mptctl_taskmgmt_id, ioc,
		    sizeof(SCSITaskMgmt_t), (u32 *)pScsiTm, CAN_SLEEP);
		if (retval != 0) {
			dfailprintk(ioc,
				printk(MYIOC_s_ERR_FMT
				"TaskMgmt send_handshake FAILED!"
				" (ioc %p, mf %p, rc=%d) \n", ioc->name,
				ioc, mf, retval));
			mpt_free_msg_frame(ioc, mf);
			mpt_clear_taskmgmt_in_progress_flag(ioc);
			goto tm_done;
		}
	}

	/* Now wait for the command to complete */
	ii = wait_for_completion_timeout(&ioc->taskmgmt_cmds.done, timeout*HZ);

	if (!(ioc->taskmgmt_cmds.status & MPT_MGMT_STATUS_COMMAND_GOOD)) {
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "TaskMgmt failed\n", ioc->name));
		mpt_free_msg_frame(ioc, mf);
		mpt_clear_taskmgmt_in_progress_flag(ioc);
		if (ioc->taskmgmt_cmds.status & MPT_MGMT_STATUS_DID_IOCRESET)
			retval = 0;
		else
			retval = -1; /* return failure */
		goto tm_done;
	}

	if (!(ioc->taskmgmt_cmds.status & MPT_MGMT_STATUS_RF_VALID)) {
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "TaskMgmt failed\n", ioc->name));
		retval = -1; /* return failure */
		goto tm_done;
	}

	pScsiTmReply = (SCSITaskMgmtReply_t *) ioc->taskmgmt_cmds.reply;
	dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
	    "TaskMgmt fw_channel = %d, fw_id = %d, task_type=0x%02X, "
	    "iocstatus=0x%04X\n\tloginfo=0x%08X, response_code=0x%02X, "
	    "term_cmnds=%d\n", ioc->name, pScsiTmReply->Bus,
	    pScsiTmReply->TargetID, tm_type,
	    le16_to_cpu(pScsiTmReply->IOCStatus),
	    le32_to_cpu(pScsiTmReply->IOCLogInfo),
	    pScsiTmReply->ResponseCode,
	    le32_to_cpu(pScsiTmReply->TerminationCount)));

	iocstatus = le16_to_cpu(pScsiTmReply->IOCStatus) & MPI_IOCSTATUS_MASK;

	if (iocstatus == MPI_IOCSTATUS_SCSI_TASK_TERMINATED ||
	   iocstatus == MPI_IOCSTATUS_SCSI_IOC_TERMINATED ||
	   iocstatus == MPI_IOCSTATUS_SUCCESS)
		retval = 0;
	else {
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "TaskMgmt failed\n", ioc->name));
		retval = -1; /* return failure */
	}

 tm_done:
	mutex_unlock(&ioc->taskmgmt_cmds.mutex);
	CLEAR_MGMT_STATUS(ioc->taskmgmt_cmds.status)
	return retval;
}