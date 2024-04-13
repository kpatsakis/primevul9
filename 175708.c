static void __exit cleanup_encrypted(void)
{
	encrypted_shash_release();
	unregister_key_type(&key_type_encrypted);
}