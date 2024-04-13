static void print_daily_error_info(struct timer_list *t)
{
	struct ext4_sb_info *sbi = from_timer(sbi, t, s_err_report);
	struct super_block *sb = sbi->s_sb;
	struct ext4_super_block *es = sbi->s_es;

	if (es->s_error_count)
		/* fsck newer than v1.41.13 is needed to clean this condition. */
		ext4_msg(sb, KERN_NOTICE, "error count since last fsck: %u",
			 le32_to_cpu(es->s_error_count));
	if (es->s_first_error_time) {
		printk(KERN_NOTICE "EXT4-fs (%s): initial error at time %llu: %.*s:%d",
		       sb->s_id,
		       ext4_get_tstamp(es, s_first_error_time),
		       (int) sizeof(es->s_first_error_func),
		       es->s_first_error_func,
		       le32_to_cpu(es->s_first_error_line));
		if (es->s_first_error_ino)
			printk(KERN_CONT ": inode %u",
			       le32_to_cpu(es->s_first_error_ino));
		if (es->s_first_error_block)
			printk(KERN_CONT ": block %llu", (unsigned long long)
			       le64_to_cpu(es->s_first_error_block));
		printk(KERN_CONT "\n");
	}
	if (es->s_last_error_time) {
		printk(KERN_NOTICE "EXT4-fs (%s): last error at time %llu: %.*s:%d",
		       sb->s_id,
		       ext4_get_tstamp(es, s_last_error_time),
		       (int) sizeof(es->s_last_error_func),
		       es->s_last_error_func,
		       le32_to_cpu(es->s_last_error_line));
		if (es->s_last_error_ino)
			printk(KERN_CONT ": inode %u",
			       le32_to_cpu(es->s_last_error_ino));
		if (es->s_last_error_block)
			printk(KERN_CONT ": block %llu", (unsigned long long)
			       le64_to_cpu(es->s_last_error_block));
		printk(KERN_CONT "\n");
	}
	mod_timer(&sbi->s_err_report, jiffies + 24*60*60*HZ);  /* Once a day */
}