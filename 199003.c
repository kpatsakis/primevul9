static void shm_add_rss_swap(struct shmid_kernel *shp,
	unsigned long *rss_add, unsigned long *swp_add)
{
	struct inode *inode;

	inode = file_inode(shp->shm_file);

	if (is_file_hugepages(shp->shm_file)) {
		struct address_space *mapping = inode->i_mapping;
		struct hstate *h = hstate_file(shp->shm_file);
		*rss_add += pages_per_huge_page(h) * mapping->nrpages;
	} else {
#ifdef CONFIG_SHMEM
		struct shmem_inode_info *info = SHMEM_I(inode);

		spin_lock_irq(&info->lock);
		*rss_add += inode->i_mapping->nrpages;
		*swp_add += info->swapped;
		spin_unlock_irq(&info->lock);
#else
		*rss_add += inode->i_mapping->nrpages;
#endif
	}
}