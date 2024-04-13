int elv_register_queue(struct request_queue *q)
{
	struct elevator_queue *e = q->elevator;
	int error;

	lockdep_assert_held(&q->sysfs_lock);

	error = kobject_add(&e->kobj, &q->kobj, "%s", "iosched");
	if (!error) {
		struct elv_fs_entry *attr = e->type->elevator_attrs;
		if (attr) {
			while (attr->attr.name) {
				if (sysfs_create_file(&e->kobj, &attr->attr))
					break;
				attr++;
			}
		}
		kobject_uevent(&e->kobj, KOBJ_ADD);
		e->registered = 1;
	}
	return error;
}