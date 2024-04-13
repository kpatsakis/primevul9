static void nfs4_layoutcommit_release(void *calldata)
{
	struct nfs4_layoutcommit_data *data = calldata;
	struct pnfs_layout_segment *lseg, *tmp;
	unsigned long *bitlock = &NFS_I(data->args.inode)->flags;

	pnfs_cleanup_layoutcommit(data);
	/* Matched by references in pnfs_set_layoutcommit */
	list_for_each_entry_safe(lseg, tmp, &data->lseg_list, pls_lc_list) {
		list_del_init(&lseg->pls_lc_list);
		if (test_and_clear_bit(NFS_LSEG_LAYOUTCOMMIT,
				       &lseg->pls_flags))
			put_lseg(lseg);
	}

	clear_bit_unlock(NFS_INO_LAYOUTCOMMITTING, bitlock);
	smp_mb__after_clear_bit();
	wake_up_bit(bitlock, NFS_INO_LAYOUTCOMMITTING);

	put_rpccred(data->cred);
	kfree(data);
}