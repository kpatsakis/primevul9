rfbClientConnectionGone(rfbClientPtr cl)
{
#if defined(LIBVNCSERVER_HAVE_LIBZ) && defined(LIBVNCSERVER_HAVE_LIBJPEG)
    int i;
#endif

    LOCK(rfbClientListMutex);

    if (cl->prev)
        cl->prev->next = cl->next;
    else
        cl->screen->clientHead = cl->next;
    if (cl->next)
        cl->next->prev = cl->prev;

    UNLOCK(rfbClientListMutex);

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    if(cl->screen->backgroundLoop != FALSE) {
      int i;
      do {
	LOCK(cl->refCountMutex);
	i=cl->refCount;
	if(i>0)
	  WAIT(cl->deleteCond,cl->refCountMutex);
	UNLOCK(cl->refCountMutex);
      } while(i>0);
    }
#endif

    if(cl->sock>=0)
	close(cl->sock);

    if (cl->scaledScreen!=NULL)
        cl->scaledScreen->scaledScreenRefCount--;

#ifdef LIBVNCSERVER_HAVE_LIBZ
    rfbFreeZrleData(cl);
#endif

    rfbFreeUltraData(cl);

    /* free buffers holding pixel data before and after encoding */
    free(cl->beforeEncBuf);
    free(cl->afterEncBuf);

    if(cl->sock>=0)
       FD_CLR(cl->sock,&(cl->screen->allFds));

    cl->clientGoneHook(cl);

    rfbLog("Client %s gone\n",cl->host);
    free(cl->host);

#ifdef LIBVNCSERVER_HAVE_LIBZ
    /* Release the compression state structures if any. */
    if ( cl->compStreamInited ) {
	deflateEnd( &(cl->compStream) );
    }

#ifdef LIBVNCSERVER_HAVE_LIBJPEG
    for (i = 0; i < 4; i++) {
	if (cl->zsActive[i])
	    deflateEnd(&cl->zsStruct[i]);
    }
#endif
#endif

    if (cl->screen->pointerClient == cl)
        cl->screen->pointerClient = NULL;

    sraRgnDestroy(cl->modifiedRegion);
    sraRgnDestroy(cl->requestedRegion);
    sraRgnDestroy(cl->copyRegion);

    if (cl->translateLookupTable) free(cl->translateLookupTable);

    TINI_COND(cl->updateCond);
    TINI_MUTEX(cl->updateMutex);

    /* make sure outputMutex is unlocked before destroying */
    LOCK(cl->outputMutex);
    UNLOCK(cl->outputMutex);
    TINI_MUTEX(cl->outputMutex);

    LOCK(cl->sendMutex);
    UNLOCK(cl->sendMutex);
    TINI_MUTEX(cl->sendMutex);

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    close(cl->pipe_notify_client_thread[0]);
    close(cl->pipe_notify_client_thread[1]);
#endif

    rfbPrintStats(cl);
    rfbResetStats(cl);

    free(cl);
}