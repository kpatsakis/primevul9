static void ext4_xattr_update_super_block(handle_t *handle,
					  struct super_block *sb)
{
	if (ext4_has_feature_xattr(sb))
		return;

	BUFFER_TRACE(EXT4_SB(sb)->s_sbh, "get_write_access");
	if (ext4_journal_get_write_access(handle, EXT4_SB(sb)->s_sbh) == 0) {
		ext4_set_feature_xattr(sb);
		ext4_handle_dirty_super(handle, sb);
	}
}