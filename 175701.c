static int derived_key_encrypt(struct encrypted_key_payload *epayload,
			       const u8 *derived_key,
			       unsigned int derived_keylen)
{
	struct scatterlist sg_in[2];
	struct scatterlist sg_out[1];
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	unsigned int encrypted_datalen;
	u8 iv[AES_BLOCK_SIZE];
	int ret;

	encrypted_datalen = roundup(epayload->decrypted_datalen, blksize);

	req = init_skcipher_req(derived_key, derived_keylen);
	ret = PTR_ERR(req);
	if (IS_ERR(req))
		goto out;
	dump_decrypted_data(epayload);

	sg_init_table(sg_in, 2);
	sg_set_buf(&sg_in[0], epayload->decrypted_data,
		   epayload->decrypted_datalen);
	sg_set_page(&sg_in[1], ZERO_PAGE(0), AES_BLOCK_SIZE, 0);

	sg_init_table(sg_out, 1);
	sg_set_buf(sg_out, epayload->encrypted_data, encrypted_datalen);

	memcpy(iv, epayload->iv, sizeof(iv));
	skcipher_request_set_crypt(req, sg_in, sg_out, encrypted_datalen, iv);
	ret = crypto_skcipher_encrypt(req);
	tfm = crypto_skcipher_reqtfm(req);
	skcipher_request_free(req);
	crypto_free_skcipher(tfm);
	if (ret < 0)
		pr_err("encrypted_key: failed to encrypt (%d)\n", ret);
	else
		dump_encrypted_data(epayload, encrypted_datalen);
out:
	return ret;
}