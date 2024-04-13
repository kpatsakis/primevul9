sg_read_oxfer(Sg_request * srp, char __user *outp, int num_read_xfer)
{
	Sg_scatter_hold *schp = &srp->data;
	int k, num;

	SCSI_LOG_TIMEOUT(4, sg_printk(KERN_INFO, srp->parentfp->parentdp,
			 "sg_read_oxfer: num_read_xfer=%d\n",
			 num_read_xfer));
	if ((!outp) || (num_read_xfer <= 0))
		return 0;

	num = 1 << (PAGE_SHIFT + schp->page_order);
	for (k = 0; k < schp->k_use_sg && schp->pages[k]; k++) {
		if (num > num_read_xfer) {
			if (__copy_to_user(outp, page_address(schp->pages[k]),
					   num_read_xfer))
				return -EFAULT;
			break;
		} else {
			if (__copy_to_user(outp, page_address(schp->pages[k]),
					   num))
				return -EFAULT;
			num_read_xfer -= num;
			if (num_read_xfer <= 0)
				break;
			outp += num;
		}
	}

	return 0;
}