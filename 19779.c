static int sev_receive_update_data(struct kvm *kvm, struct kvm_sev_cmd *argp)
{
	struct kvm_sev_info *sev = &to_kvm_svm(kvm)->sev_info;
	struct kvm_sev_receive_update_data params;
	struct sev_data_receive_update_data data;
	void *hdr = NULL, *trans = NULL;
	struct page **guest_page;
	unsigned long n;
	int ret, offset;

	if (!sev_guest(kvm))
		return -EINVAL;

	if (copy_from_user(&params, (void __user *)(uintptr_t)argp->data,
			sizeof(struct kvm_sev_receive_update_data)))
		return -EFAULT;

	if (!params.hdr_uaddr || !params.hdr_len ||
	    !params.guest_uaddr || !params.guest_len ||
	    !params.trans_uaddr || !params.trans_len)
		return -EINVAL;

	/* Check if we are crossing the page boundary */
	offset = params.guest_uaddr & (PAGE_SIZE - 1);
	if ((params.guest_len + offset > PAGE_SIZE))
		return -EINVAL;

	hdr = psp_copy_user_blob(params.hdr_uaddr, params.hdr_len);
	if (IS_ERR(hdr))
		return PTR_ERR(hdr);

	trans = psp_copy_user_blob(params.trans_uaddr, params.trans_len);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto e_free_hdr;
	}

	memset(&data, 0, sizeof(data));
	data.hdr_address = __psp_pa(hdr);
	data.hdr_len = params.hdr_len;
	data.trans_address = __psp_pa(trans);
	data.trans_len = params.trans_len;

	/* Pin guest memory */
	guest_page = sev_pin_memory(kvm, params.guest_uaddr & PAGE_MASK,
				    PAGE_SIZE, &n, 1);
	if (IS_ERR(guest_page)) {
		ret = PTR_ERR(guest_page);
		goto e_free_trans;
	}

	/* The RECEIVE_UPDATE_DATA command requires C-bit to be always set. */
	data.guest_address = (page_to_pfn(guest_page[0]) << PAGE_SHIFT) + offset;
	data.guest_address |= sev_me_mask;
	data.guest_len = params.guest_len;
	data.handle = sev->handle;

	ret = sev_issue_cmd(kvm, SEV_CMD_RECEIVE_UPDATE_DATA, &data,
				&argp->error);

	sev_unpin_memory(kvm, guest_page, n);

e_free_trans:
	kfree(trans);
e_free_hdr:
	kfree(hdr);

	return ret;
}