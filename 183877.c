vnc_display_setup_auth(int *auth,
                       int *subauth,
                       QCryptoTLSCreds *tlscreds,
                       bool password,
                       bool sasl,
                       bool websocket,
                       Error **errp)
{
    /*
     * We have a choice of 3 authentication options
     *
     *   1. none
     *   2. vnc
     *   3. sasl
     *
     * The channel can be run in 2 modes
     *
     *   1. clear
     *   2. tls
     *
     * And TLS can use 2 types of credentials
     *
     *   1. anon
     *   2. x509
     *
     * We thus have 9 possible logical combinations
     *
     *   1. clear + none
     *   2. clear + vnc
     *   3. clear + sasl
     *   4. tls + anon + none
     *   5. tls + anon + vnc
     *   6. tls + anon + sasl
     *   7. tls + x509 + none
     *   8. tls + x509 + vnc
     *   9. tls + x509 + sasl
     *
     * These need to be mapped into the VNC auth schemes
     * in an appropriate manner. In regular VNC, all the
     * TLS options get mapped into VNC_AUTH_VENCRYPT
     * sub-auth types.
     *
     * In websockets, the https:// protocol already provides
     * TLS support, so there is no need to make use of the
     * VeNCrypt extension. Furthermore, websockets browser
     * clients could not use VeNCrypt even if they wanted to,
     * as they cannot control when the TLS handshake takes
     * place. Thus there is no option but to rely on https://,
     * meaning combinations 4->6 and 7->9 will be mapped to
     * VNC auth schemes in the same way as combos 1->3.
     *
     * Regardless of fact that we have a different mapping to
     * VNC auth mechs for plain VNC vs websockets VNC, the end
     * result has the same security characteristics.
     */
    if (websocket || !tlscreds) {
        if (password) {
            VNC_DEBUG("Initializing VNC server with password auth\n");
            *auth = VNC_AUTH_VNC;
        } else if (sasl) {
            VNC_DEBUG("Initializing VNC server with SASL auth\n");
            *auth = VNC_AUTH_SASL;
        } else {
            VNC_DEBUG("Initializing VNC server with no auth\n");
            *auth = VNC_AUTH_NONE;
        }
        *subauth = VNC_AUTH_INVALID;
    } else {
        bool is_x509 = object_dynamic_cast(OBJECT(tlscreds),
                                           TYPE_QCRYPTO_TLS_CREDS_X509) != NULL;
        bool is_anon = object_dynamic_cast(OBJECT(tlscreds),
                                           TYPE_QCRYPTO_TLS_CREDS_ANON) != NULL;

        if (!is_x509 && !is_anon) {
            error_setg(errp,
                       "Unsupported TLS cred type %s",
                       object_get_typename(OBJECT(tlscreds)));
            return -1;
        }
        *auth = VNC_AUTH_VENCRYPT;
        if (password) {
            if (is_x509) {
                VNC_DEBUG("Initializing VNC server with x509 password auth\n");
                *subauth = VNC_AUTH_VENCRYPT_X509VNC;
            } else {
                VNC_DEBUG("Initializing VNC server with TLS password auth\n");
                *subauth = VNC_AUTH_VENCRYPT_TLSVNC;
            }

        } else if (sasl) {
            if (is_x509) {
                VNC_DEBUG("Initializing VNC server with x509 SASL auth\n");
                *subauth = VNC_AUTH_VENCRYPT_X509SASL;
            } else {
                VNC_DEBUG("Initializing VNC server with TLS SASL auth\n");
                *subauth = VNC_AUTH_VENCRYPT_TLSSASL;
            }
        } else {
            if (is_x509) {
                VNC_DEBUG("Initializing VNC server with x509 no auth\n");
                *subauth = VNC_AUTH_VENCRYPT_X509NONE;
            } else {
                VNC_DEBUG("Initializing VNC server with TLS no auth\n");
                *subauth = VNC_AUTH_VENCRYPT_TLSNONE;
            }
        }
    }
    return 0;
}