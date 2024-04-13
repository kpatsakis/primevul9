void mdesc_update(void)
{
	unsigned long len, real_len, status;
	struct mdesc_handle *hp, *orig_hp;
	unsigned long flags;

	mutex_lock(&mdesc_mutex);

	(void) sun4v_mach_desc(0UL, 0UL, &len);

	hp = mdesc_alloc(len, &kmalloc_mdesc_memops);
	if (!hp) {
		printk(KERN_ERR "MD: mdesc alloc fails\n");
		goto out;
	}

	status = sun4v_mach_desc(__pa(&hp->mdesc), len, &real_len);
	if (status != HV_EOK || real_len > len) {
		printk(KERN_ERR "MD: mdesc reread fails with %lu\n",
		       status);
		refcount_dec(&hp->refcnt);
		mdesc_free(hp);
		goto out;
	}

	spin_lock_irqsave(&mdesc_lock, flags);
	orig_hp = cur_mdesc;
	cur_mdesc = hp;
	spin_unlock_irqrestore(&mdesc_lock, flags);

	mdesc_notify_clients(orig_hp, hp);

	spin_lock_irqsave(&mdesc_lock, flags);
	if (refcount_dec_and_test(&orig_hp->refcnt))
		mdesc_free(orig_hp);
	else
		list_add(&orig_hp->list, &mdesc_zombie_list);
	spin_unlock_irqrestore(&mdesc_lock, flags);

out:
	mutex_unlock(&mdesc_mutex);
}