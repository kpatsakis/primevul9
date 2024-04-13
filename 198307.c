struct sctp_chunk *sctp_make_auth(const struct sctp_association *asoc)
{
	struct sctp_authhdr auth_hdr;
	struct sctp_hmac *hmac_desc;
	struct sctp_chunk *retval;

	/* Get the first hmac that the peer told us to use */
	hmac_desc = sctp_auth_asoc_get_hmac(asoc);
	if (unlikely(!hmac_desc))
		return NULL;

	retval = sctp_make_control(asoc, SCTP_CID_AUTH, 0,
				   hmac_desc->hmac_len + sizeof(auth_hdr),
				   GFP_ATOMIC);
	if (!retval)
		return NULL;

	auth_hdr.hmac_id = htons(hmac_desc->hmac_id);
	auth_hdr.shkey_id = htons(asoc->active_key_id);

	retval->subh.auth_hdr = sctp_addto_chunk(retval, sizeof(auth_hdr),
						 &auth_hdr);

	skb_put_zero(retval->skb, hmac_desc->hmac_len);

	/* Adjust the chunk header to include the empty MAC */
	retval->chunk_hdr->length =
		htons(ntohs(retval->chunk_hdr->length) + hmac_desc->hmac_len);
	retval->chunk_end = skb_tail_pointer(retval->skb);

	return retval;
}