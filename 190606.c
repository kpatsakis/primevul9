static inline void f2fs_put_dnode(struct dnode_of_data *dn)
{
	if (dn->node_page)
		f2fs_put_page(dn->node_page, 1);
	if (dn->inode_page && dn->node_page != dn->inode_page)
		f2fs_put_page(dn->inode_page, 0);
	dn->node_page = NULL;
	dn->inode_page = NULL;
}