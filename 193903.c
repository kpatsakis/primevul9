static void elevator_release(struct kobject *kobj)
{
	struct elevator_queue *e;

	e = container_of(kobj, struct elevator_queue, kobj);
	elevator_put(e->type);
	kfree(e);
}