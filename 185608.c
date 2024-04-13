static void rb_set_list_to_head(struct list_head *list)
{
	unsigned long *ptr;

	ptr = (unsigned long *)&list->next;
	*ptr |= RB_PAGE_HEAD;
	*ptr &= ~RB_PAGE_UPDATE;
}