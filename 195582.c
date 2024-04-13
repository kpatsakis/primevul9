static int ext4_mark_dquot_dirty(struct dquot *dquot)
{
	/* Are we journaling quotas? */
	if (EXT4_SB(dquot->dq_sb)->s_qf_names[USRQUOTA] ||
	    EXT4_SB(dquot->dq_sb)->s_qf_names[GRPQUOTA]) {
		dquot_mark_dquot_dirty(dquot);
		return ext4_write_dquot(dquot);
	} else {
		return dquot_mark_dquot_dirty(dquot);
	}
}