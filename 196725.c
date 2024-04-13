xfs_init_zones(void)
{
	xfs_ioend_bioset = bioset_create(4 * MAX_BUF_PER_PAGE,
			offsetof(struct xfs_ioend, io_inline_bio),
			BIOSET_NEED_BVECS);
	if (!xfs_ioend_bioset)
		goto out;

	xfs_log_ticket_zone = kmem_zone_init(sizeof(xlog_ticket_t),
						"xfs_log_ticket");
	if (!xfs_log_ticket_zone)
		goto out_free_ioend_bioset;

	xfs_bmap_free_item_zone = kmem_zone_init(
			sizeof(struct xfs_extent_free_item),
			"xfs_bmap_free_item");
	if (!xfs_bmap_free_item_zone)
		goto out_destroy_log_ticket_zone;

	xfs_btree_cur_zone = kmem_zone_init(sizeof(xfs_btree_cur_t),
						"xfs_btree_cur");
	if (!xfs_btree_cur_zone)
		goto out_destroy_bmap_free_item_zone;

	xfs_da_state_zone = kmem_zone_init(sizeof(xfs_da_state_t),
						"xfs_da_state");
	if (!xfs_da_state_zone)
		goto out_destroy_btree_cur_zone;

	xfs_ifork_zone = kmem_zone_init(sizeof(xfs_ifork_t), "xfs_ifork");
	if (!xfs_ifork_zone)
		goto out_destroy_da_state_zone;

	xfs_trans_zone = kmem_zone_init(sizeof(xfs_trans_t), "xfs_trans");
	if (!xfs_trans_zone)
		goto out_destroy_ifork_zone;


	/*
	 * The size of the zone allocated buf log item is the maximum
	 * size possible under XFS.  This wastes a little bit of memory,
	 * but it is much faster.
	 */
	xfs_buf_item_zone = kmem_zone_init(sizeof(struct xfs_buf_log_item),
					   "xfs_buf_item");
	if (!xfs_buf_item_zone)
		goto out_destroy_trans_zone;

	xfs_efd_zone = kmem_zone_init((sizeof(xfs_efd_log_item_t) +
			((XFS_EFD_MAX_FAST_EXTENTS - 1) *
				 sizeof(xfs_extent_t))), "xfs_efd_item");
	if (!xfs_efd_zone)
		goto out_destroy_buf_item_zone;

	xfs_efi_zone = kmem_zone_init((sizeof(xfs_efi_log_item_t) +
			((XFS_EFI_MAX_FAST_EXTENTS - 1) *
				sizeof(xfs_extent_t))), "xfs_efi_item");
	if (!xfs_efi_zone)
		goto out_destroy_efd_zone;

	xfs_inode_zone =
		kmem_zone_init_flags(sizeof(xfs_inode_t), "xfs_inode",
			KM_ZONE_HWALIGN | KM_ZONE_RECLAIM | KM_ZONE_SPREAD |
			KM_ZONE_ACCOUNT, xfs_fs_inode_init_once);
	if (!xfs_inode_zone)
		goto out_destroy_efi_zone;

	xfs_ili_zone =
		kmem_zone_init_flags(sizeof(xfs_inode_log_item_t), "xfs_ili",
					KM_ZONE_SPREAD, NULL);
	if (!xfs_ili_zone)
		goto out_destroy_inode_zone;
	xfs_icreate_zone = kmem_zone_init(sizeof(struct xfs_icreate_item),
					"xfs_icr");
	if (!xfs_icreate_zone)
		goto out_destroy_ili_zone;

	xfs_rud_zone = kmem_zone_init(sizeof(struct xfs_rud_log_item),
			"xfs_rud_item");
	if (!xfs_rud_zone)
		goto out_destroy_icreate_zone;

	xfs_rui_zone = kmem_zone_init(
			xfs_rui_log_item_sizeof(XFS_RUI_MAX_FAST_EXTENTS),
			"xfs_rui_item");
	if (!xfs_rui_zone)
		goto out_destroy_rud_zone;

	xfs_cud_zone = kmem_zone_init(sizeof(struct xfs_cud_log_item),
			"xfs_cud_item");
	if (!xfs_cud_zone)
		goto out_destroy_rui_zone;

	xfs_cui_zone = kmem_zone_init(
			xfs_cui_log_item_sizeof(XFS_CUI_MAX_FAST_EXTENTS),
			"xfs_cui_item");
	if (!xfs_cui_zone)
		goto out_destroy_cud_zone;

	xfs_bud_zone = kmem_zone_init(sizeof(struct xfs_bud_log_item),
			"xfs_bud_item");
	if (!xfs_bud_zone)
		goto out_destroy_cui_zone;

	xfs_bui_zone = kmem_zone_init(
			xfs_bui_log_item_sizeof(XFS_BUI_MAX_FAST_EXTENTS),
			"xfs_bui_item");
	if (!xfs_bui_zone)
		goto out_destroy_bud_zone;

	return 0;

 out_destroy_bud_zone:
	kmem_zone_destroy(xfs_bud_zone);
 out_destroy_cui_zone:
	kmem_zone_destroy(xfs_cui_zone);
 out_destroy_cud_zone:
	kmem_zone_destroy(xfs_cud_zone);
 out_destroy_rui_zone:
	kmem_zone_destroy(xfs_rui_zone);
 out_destroy_rud_zone:
	kmem_zone_destroy(xfs_rud_zone);
 out_destroy_icreate_zone:
	kmem_zone_destroy(xfs_icreate_zone);
 out_destroy_ili_zone:
	kmem_zone_destroy(xfs_ili_zone);
 out_destroy_inode_zone:
	kmem_zone_destroy(xfs_inode_zone);
 out_destroy_efi_zone:
	kmem_zone_destroy(xfs_efi_zone);
 out_destroy_efd_zone:
	kmem_zone_destroy(xfs_efd_zone);
 out_destroy_buf_item_zone:
	kmem_zone_destroy(xfs_buf_item_zone);
 out_destroy_trans_zone:
	kmem_zone_destroy(xfs_trans_zone);
 out_destroy_ifork_zone:
	kmem_zone_destroy(xfs_ifork_zone);
 out_destroy_da_state_zone:
	kmem_zone_destroy(xfs_da_state_zone);
 out_destroy_btree_cur_zone:
	kmem_zone_destroy(xfs_btree_cur_zone);
 out_destroy_bmap_free_item_zone:
	kmem_zone_destroy(xfs_bmap_free_item_zone);
 out_destroy_log_ticket_zone:
	kmem_zone_destroy(xfs_log_ticket_zone);
 out_free_ioend_bioset:
	bioset_free(xfs_ioend_bioset);
 out:
	return -ENOMEM;
}