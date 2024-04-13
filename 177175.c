static void ext4_xattr_inode_free_quota(struct inode *parent,
					struct inode *ea_inode,
					size_t len)
{
	if (ea_inode &&
	    ext4_test_inode_state(ea_inode, EXT4_STATE_LUSTRE_EA_INODE))
		return;
	dquot_free_space_nodirty(parent, round_up_cluster(parent, len));
	dquot_free_inode(parent);
}