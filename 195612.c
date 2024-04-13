static void ext4_clear_journal_err(struct super_block *sb,
				   struct ext4_super_block *es)
{
	journal_t *journal;
	int j_errno;
	const char *errstr;

	BUG_ON(!EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_HAS_JOURNAL));

	journal = EXT4_SB(sb)->s_journal;

	/*
	 * Now check for any error status which may have been recorded in the
	 * journal by a prior ext4_error() or ext4_abort()
	 */

	j_errno = jbd2_journal_errno(journal);
	if (j_errno) {
		char nbuf[16];

		errstr = ext4_decode_error(sb, j_errno, nbuf);
		ext4_warning(sb, "Filesystem error recorded "
			     "from previous mount: %s", errstr);
		ext4_warning(sb, "Marking fs in need of filesystem check.");

		EXT4_SB(sb)->s_mount_state |= EXT4_ERROR_FS;
		es->s_state |= cpu_to_le16(EXT4_ERROR_FS);
		ext4_commit_super(sb, 1);

		jbd2_journal_clear_err(journal);
	}
}