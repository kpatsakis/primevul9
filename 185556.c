futex_wake_op(u32 __user *uaddr1, unsigned int flags, u32 __user *uaddr2,
	      int nr_wake, int nr_wake2, int op)
{
	union futex_key key1 = FUTEX_KEY_INIT, key2 = FUTEX_KEY_INIT;
	struct futex_hash_bucket *hb1, *hb2;
	struct futex_q *this, *next;
	int ret, op_ret;
	WAKE_Q(wake_q);

retry:
	ret = get_futex_key(uaddr1, flags & FLAGS_SHARED, &key1, VERIFY_READ);
	if (unlikely(ret != 0))
		goto out;
	ret = get_futex_key(uaddr2, flags & FLAGS_SHARED, &key2, VERIFY_WRITE);
	if (unlikely(ret != 0))
		goto out_put_key1;

	hb1 = hash_futex(&key1);
	hb2 = hash_futex(&key2);

retry_private:
	double_lock_hb(hb1, hb2);
	op_ret = futex_atomic_op_inuser(op, uaddr2);
	if (unlikely(op_ret < 0)) {

		double_unlock_hb(hb1, hb2);

#ifndef CONFIG_MMU
		/*
		 * we don't get EFAULT from MMU faults if we don't have an MMU,
		 * but we might get them from range checking
		 */
		ret = op_ret;
		goto out_put_keys;
#endif

		if (unlikely(op_ret != -EFAULT)) {
			ret = op_ret;
			goto out_put_keys;
		}

		ret = fault_in_user_writeable(uaddr2);
		if (ret)
			goto out_put_keys;

		if (!(flags & FLAGS_SHARED))
			goto retry_private;

		put_futex_key(&key2);
		put_futex_key(&key1);
		goto retry;
	}

	plist_for_each_entry_safe(this, next, &hb1->chain, list) {
		if (match_futex (&this->key, &key1)) {
			if (this->pi_state || this->rt_waiter) {
				ret = -EINVAL;
				goto out_unlock;
			}
			mark_wake_futex(&wake_q, this);
			if (++ret >= nr_wake)
				break;
		}
	}

	if (op_ret > 0) {
		op_ret = 0;
		plist_for_each_entry_safe(this, next, &hb2->chain, list) {
			if (match_futex (&this->key, &key2)) {
				if (this->pi_state || this->rt_waiter) {
					ret = -EINVAL;
					goto out_unlock;
				}
				mark_wake_futex(&wake_q, this);
				if (++op_ret >= nr_wake2)
					break;
			}
		}
		ret += op_ret;
	}

out_unlock:
	double_unlock_hb(hb1, hb2);
	wake_up_q(&wake_q);
out_put_keys:
	put_futex_key(&key2);
out_put_key1:
	put_futex_key(&key1);
out:
	return ret;
}