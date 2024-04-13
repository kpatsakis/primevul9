xfs_destroy_zones(void)
{
	/*
	 * Make sure all delayed rcu free are flushed before we
	 * destroy caches.
	 */
	rcu_barrier();
	kmem_zone_destroy(xfs_bui_zone);
	kmem_zone_destroy(xfs_bud_zone);
	kmem_zone_destroy(xfs_cui_zone);
	kmem_zone_destroy(xfs_cud_zone);
	kmem_zone_destroy(xfs_rui_zone);
	kmem_zone_destroy(xfs_rud_zone);
	kmem_zone_destroy(xfs_icreate_zone);
	kmem_zone_destroy(xfs_ili_zone);
	kmem_zone_destroy(xfs_inode_zone);
	kmem_zone_destroy(xfs_efi_zone);
	kmem_zone_destroy(xfs_efd_zone);
	kmem_zone_destroy(xfs_buf_item_zone);
	kmem_zone_destroy(xfs_trans_zone);
	kmem_zone_destroy(xfs_ifork_zone);
	kmem_zone_destroy(xfs_da_state_zone);
	kmem_zone_destroy(xfs_btree_cur_zone);
	kmem_zone_destroy(xfs_bmap_free_item_zone);
	kmem_zone_destroy(xfs_log_ticket_zone);
	bioset_free(xfs_ioend_bioset);
}