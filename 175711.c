static int derived_key_decrypt(struct encrypted_key_payload *epayload,
			       const u8 *derived_key,
			       unsigned int derived_keylen)
{
	struct scatterlist sg_in[1];
	struct scatterlist sg_out[2];
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	unsigned int encrypted_datalen;
	u8 iv[AES_BLOCK_SIZE];
	u8 *pad;
	int ret;

	/* Throwaway buffer to hold the unused zero padding at the end */
	pad = kmalloc(AES_BLOCK_SIZE, GFP_KERNEL);
	if (!pad)
		return -ENOMEM;

	encrypted_datalen = roundup(epayload->decrypted_datalen, blksize);
	req = init_skcipher_req(derived_key, derived_keylen);
	ret = PTR_ERR(req);
	if (IS_ERR(req))
		goto out;
	dump_encrypted_data(epayload, encrypted_datalen);

	sg_init_table(sg_in, 1);
	sg_init_table(sg_out, 2);
	sg_set_buf(sg_in, epayload->encrypted_data, encrypted_datalen);
	sg_set_buf(&sg_out[0], epayload->decrypted_data,
		   epayload->decrypted_datalen);
	sg_set_buf(&sg_out[1], pad, AES_BLOCK_SIZE);

	memcpy(iv, epayload->iv, sizeof(iv));
	skcipher_request_set_crypt(req, sg_in, sg_out, encrypted_datalen, iv);
	ret = crypto_skcipher_decrypt(req);
	tfm = crypto_skcipher_reqtfm(req);
	skcipher_request_free(req);
	crypto_free_skcipher(tfm);
	if (ret < 0)
		goto out;
	dump_decrypted_data(epayload);
out:
	kfree(pad);
	return ret;
}