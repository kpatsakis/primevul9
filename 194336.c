sg_ioctl(struct file *filp, unsigned int cmd_in, unsigned long arg)
{
	void __user *p = (void __user *)arg;
	int __user *ip = p;
	int result, val, read_only;
	Sg_device *sdp;
	Sg_fd *sfp;
	Sg_request *srp;
	unsigned long iflags;

	if ((!(sfp = (Sg_fd *) filp->private_data)) || (!(sdp = sfp->parentdp)))
		return -ENXIO;

	SCSI_LOG_TIMEOUT(3, sg_printk(KERN_INFO, sdp,
				   "sg_ioctl: cmd=0x%x\n", (int) cmd_in));
	read_only = (O_RDWR != (filp->f_flags & O_ACCMODE));

	switch (cmd_in) {
	case SG_IO:
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		if (!scsi_block_when_processing_errors(sdp->device))
			return -ENXIO;
		if (!access_ok(VERIFY_WRITE, p, SZ_SG_IO_HDR))
			return -EFAULT;
		mutex_lock(&sfp->parentdp->open_rel_lock);
		result = sg_new_write(sfp, filp, p, SZ_SG_IO_HDR,
				 1, read_only, 1, &srp);
		mutex_unlock(&sfp->parentdp->open_rel_lock);
		if (result < 0)
			return result;
		result = wait_event_interruptible(sfp->read_wait,
			(srp_done(sfp, srp) || atomic_read(&sdp->detaching)));
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		write_lock_irq(&sfp->rq_list_lock);
		if (srp->done) {
			srp->done = 2;
			write_unlock_irq(&sfp->rq_list_lock);
			result = sg_new_read(sfp, p, SZ_SG_IO_HDR, srp);
			return (result < 0) ? result : 0;
		}
		srp->orphan = 1;
		write_unlock_irq(&sfp->rq_list_lock);
		return result;	/* -ERESTARTSYS because signal hit process */
	case SG_SET_TIMEOUT:
		result = get_user(val, ip);
		if (result)
			return result;
		if (val < 0)
			return -EIO;
		if (val >= MULDIV (INT_MAX, USER_HZ, HZ))
		    val = MULDIV (INT_MAX, USER_HZ, HZ);
		sfp->timeout_user = val;
		sfp->timeout = MULDIV (val, HZ, USER_HZ);

		return 0;
	case SG_GET_TIMEOUT:	/* N.B. User receives timeout as return value */
				/* strange ..., for backward compatibility */
		return sfp->timeout_user;
	case SG_SET_FORCE_LOW_DMA:
		result = get_user(val, ip);
		if (result)
			return result;
		if (val) {
			sfp->low_dma = 1;
			if ((0 == sfp->low_dma) && (0 == sg_res_in_use(sfp))) {
				val = (int) sfp->reserve.bufflen;
				mutex_lock(&sfp->parentdp->open_rel_lock);
				sg_remove_scat(sfp, &sfp->reserve);
				sg_build_reserve(sfp, val);
				mutex_unlock(&sfp->parentdp->open_rel_lock);
			}
		} else {
			if (atomic_read(&sdp->detaching))
				return -ENODEV;
			sfp->low_dma = sdp->device->host->unchecked_isa_dma;
		}
		return 0;
	case SG_GET_LOW_DMA:
		return put_user((int) sfp->low_dma, ip);
	case SG_GET_SCSI_ID:
		if (!access_ok(VERIFY_WRITE, p, sizeof (sg_scsi_id_t)))
			return -EFAULT;
		else {
			sg_scsi_id_t __user *sg_idp = p;

			if (atomic_read(&sdp->detaching))
				return -ENODEV;
			__put_user((int) sdp->device->host->host_no,
				   &sg_idp->host_no);
			__put_user((int) sdp->device->channel,
				   &sg_idp->channel);
			__put_user((int) sdp->device->id, &sg_idp->scsi_id);
			__put_user((int) sdp->device->lun, &sg_idp->lun);
			__put_user((int) sdp->device->type, &sg_idp->scsi_type);
			__put_user((short) sdp->device->host->cmd_per_lun,
				   &sg_idp->h_cmd_per_lun);
			__put_user((short) sdp->device->queue_depth,
				   &sg_idp->d_queue_depth);
			__put_user(0, &sg_idp->unused[0]);
			__put_user(0, &sg_idp->unused[1]);
			return 0;
		}
	case SG_SET_FORCE_PACK_ID:
		result = get_user(val, ip);
		if (result)
			return result;
		sfp->force_packid = val ? 1 : 0;
		return 0;
	case SG_GET_PACK_ID:
		if (!access_ok(VERIFY_WRITE, ip, sizeof (int)))
			return -EFAULT;
		read_lock_irqsave(&sfp->rq_list_lock, iflags);
		for (srp = sfp->headrp; srp; srp = srp->nextrp) {
			if ((1 == srp->done) && (!srp->sg_io_owned)) {
				read_unlock_irqrestore(&sfp->rq_list_lock,
						       iflags);
				__put_user(srp->header.pack_id, ip);
				return 0;
			}
		}
		read_unlock_irqrestore(&sfp->rq_list_lock, iflags);
		__put_user(-1, ip);
		return 0;
	case SG_GET_NUM_WAITING:
		read_lock_irqsave(&sfp->rq_list_lock, iflags);
		for (val = 0, srp = sfp->headrp; srp; srp = srp->nextrp) {
			if ((1 == srp->done) && (!srp->sg_io_owned))
				++val;
		}
		read_unlock_irqrestore(&sfp->rq_list_lock, iflags);
		return put_user(val, ip);
	case SG_GET_SG_TABLESIZE:
		return put_user(sdp->sg_tablesize, ip);
	case SG_SET_RESERVED_SIZE:
		result = get_user(val, ip);
		if (result)
			return result;
		if (val < 0)
			return -EINVAL;
		val = min_t(int, val,
			    max_sectors_bytes(sdp->device->request_queue));
		if (val != sfp->reserve.bufflen) {
			if (sg_res_in_use(sfp) || sfp->mmap_called)
				return -EBUSY;
			mutex_lock(&sfp->parentdp->open_rel_lock);
			sg_remove_scat(sfp, &sfp->reserve);
			sg_build_reserve(sfp, val);
			mutex_unlock(&sfp->parentdp->open_rel_lock);
		}
		return 0;
	case SG_GET_RESERVED_SIZE:
		val = min_t(int, sfp->reserve.bufflen,
			    max_sectors_bytes(sdp->device->request_queue));
		return put_user(val, ip);
	case SG_SET_COMMAND_Q:
		result = get_user(val, ip);
		if (result)
			return result;
		sfp->cmd_q = val ? 1 : 0;
		return 0;
	case SG_GET_COMMAND_Q:
		return put_user((int) sfp->cmd_q, ip);
	case SG_SET_KEEP_ORPHAN:
		result = get_user(val, ip);
		if (result)
			return result;
		sfp->keep_orphan = val;
		return 0;
	case SG_GET_KEEP_ORPHAN:
		return put_user((int) sfp->keep_orphan, ip);
	case SG_NEXT_CMD_LEN:
		result = get_user(val, ip);
		if (result)
			return result;
		if (val > SG_MAX_CDB_SIZE)
			return -ENOMEM;
		sfp->next_cmd_len = (val > 0) ? val : 0;
		return 0;
	case SG_GET_VERSION_NUM:
		return put_user(sg_version_num, ip);
	case SG_GET_ACCESS_COUNT:
		/* faked - we don't have a real access count anymore */
		val = (sdp->device ? 1 : 0);
		return put_user(val, ip);
	case SG_GET_REQUEST_TABLE:
		if (!access_ok(VERIFY_WRITE, p, SZ_SG_REQ_INFO * SG_MAX_QUEUE))
			return -EFAULT;
		else {
			sg_req_info_t *rinfo;
			unsigned int ms;

			rinfo = kmalloc(SZ_SG_REQ_INFO * SG_MAX_QUEUE,
								GFP_KERNEL);
			if (!rinfo)
				return -ENOMEM;
			read_lock_irqsave(&sfp->rq_list_lock, iflags);
			for (srp = sfp->headrp, val = 0; val < SG_MAX_QUEUE;
			     ++val, srp = srp ? srp->nextrp : srp) {
				memset(&rinfo[val], 0, SZ_SG_REQ_INFO);
				if (srp) {
					rinfo[val].req_state = srp->done + 1;
					rinfo[val].problem =
					    srp->header.masked_status &
					    srp->header.host_status &
					    srp->header.driver_status;
					if (srp->done)
						rinfo[val].duration =
							srp->header.duration;
					else {
						ms = jiffies_to_msecs(jiffies);
						rinfo[val].duration =
						    (ms > srp->header.duration) ?
						    (ms - srp->header.duration) : 0;
					}
					rinfo[val].orphan = srp->orphan;
					rinfo[val].sg_io_owned =
							srp->sg_io_owned;
					rinfo[val].pack_id =
							srp->header.pack_id;
					rinfo[val].usr_ptr =
							srp->header.usr_ptr;
				}
			}
			read_unlock_irqrestore(&sfp->rq_list_lock, iflags);
			result = __copy_to_user(p, rinfo,
						SZ_SG_REQ_INFO * SG_MAX_QUEUE);
			result = result ? -EFAULT : 0;
			kfree(rinfo);
			return result;
		}
	case SG_EMULATED_HOST:
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		return put_user(sdp->device->host->hostt->emulated, ip);
	case SG_SCSI_RESET:
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		if (filp->f_flags & O_NONBLOCK) {
			if (scsi_host_in_recovery(sdp->device->host))
				return -EBUSY;
		} else if (!scsi_block_when_processing_errors(sdp->device))
			return -EBUSY;
		result = get_user(val, ip);
		if (result)
			return result;
		if (SG_SCSI_RESET_NOTHING == val)
			return 0;
		switch (val) {
		case SG_SCSI_RESET_DEVICE:
			val = SCSI_TRY_RESET_DEVICE;
			break;
		case SG_SCSI_RESET_TARGET:
			val = SCSI_TRY_RESET_TARGET;
			break;
		case SG_SCSI_RESET_BUS:
			val = SCSI_TRY_RESET_BUS;
			break;
		case SG_SCSI_RESET_HOST:
			val = SCSI_TRY_RESET_HOST;
			break;
		default:
			return -EINVAL;
		}
		if (!capable(CAP_SYS_ADMIN) || !capable(CAP_SYS_RAWIO))
			return -EACCES;
		return (scsi_reset_provider(sdp->device, val) ==
			SUCCESS) ? 0 : -EIO;
	case SCSI_IOCTL_SEND_COMMAND:
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		if (read_only) {
			unsigned char opcode = WRITE_6;
			Scsi_Ioctl_Command __user *siocp = p;

			if (copy_from_user(&opcode, siocp->data, 1))
				return -EFAULT;
			if (sg_allow_access(filp, &opcode))
				return -EPERM;
		}
		return sg_scsi_ioctl(sdp->device->request_queue, NULL, filp->f_mode, p);
	case SG_SET_DEBUG:
		result = get_user(val, ip);
		if (result)
			return result;
		sdp->sgdebug = (char) val;
		return 0;
	case SCSI_IOCTL_GET_IDLUN:
	case SCSI_IOCTL_GET_BUS_NUMBER:
	case SCSI_IOCTL_PROBE_HOST:
	case SG_GET_TRANSFORM:
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		return scsi_ioctl(sdp->device, cmd_in, p);
	case BLKSECTGET:
		return put_user(max_sectors_bytes(sdp->device->request_queue),
				ip);
	case BLKTRACESETUP:
		return blk_trace_setup(sdp->device->request_queue,
				       sdp->disk->disk_name,
				       MKDEV(SCSI_GENERIC_MAJOR, sdp->index),
				       NULL,
				       (char *)arg);
	case BLKTRACESTART:
		return blk_trace_startstop(sdp->device->request_queue, 1);
	case BLKTRACESTOP:
		return blk_trace_startstop(sdp->device->request_queue, 0);
	case BLKTRACETEARDOWN:
		return blk_trace_remove(sdp->device->request_queue);
	default:
		if (read_only)
			return -EPERM;	/* don't know so take safe approach */
		return scsi_ioctl(sdp->device, cmd_in, p);
	}
}