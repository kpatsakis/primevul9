void ext4_resize_end(struct super_block *sb)
{
	clear_bit_unlock(EXT4_RESIZING, &EXT4_SB(sb)->s_resize_flags);
	smp_mb__after_clear_bit();
}