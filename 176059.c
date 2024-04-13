static int f2fs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct f2fs_sb_info *sbi = F2FS_SB(sb);
	u64 id = huge_encode_dev(sb->s_bdev->bd_dev);
	block_t total_count, user_block_count, start_count, ovp_count;

	total_count = le64_to_cpu(sbi->raw_super->block_count);
	user_block_count = sbi->user_block_count;
	start_count = le32_to_cpu(sbi->raw_super->segment0_blkaddr);
	ovp_count = SM_I(sbi)->ovp_segments << sbi->log_blocks_per_seg;
	buf->f_type = F2FS_SUPER_MAGIC;
	buf->f_bsize = sbi->blocksize;

	buf->f_blocks = total_count - start_count;
	buf->f_bfree = buf->f_blocks - valid_user_blocks(sbi) - ovp_count;
	buf->f_bavail = user_block_count - valid_user_blocks(sbi);

	buf->f_files = sbi->total_node_count - F2FS_RESERVED_NODE_NUM;
	buf->f_ffree = buf->f_files - valid_inode_count(sbi);

	buf->f_namelen = F2FS_NAME_LEN;
	buf->f_fsid.val[0] = (u32)id;
	buf->f_fsid.val[1] = (u32)(id >> 32);

	return 0;
}