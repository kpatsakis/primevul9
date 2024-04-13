static void selinux_task_to_inode(struct task_struct *p,
				  struct inode *inode)
{
	struct inode_security_struct *isec = selinux_inode(inode);
	u32 sid = task_sid(p);

	spin_lock(&isec->lock);
	isec->sclass = inode_mode_to_security_class(inode->i_mode);
	isec->sid = sid;
	isec->initialized = LABEL_INITIALIZED;
	spin_unlock(&isec->lock);
}