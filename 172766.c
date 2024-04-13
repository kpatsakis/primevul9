static int ext4_commit_super(struct super_block *sb, int sync)
{
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	struct buffer_head *sbh = EXT4_SB(sb)->s_sbh;
	int error = 0;

	if (!sbh || block_device_ejected(sb))
		return error;

	/*
	 * The superblock bh should be mapped, but it might not be if the
	 * device was hot-removed. Not much we can do but fail the I/O.
	 */
	if (!buffer_mapped(sbh))
		return error;

	/*
	 * If the file system is mounted read-only, don't update the
	 * superblock write time.  This avoids updating the superblock
	 * write time when we are mounting the root file system
	 * read/only but we need to replay the journal; at that point,
	 * for people who are east of GMT and who make their clock
	 * tick in localtime for Windows bug-for-bug compatibility,
	 * the clock is set in the future, and this will cause e2fsck
	 * to complain and force a full file system check.
	 */
	if (!(sb->s_flags & SB_RDONLY))
		ext4_update_tstamp(es, s_wtime);
	if (sb->s_bdev->bd_part)
		es->s_kbytes_written =
			cpu_to_le64(EXT4_SB(sb)->s_kbytes_written +
			    ((part_stat_read(sb->s_bdev->bd_part,
					     sectors[STAT_WRITE]) -
			      EXT4_SB(sb)->s_sectors_written_start) >> 1));
	else
		es->s_kbytes_written =
			cpu_to_le64(EXT4_SB(sb)->s_kbytes_written);
	if (percpu_counter_initialized(&EXT4_SB(sb)->s_freeclusters_counter))
		ext4_free_blocks_count_set(es,
			EXT4_C2B(EXT4_SB(sb), percpu_counter_sum_positive(
				&EXT4_SB(sb)->s_freeclusters_counter)));
	if (percpu_counter_initialized(&EXT4_SB(sb)->s_freeinodes_counter))
		es->s_free_inodes_count =
			cpu_to_le32(percpu_counter_sum_positive(
				&EXT4_SB(sb)->s_freeinodes_counter));
	BUFFER_TRACE(sbh, "marking dirty");
	ext4_superblock_csum_set(sb);
	if (sync)
		lock_buffer(sbh);
	if (buffer_write_io_error(sbh) || !buffer_uptodate(sbh)) {
		/*
		 * Oh, dear.  A previous attempt to write the
		 * superblock failed.  This could happen because the
		 * USB device was yanked out.  Or it could happen to
		 * be a transient write error and maybe the block will
		 * be remapped.  Nothing we can do but to retry the
		 * write and hope for the best.
		 */
		ext4_msg(sb, KERN_ERR, "previous I/O error to "
		       "superblock detected");
		clear_buffer_write_io_error(sbh);
		set_buffer_uptodate(sbh);
	}
	mark_buffer_dirty(sbh);
	if (sync) {
		unlock_buffer(sbh);
		error = __sync_dirty_buffer(sbh,
			REQ_SYNC | (test_opt(sb, BARRIER) ? REQ_FUA : 0));
		if (buffer_write_io_error(sbh)) {
			ext4_msg(sb, KERN_ERR, "I/O error while writing "
			       "superblock");
			clear_buffer_write_io_error(sbh);
			set_buffer_uptodate(sbh);
		}
	}
	return error;
}