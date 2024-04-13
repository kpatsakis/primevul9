static int __init init_encrypted(void)
{
	int ret;

	ret = encrypted_shash_alloc();
	if (ret < 0)
		return ret;
	ret = aes_get_sizes();
	if (ret < 0)
		goto out;
	ret = register_key_type(&key_type_encrypted);
	if (ret < 0)
		goto out;
	return 0;
out:
	encrypted_shash_release();
	return ret;

}