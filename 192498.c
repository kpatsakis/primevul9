sg_new_read(Sg_fd * sfp, char __user *buf, size_t count, Sg_request * srp)
{
	sg_io_hdr_t *hp = &srp->header;
	int err = 0, err2;
	int len;

	if (count < SZ_SG_IO_HDR) {
		err = -EINVAL;
		goto err_out;
	}
	hp->sb_len_wr = 0;
	if ((hp->mx_sb_len > 0) && hp->sbp) {
		if ((CHECK_CONDITION & hp->masked_status) ||
		    (DRIVER_SENSE & hp->driver_status)) {
			int sb_len = SCSI_SENSE_BUFFERSIZE;
			sb_len = (hp->mx_sb_len > sb_len) ? sb_len : hp->mx_sb_len;
			len = 8 + (int) srp->sense_b[7];	/* Additional sense length field */
			len = (len > sb_len) ? sb_len : len;
			if (copy_to_user(hp->sbp, srp->sense_b, len)) {
				err = -EFAULT;
				goto err_out;
			}
			hp->sb_len_wr = len;
		}
	}
	if (hp->masked_status || hp->host_status || hp->driver_status)
		hp->info |= SG_INFO_CHECK;
	if (copy_to_user(buf, hp, SZ_SG_IO_HDR)) {
		err = -EFAULT;
		goto err_out;
	}
err_out:
	err2 = sg_finish_rem_req(srp);
	return err ? : err2 ? : count;
}