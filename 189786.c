static struct inode *proc_pid_make_inode(struct super_block * sb, struct task_struct *task)
{
	struct inode * inode;
	struct proc_inode *ei;

	/* We need a new inode */

	inode = new_inode(sb);
	if (!inode)
		goto out;

	/* Common stuff */
	ei = PROC_I(inode);
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	inode->i_op = &proc_def_inode_operations;

	/*
	 * grab the reference to task.
	 */
	ei->pid = get_task_pid(task, PIDTYPE_PID);
	if (!ei->pid)
		goto out_unlock;

	inode->i_uid = 0;
	inode->i_gid = 0;
	if (task_dumpable(task)) {
		inode->i_uid = task->euid;
		inode->i_gid = task->egid;
	}
	security_task_to_inode(task, inode);

out:
	return inode;

out_unlock:
	iput(inode);
	return NULL;
}