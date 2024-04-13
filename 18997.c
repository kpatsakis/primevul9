dnsc_handle_curved_request(struct dnsc_env* dnscenv,
                           struct comm_reply* repinfo)
{
    struct comm_point* c = repinfo->c;

    repinfo->is_dnscrypted = 0;
    if( !c->dnscrypt ) {
        return 1;
    }
    // Attempt to decrypt the query. If it is not crypted, we may still need
    // to serve the certificate.
    verbose(VERB_ALGO, "handle request called on DNSCrypt socket");
    if ((repinfo->dnsc_cert = dnsc_find_cert(dnscenv, c->buffer)) != NULL) {
        if(dnscrypt_server_uncurve(dnscenv,
                                   repinfo->dnsc_cert,
                                   repinfo->client_nonce,
                                   repinfo->nmkey,
                                   c->buffer) != 0){
            verbose(VERB_ALGO, "dnscrypt: Failed to uncurve");
            comm_point_drop_reply(repinfo);
            return 0;
        }
        repinfo->is_dnscrypted = 1;
        sldns_buffer_rewind(c->buffer);
    }
    return 1;
}