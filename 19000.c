dnsc_handle_uncurved_request(struct comm_reply *repinfo)
{
    if(!repinfo->c->dnscrypt) {
        return 1;
    }
    sldns_buffer_copy(repinfo->c->dnscrypt_buffer, repinfo->c->buffer);
    if(!repinfo->is_dnscrypted) {
        return 1;
    }
	if(dnscrypt_server_curve(repinfo->dnsc_cert,
                             repinfo->client_nonce,
                             repinfo->nmkey,
                             repinfo->c->dnscrypt_buffer,
                             repinfo->c->type == comm_udp,
                             repinfo->max_udp_size) != 0){
		verbose(VERB_ALGO, "dnscrypt: Failed to curve cached missed answer");
		comm_point_drop_reply(repinfo);
		return 0;
	}
    return 1;
}