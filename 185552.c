static int lock_pi_update_atomic(u32 __user *uaddr, u32 uval, u32 newval)
{
	u32 uninitialized_var(curval);

	if (unlikely(should_fail_futex(true)))
		return -EFAULT;

	if (unlikely(cmpxchg_futex_value_locked(&curval, uaddr, uval, newval)))
		return -EFAULT;

	/*If user space value changed, let the caller retry */
	return curval != uval ? -EAGAIN : 0;
}