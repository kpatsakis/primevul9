smtp_session_init(void)
{
	static int	init = 0;

	if (!init) {
		tree_init(&wait_lka_helo);
		tree_init(&wait_lka_mail);
		tree_init(&wait_lka_rcpt);
		tree_init(&wait_parent_auth);
		tree_init(&wait_queue_msg);
		tree_init(&wait_queue_fd);
		tree_init(&wait_queue_commit);
		tree_init(&wait_ssl_init);
		tree_init(&wait_ssl_verify);
		tree_init(&wait_filters);
		tree_init(&wait_filter_fd);
		init = 1;
	}
}