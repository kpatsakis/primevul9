struct list_head *seq_list_start_head(struct list_head *head, loff_t pos)
{
	if (!pos)
		return head;

	return seq_list_start(head, pos - 1);
}