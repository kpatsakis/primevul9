int gg_notify(struct gg_session *sess, uin_t *userlist, int count)
{
	return gg_notify_ex(sess, userlist, NULL, count);
}