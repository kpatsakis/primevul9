instance_create(struct nfnl_queue_net *q, u_int16_t queue_num,
		int portid)
{
	struct nfqnl_instance *inst;
	unsigned int h;
	int err;

	spin_lock(&q->instances_lock);
	if (instance_lookup(q, queue_num)) {
		err = -EEXIST;
		goto out_unlock;
	}

	inst = kzalloc(sizeof(*inst), GFP_ATOMIC);
	if (!inst) {
		err = -ENOMEM;
		goto out_unlock;
	}

	inst->queue_num = queue_num;
	inst->peer_portid = portid;
	inst->queue_maxlen = NFQNL_QMAX_DEFAULT;
	inst->copy_range = NFQNL_MAX_COPY_RANGE;
	inst->copy_mode = NFQNL_COPY_NONE;
	spin_lock_init(&inst->lock);
	INIT_LIST_HEAD(&inst->queue_list);

	if (!try_module_get(THIS_MODULE)) {
		err = -EAGAIN;
		goto out_free;
	}

	h = instance_hashfn(queue_num);
	hlist_add_head_rcu(&inst->hlist, &q->instance_table[h]);

	spin_unlock(&q->instances_lock);

	return inst;

out_free:
	kfree(inst);
out_unlock:
	spin_unlock(&q->instances_lock);
	return ERR_PTR(err);
}