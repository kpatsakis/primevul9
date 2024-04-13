static void *__bpf_copy_key(void __user *ukey, u64 key_size)
{
	if (key_size)
		return memdup_user(ukey, key_size);

	if (ukey)
		return ERR_PTR(-EINVAL);

	return NULL;
}