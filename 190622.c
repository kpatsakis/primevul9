static inline void set_new_dnode(struct dnode_of_data *dn, struct inode *inode,
		struct page *ipage, struct page *npage, nid_t nid)
{
	memset(dn, 0, sizeof(*dn));
	dn->inode = inode;
	dn->inode_page = ipage;
	dn->node_page = npage;
	dn->nid = nid;
}