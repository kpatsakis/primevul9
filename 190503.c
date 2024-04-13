static inline int update_sits_in_cursum(struct f2fs_journal *journal, int i)
{
	int before = sits_in_cursum(journal);

	journal->n_sits = cpu_to_le16(before + i);
	return before;
}