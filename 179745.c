rfbBool ConnectToRFBRepeater(rfbClient* client,const char *repeaterHost, int repeaterPort, const char *destHost, int destPort)
{
  rfbProtocolVersionMsg pv;
  int major,minor;
  char tmphost[250];

#ifdef LIBVNCSERVER_IPv6
  client->sock = ConnectClientToTcpAddr6WithTimeout(repeaterHost, repeaterPort, client->connectTimeout);
#else
  unsigned int host;
  if (!StringToIPAddr(repeaterHost, &host)) {
    rfbClientLog("Couldn't convert '%s' to host address\n", repeaterHost);
    return FALSE;
  }

  client->sock = ConnectClientToTcpAddrWithTimeout(host, repeaterPort, client->connectTimeout);
#endif

  if (client->sock == RFB_INVALID_SOCKET) {
    rfbClientLog("Unable to connect to VNC repeater\n");
    return FALSE;
  }

  if (!ReadFromRFBServer(client, pv, sz_rfbProtocolVersionMsg))
    return FALSE;
  pv[sz_rfbProtocolVersionMsg] = 0;

  /* UltraVNC repeater always report version 000.000 to identify itself */
  if (sscanf(pv,rfbProtocolVersionFormat,&major,&minor) != 2 || major != 0 || minor != 0) {
    rfbClientLog("Not a valid VNC repeater (%s)\n",pv);
    return FALSE;
  }

  rfbClientLog("Connected to VNC repeater, using protocol version %d.%d\n", major, minor);

  memset(tmphost, 0, sizeof(tmphost));
  if(snprintf(tmphost, sizeof(tmphost), "%s:%d", destHost, destPort) >= (int)sizeof(tmphost))
    return FALSE; /* output truncated */
  if (!WriteToRFBServer(client, tmphost, sizeof(tmphost)))
    return FALSE;

  return TRUE;
}