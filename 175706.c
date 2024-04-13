static void encrypted_rcu_free(struct rcu_head *rcu)
{
	struct encrypted_key_payload *epayload;

	epayload = container_of(rcu, struct encrypted_key_payload, rcu);
	memset(epayload->decrypted_data, 0, epayload->decrypted_datalen);
	kfree(epayload);
}