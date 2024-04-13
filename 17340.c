static __always_inline void set_root_rcu(struct nameidata *nd)
{
	if (!nd->root.mnt) {
		struct fs_struct *fs = current->fs;
		unsigned seq;

		do {
			seq = read_seqcount_begin(&fs->seq);
			nd->root = fs->root;
			nd->seq = __read_seqcount_begin(&nd->root.dentry->d_seq);
		} while (read_seqcount_retry(&fs->seq, seq));
	}
}