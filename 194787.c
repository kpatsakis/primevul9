    while (ptr_hook)
    {
        /* looking for the right hook using to the gnutls session pointer */
        if (!ptr_hook->deleted
            && HOOK_CONNECT(ptr_hook, gnutls_sess)
            && (*(HOOK_CONNECT(ptr_hook, gnutls_sess)) == tls_session))
        {
            rc = (int) (HOOK_CONNECT(ptr_hook, gnutls_cb))
                (ptr_hook->callback_data, tls_session, req_ca, nreq,
                 pk_algos, pk_algos_len, answer,
                 WEECHAT_HOOK_CONNECT_GNUTLS_CB_SET_CERT);
            break;
        }
        ptr_hook = ptr_hook->next_hook;
    }