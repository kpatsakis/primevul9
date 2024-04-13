sg_mmap(struct file *filp, struct vm_area_struct *vma)
{
	Sg_fd *sfp;
	unsigned long req_sz, len, sa;
	Sg_scatter_hold *rsv_schp;
	int k, length;

	if ((!filp) || (!vma) || (!(sfp = (Sg_fd *) filp->private_data)))
		return -ENXIO;
	req_sz = vma->vm_end - vma->vm_start;
	SCSI_LOG_TIMEOUT(3, sg_printk(KERN_INFO, sfp->parentdp,
				      "sg_mmap starting, vm_start=%p, len=%d\n",
				      (void *) vma->vm_start, (int) req_sz));
	if (vma->vm_pgoff)
		return -EINVAL;	/* want no offset */
	rsv_schp = &sfp->reserve;
	if (req_sz > rsv_schp->bufflen)
		return -ENOMEM;	/* cannot map more than reserved buffer */

	sa = vma->vm_start;
	length = 1 << (PAGE_SHIFT + rsv_schp->page_order);
	for (k = 0; k < rsv_schp->k_use_sg && sa < vma->vm_end; k++) {
		len = vma->vm_end - sa;
		len = (len < length) ? len : length;
		sa += len;
	}

	sfp->mmap_called = 1;
	vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_private_data = sfp;
	vma->vm_ops = &sg_mmap_vm_ops;
	return 0;
}