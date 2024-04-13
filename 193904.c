ssize_t elv_iosched_show(struct request_queue *q, char *name)
{
	struct elevator_queue *e = q->elevator;
	struct elevator_type *elv = NULL;
	struct elevator_type *__e;
	int len = 0;

	if (!queue_is_mq(q))
		return sprintf(name, "none\n");

	if (!q->elevator)
		len += sprintf(name+len, "[none] ");
	else
		elv = e->type;

	spin_lock(&elv_list_lock);
	list_for_each_entry(__e, &elv_list, list) {
		if (elv && elevator_match(elv, __e->elevator_name)) {
			len += sprintf(name+len, "[%s] ", elv->elevator_name);
			continue;
		}
		if (elv_support_iosched(q))
			len += sprintf(name+len, "%s ", __e->elevator_name);
	}
	spin_unlock(&elv_list_lock);

	if (q->elevator)
		len += sprintf(name+len, "none");

	len += sprintf(len+name, "\n");
	return len;
}