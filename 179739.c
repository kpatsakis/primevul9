ReadSupportedSecurityType(rfbClient* client, uint32_t *result, rfbBool subAuth)
{
    uint8_t count=0;
    uint8_t loop=0;
    uint8_t flag=0;
    rfbBool extAuthHandler;
    uint8_t tAuth[256];
    char buf1[500],buf2[10];
    uint32_t authScheme;
    rfbClientProtocolExtension* e;

    if (!ReadFromRFBServer(client, (char *)&count, 1)) return FALSE;

    if (count==0)
    {
        rfbClientLog("List of security types is ZERO, expecting an error to follow\n");
        ReadReason(client);
        return FALSE;
    }

    rfbClientLog("We have %d security types to read\n", count);
    authScheme=0;
    /* now, we have a list of available security types to read ( uint8_t[] ) */
    for (loop=0;loop<count;loop++)
    {
        if (!ReadFromRFBServer(client, (char *)&tAuth[loop], 1)) return FALSE;
        rfbClientLog("%d) Received security type %d\n", loop, tAuth[loop]);
        if (flag) continue;
        extAuthHandler=FALSE;
        for (e = rfbClientExtensions; e; e = e->next) {
            if (!e->handleAuthentication) continue;
            uint32_t const* secType;
            for (secType = e->securityTypes; secType && *secType; secType++) {
                if (tAuth[loop]==*secType) {
                    extAuthHandler=TRUE;
                }
            }
        }
        if (tAuth[loop]==rfbVncAuth || tAuth[loop]==rfbNoAuth ||
			extAuthHandler ||
#if defined(LIBVNCSERVER_HAVE_GNUTLS) || defined(LIBVNCSERVER_HAVE_LIBSSL)
	    (!subAuth && (tAuth[loop]==rfbTLS || tAuth[loop]==rfbVeNCrypt)) ||
#endif
#ifdef LIBVNCSERVER_HAVE_SASL
            tAuth[loop]==rfbSASL ||
#endif /* LIBVNCSERVER_HAVE_SASL */
            (tAuth[loop]==rfbARD && client->GetCredential))
        {
            if (!subAuth && client->clientAuthSchemes)
            {
                int i;
                for (i=0;client->clientAuthSchemes[i];i++)
                {
                    if (client->clientAuthSchemes[i]==(uint32_t)tAuth[loop])
                    {
                        flag++;
                        authScheme=tAuth[loop];
                        break;
                    }
                }
            }
            else
            {
                flag++;
                authScheme=tAuth[loop];
            }
            if (flag)
            {
                rfbClientLog("Selecting security type %d (%d/%d in the list)\n", authScheme, loop, count);
                /* send back a single byte indicating which security type to use */
                if (!WriteToRFBServer(client, (char *)&tAuth[loop], 1)) return FALSE;
            }
        }
    }
    if (authScheme==0)
    {
        memset(buf1, 0, sizeof(buf1));
        for (loop=0;loop<count;loop++)
        {
            if (strlen(buf1)>=sizeof(buf1)-1) break;
            snprintf(buf2, sizeof(buf2), (loop>0 ? ", %d" : "%d"), (int)tAuth[loop]);
            strncat(buf1, buf2, sizeof(buf1)-strlen(buf1)-1);
        }
        rfbClientLog("Unknown authentication scheme from VNC server: %s\n",
               buf1);
        return FALSE;
    }
    *result = authScheme;
    return TRUE;
}