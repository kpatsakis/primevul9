mptctl_timeout_expired(MPT_ADAPTER *ioc, MPT_FRAME_HDR *mf)
{
	unsigned long flags;
	int ret_val = -1;
	SCSIIORequest_t *scsi_req = (SCSIIORequest_t *) mf;
	u8 function = mf->u.hdr.Function;

	dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT ": %s\n",
		ioc->name, __func__));

	if (mpt_fwfault_debug)
		mpt_halt_firmware(ioc);

	spin_lock_irqsave(&ioc->taskmgmt_lock, flags);
	if (ioc->ioc_reset_in_progress) {
		spin_unlock_irqrestore(&ioc->taskmgmt_lock, flags);
		CLEAR_MGMT_PENDING_STATUS(ioc->ioctl_cmds.status)
		mpt_free_msg_frame(ioc, mf);
		return;
	}
	spin_unlock_irqrestore(&ioc->taskmgmt_lock, flags);


	CLEAR_MGMT_PENDING_STATUS(ioc->ioctl_cmds.status)

	if (ioc->bus_type == SAS) {
		if (function == MPI_FUNCTION_SCSI_IO_REQUEST)
			ret_val = mptctl_do_taskmgmt(ioc,
				MPI_SCSITASKMGMT_TASKTYPE_TARGET_RESET,
				scsi_req->Bus, scsi_req->TargetID);
		else if (function == MPI_FUNCTION_RAID_SCSI_IO_PASSTHROUGH)
			ret_val = mptctl_do_taskmgmt(ioc,
				MPI_SCSITASKMGMT_TASKTYPE_RESET_BUS,
				scsi_req->Bus, 0);
		if (!ret_val)
			return;
	} else {
		if ((function == MPI_FUNCTION_SCSI_IO_REQUEST) ||
			(function == MPI_FUNCTION_RAID_SCSI_IO_PASSTHROUGH))
			ret_val = mptctl_do_taskmgmt(ioc,
				MPI_SCSITASKMGMT_TASKTYPE_RESET_BUS,
				scsi_req->Bus, 0);
		if (!ret_val)
			return;
	}

	dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT "Calling Reset! \n",
		 ioc->name));
	mpt_Soft_Hard_ResetHandler(ioc, CAN_SLEEP);
	mpt_free_msg_frame(ioc, mf);
}