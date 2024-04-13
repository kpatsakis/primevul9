mptctl_reply(MPT_ADAPTER *ioc, MPT_FRAME_HDR *req, MPT_FRAME_HDR *reply)
{
	char	*sense_data;
	int	req_index;
	int	sz;

	if (!req)
		return 0;

	dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT "completing mpi function "
	    "(0x%02X), req=%p, reply=%p\n", ioc->name,  req->u.hdr.Function,
	    req, reply));

	/*
	 * Handling continuation of the same reply. Processing the first
	 * reply, and eating the other replys that come later.
	 */
	if (ioc->ioctl_cmds.msg_context != req->u.hdr.MsgContext)
		goto out_continuation;

	ioc->ioctl_cmds.status |= MPT_MGMT_STATUS_COMMAND_GOOD;

	if (!reply)
		goto out;

	ioc->ioctl_cmds.status |= MPT_MGMT_STATUS_RF_VALID;
	sz = min(ioc->reply_sz, 4*reply->u.reply.MsgLength);
	memcpy(ioc->ioctl_cmds.reply, reply, sz);

	if (reply->u.reply.IOCStatus || reply->u.reply.IOCLogInfo)
		dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "iocstatus (0x%04X), loginfo (0x%08X)\n", ioc->name,
		    le16_to_cpu(reply->u.reply.IOCStatus),
		    le32_to_cpu(reply->u.reply.IOCLogInfo)));

	if ((req->u.hdr.Function == MPI_FUNCTION_SCSI_IO_REQUEST) ||
		(req->u.hdr.Function ==
		 MPI_FUNCTION_RAID_SCSI_IO_PASSTHROUGH)) {

		if (reply->u.sreply.SCSIStatus || reply->u.sreply.SCSIState)
			dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT
			"scsi_status (0x%02x), scsi_state (0x%02x), "
			"tag = (0x%04x), transfer_count (0x%08x)\n", ioc->name,
			reply->u.sreply.SCSIStatus,
			reply->u.sreply.SCSIState,
			le16_to_cpu(reply->u.sreply.TaskTag),
			le32_to_cpu(reply->u.sreply.TransferCount)));

		if (reply->u.sreply.SCSIState &
			MPI_SCSI_STATE_AUTOSENSE_VALID) {
			sz = req->u.scsireq.SenseBufferLength;
			req_index =
			    le16_to_cpu(req->u.frame.hwhdr.msgctxu.fld.req_idx);
			sense_data = ((u8 *)ioc->sense_buf_pool +
			     (req_index * MPT_SENSE_BUFFER_ALLOC));
			memcpy(ioc->ioctl_cmds.sense, sense_data, sz);
			ioc->ioctl_cmds.status |= MPT_MGMT_STATUS_SENSE_VALID;
		}
	}

 out:
	/* We are done, issue wake up
	 */
	if (ioc->ioctl_cmds.status & MPT_MGMT_STATUS_PENDING) {
		if (req->u.hdr.Function == MPI_FUNCTION_SCSI_TASK_MGMT) {
			mpt_clear_taskmgmt_in_progress_flag(ioc);
			ioc->ioctl_cmds.status &= ~MPT_MGMT_STATUS_PENDING;
			complete(&ioc->ioctl_cmds.done);
			if (ioc->bus_type == SAS)
				ioc->schedule_target_reset(ioc);
		} else {
			ioc->ioctl_cmds.status &= ~MPT_MGMT_STATUS_PENDING;
			complete(&ioc->ioctl_cmds.done);
		}
	}

 out_continuation:
	if (reply && (reply->u.reply.MsgFlags &
	    MPI_MSGFLAGS_CONTINUATION_REPLY))
		return 0;
	return 1;
}