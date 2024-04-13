static inline void cifs_stats_fail_inc(struct cifs_tcon *tcon, uint16_t code)
{
	cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_failed[code]);
}