rfbNewTCPOrUDPClient(rfbScreenInfoPtr rfbScreen,
                     int sock,
                     rfbBool isUDP)
{
    rfbProtocolVersionMsg pv;
    rfbClientIteratorPtr iterator;
    rfbClientPtr cl,cl_;
#ifdef LIBVNCSERVER_IPv6
    struct sockaddr_storage addr;
#else
    struct sockaddr_in addr;
#endif
    socklen_t addrlen = sizeof(addr);
    rfbProtocolExtension* extension;

    cl = (rfbClientPtr)calloc(sizeof(rfbClientRec),1);

    cl->screen = rfbScreen;
    cl->sock = sock;
    cl->viewOnly = FALSE;
    /* setup pseudo scaling */
    cl->scaledScreen = rfbScreen;
    cl->scaledScreen->scaledScreenRefCount++;

    rfbResetStats(cl);

    cl->clientData = NULL;
    cl->clientGoneHook = rfbDoNothingWithClient;

    if(isUDP) {
      rfbLog(" accepted UDP client\n");
	} else {
#ifdef LIBVNCSERVER_IPv6
		char host[1024];
#endif
      int one=1;
      size_t otherClientsCount = 0;

      getpeername(sock, (struct sockaddr *)&addr, &addrlen);
#ifdef LIBVNCSERVER_IPv6
      if(getnameinfo((struct sockaddr*)&addr, addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST) != 0) {
	rfbLogPerror("rfbNewClient: error in getnameinfo");
	cl->host = strdup("");
      }
      else
	cl->host = strdup(host);
#else
      cl->host = strdup(inet_ntoa(addr.sin_addr));
#endif

      iterator = rfbGetClientIterator(rfbScreen);
      while ((cl_ = rfbClientIteratorNext(iterator)) != NULL)
	  ++otherClientsCount;
      rfbReleaseClientIterator(iterator);
      rfbLog("  %lu other clients\n", (unsigned long) otherClientsCount);

      if(!rfbSetNonBlocking(sock)) {
	close(sock);
	return NULL;
      }

      if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		     (char *)&one, sizeof(one)) < 0) {
	rfbLogPerror("setsockopt failed: can't set TCP_NODELAY flag, non TCP socket?");
      }

      FD_SET(sock,&(rfbScreen->allFds));
		rfbScreen->maxFd = rfbMax(sock,rfbScreen->maxFd);

      INIT_MUTEX(cl->outputMutex);
      INIT_MUTEX(cl->refCountMutex);
      INIT_MUTEX(cl->sendMutex);
      INIT_COND(cl->deleteCond);

      cl->state = RFB_PROTOCOL_VERSION;

      cl->reverseConnection = FALSE;
      cl->readyForSetColourMapEntries = FALSE;
      cl->useCopyRect = FALSE;
      cl->preferredEncoding = -1;
      cl->correMaxWidth = 48;
      cl->correMaxHeight = 48;
#ifdef LIBVNCSERVER_HAVE_LIBZ
      cl->zrleData = NULL;
#endif

      cl->copyRegion = sraRgnCreate();
      cl->copyDX = 0;
      cl->copyDY = 0;
   
      cl->modifiedRegion =
	sraRgnCreateRect(0,0,rfbScreen->width,rfbScreen->height);

      INIT_MUTEX(cl->updateMutex);
      INIT_COND(cl->updateCond);

      cl->requestedRegion = sraRgnCreate();

      cl->format = cl->screen->serverFormat;
      cl->translateFn = rfbTranslateNone;
      cl->translateLookupTable = NULL;

      LOCK(rfbClientListMutex);

      IF_PTHREADS(cl->refCount = 0);
      cl->next = rfbScreen->clientHead;
      cl->prev = NULL;
      if (rfbScreen->clientHead)
        rfbScreen->clientHead->prev = cl;

      rfbScreen->clientHead = cl;
      UNLOCK(rfbClientListMutex);

#if defined(LIBVNCSERVER_HAVE_LIBZ) || defined(LIBVNCSERVER_HAVE_LIBPNG)
      cl->tightQualityLevel = -1;
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
      cl->tightCompressLevel = TIGHT_DEFAULT_COMPRESSION;
      cl->turboSubsampLevel = TURBO_DEFAULT_SUBSAMP;
      {
	int i;
	for (i = 0; i < 4; i++)
          cl->zsActive[i] = FALSE;
      }
#endif
#endif

      cl->fileTransfer.fd = -1;

      cl->enableCursorShapeUpdates = FALSE;
      cl->enableCursorPosUpdates = FALSE;
      cl->useRichCursorEncoding = FALSE;
      cl->enableLastRectEncoding = FALSE;
      cl->enableKeyboardLedState = FALSE;
      cl->enableSupportedMessages = FALSE;
      cl->enableSupportedEncodings = FALSE;
      cl->enableServerIdentity = FALSE;
      cl->lastKeyboardLedState = -1;
      cl->cursorX = rfbScreen->cursorX;
      cl->cursorY = rfbScreen->cursorY;
      cl->useNewFBSize = FALSE;
      cl->useExtDesktopSize = FALSE;
      cl->requestedDesktopSizeChange = 0;
      cl->lastDesktopSizeChangeError = 0;

#ifdef LIBVNCSERVER_HAVE_LIBZ
      cl->compStreamInited = FALSE;
      cl->compStream.total_in = 0;
      cl->compStream.total_out = 0;
      cl->compStream.zalloc = Z_NULL;
      cl->compStream.zfree = Z_NULL;
      cl->compStream.opaque = Z_NULL;

      cl->zlibCompressLevel = 5;
#endif

      cl->progressiveSliceY = 0;

      cl->extensions = NULL;

      cl->lastPtrX = -1;

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
      cl->pipe_notify_client_thread[0] = -1;
      cl->pipe_notify_client_thread[1] = -1;
#endif

#ifdef LIBVNCSERVER_WITH_WEBSOCKETS
      /*
       * Wait a few ms for the client to send WebSockets connection (TLS/SSL or plain)
       */
      if (!webSocketsCheck(cl)) {
        /* Error reporting handled in webSocketsHandshake */
        rfbCloseClient(cl);
        rfbClientConnectionGone(cl);
        return NULL;
      }
#endif

      sprintf(pv,rfbProtocolVersionFormat,rfbScreen->protocolMajorVersion, 
              rfbScreen->protocolMinorVersion);

      if (rfbWriteExact(cl, pv, sz_rfbProtocolVersionMsg) < 0) {
        rfbLogPerror("rfbNewClient: write");
        rfbCloseClient(cl);
	rfbClientConnectionGone(cl);
        return NULL;
      }
    }

    for(extension = rfbGetExtensionIterator(); extension;
	    extension=extension->next) {
	void* data = NULL;
	/* if the extension does not have a newClient method, it wants
	 * to be initialized later. */
	if(extension->newClient && extension->newClient(cl, &data))
		rfbEnableExtension(cl, extension, data);
    }
    rfbReleaseExtensionIterator();

    switch (cl->screen->newClientHook(cl)) {
    case RFB_CLIENT_ON_HOLD:
	    cl->onHold = TRUE;
	    break;
    case RFB_CLIENT_ACCEPT:
	    cl->onHold = FALSE;
	    break;
    case RFB_CLIENT_REFUSE:
	    rfbCloseClient(cl);
	    rfbClientConnectionGone(cl);
	    cl = NULL;
	    break;
    }
    return cl;
}