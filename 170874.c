static void rb_init_page(struct buffer_data_page *bpage)
{
	local_set(&bpage->commit, 0);
}