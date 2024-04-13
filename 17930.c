ProcXkbBell(ClientPtr client)
{
    REQUEST(xkbBellReq);
    DeviceIntPtr dev;
    WindowPtr	 pWin;
    int rc;

    REQUEST_SIZE_MATCH(xkbBellReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_BELL_DEVICE(dev, stuff->deviceSpec, client, DixBellAccess);
    CHK_ATOM_OR_NONE(stuff->name);

    /* device-independent checks request for sane values */
    if ((stuff->forceSound)&&(stuff->eventOnly)) {
	client->errorValue=_XkbErrCode3(0x1,stuff->forceSound,stuff->eventOnly);
	return BadMatch;
    }
    if (stuff->percent < -100 || stuff->percent > 100) {
	client->errorValue = _XkbErrCode2(0x2,stuff->percent);
	return BadValue;
    }
    if (stuff->duration<-1) {
	client->errorValue = _XkbErrCode2(0x3,stuff->duration);
	return BadValue;
    }
    if (stuff->pitch<-1) {
	client->errorValue = _XkbErrCode2(0x4,stuff->pitch);
	return BadValue;
    }

    if (stuff->bellClass == XkbDfltXIClass) {
	if (dev->kbdfeed!=NULL)
	     stuff->bellClass= KbdFeedbackClass;
	else stuff->bellClass= BellFeedbackClass;
    }

    if (stuff->window!=None) {
	rc = dixLookupWindow(&pWin, stuff->window, client, DixGetAttrAccess);
	if (rc != Success) {
	    client->errorValue= stuff->window;
	    return rc;
	}
    }
    else pWin= NULL;

    /* Client wants to ring a bell on the core keyboard?
       Ring the bell on the core keyboard (which does nothing, but if that
       fails the client is screwed anyway), and then on all extension devices.
       Fail if the core keyboard fails but not the extension devices.  this
       may cause some keyboards to ding and others to stay silent. Fix
       your client to use explicit keyboards to avoid this.

       dev is the device the client requested.
     */
    rc = _XkbBell(client, dev, pWin, stuff->bellClass, stuff->bellID,
                  stuff->pitch, stuff->duration, stuff->percent,
                  stuff->forceSound, stuff->eventOnly, stuff->name);

    if ((rc == Success) && ((stuff->deviceSpec == XkbUseCoreKbd) ||
                            (stuff->deviceSpec == XkbUseCorePtr)))
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && other->key && !other->isMaster && (other->u.master == dev))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixBellAccess);
                if (rc == Success)
                    _XkbBell(client, other, pWin, stuff->bellClass,
                             stuff->bellID, stuff->pitch, stuff->duration,
                             stuff->percent, stuff->forceSound,
                             stuff->eventOnly, stuff->name);
            }
        }
        rc = Success; /* reset to success, that's what we got for the VCK */
    }

    return rc;
}