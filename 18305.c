size_t hash_and_copy_to_iter(const void *addr, size_t bytes, void *hashp,
		struct iov_iter *i)
{
#ifdef CONFIG_CRYPTO_HASH
	struct ahash_request *hash = hashp;
	struct scatterlist sg;
	size_t copied;

	copied = copy_to_iter(addr, bytes, i);
	sg_init_one(&sg, addr, copied);
	ahash_request_set_crypt(hash, &sg, NULL, copied);
	crypto_ahash_update(hash);
	return copied;
#else
	return 0;
#endif
}