hook_connect_gnutls_verify_certificates (gnutls_session_t tls_session)
{
    struct t_hook *ptr_hook;
    int rc;

    rc = -1;
    ptr_hook = weechat_hooks[HOOK_TYPE_CONNECT];
    while (ptr_hook)
    {
        /* looking for the right hook using to the gnutls session pointer */
        if (!ptr_hook->deleted
            && HOOK_CONNECT(ptr_hook, gnutls_sess)
            && (*(HOOK_CONNECT(ptr_hook, gnutls_sess)) == tls_session))
        {
            rc = (int) (HOOK_CONNECT(ptr_hook, gnutls_cb))
                (ptr_hook->callback_data, tls_session, NULL, 0,
                 NULL, 0, NULL,
                 WEECHAT_HOOK_CONNECT_GNUTLS_CB_VERIFY_CERT);
            break;
        }
        ptr_hook = ptr_hook->next_hook;
    }

    return rc;
}