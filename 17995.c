ProcXkbGetDeviceInfo(ClientPtr client)
{
DeviceIntPtr		dev;
xkbGetDeviceInfoReply	rep;
int			status,nDeviceLedFBs;
unsigned		length,nameLen;
CARD16			ledClass,ledID;
unsigned		wanted,supported;
char *			str;

    REQUEST(xkbGetDeviceInfoReq);
    REQUEST_SIZE_MATCH(xkbGetDeviceInfoReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    wanted= stuff->wanted;

    CHK_ANY_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_MASK_LEGAL(0x01,wanted,XkbXI_AllDeviceFeaturesMask);

    if ((!dev->button)||((stuff->nBtns<1)&&(!stuff->allBtns)))
	wanted&= ~XkbXI_ButtonActionsMask;
    if ((!dev->kbdfeed)&&(!dev->leds))
	wanted&= ~XkbXI_IndicatorsMask;

    nameLen= XkbSizeCountedString(dev->name);
    bzero((char *)&rep,SIZEOF(xkbGetDeviceInfoReply));
    rep.type = X_Reply;
    rep.deviceID= dev->id;
    rep.sequenceNumber = client->sequence;
    rep.length = nameLen/4;
    rep.present = wanted;
    rep.supported = XkbXI_AllDeviceFeaturesMask;
    rep.unsupported = 0;
    rep.firstBtnWanted = rep.nBtnsWanted = 0;
    rep.firstBtnRtrn = rep.nBtnsRtrn = 0;
    if (dev->button)
	 rep.totalBtns= dev->button->numButtons;
    else rep.totalBtns= 0;
    rep.devType=	dev->type;
    rep.hasOwnState=	(dev->key && dev->key->xkbInfo);
    rep.nDeviceLedFBs = 0;
    if (dev->kbdfeed)	rep.dfltKbdFB= dev->kbdfeed->ctrl.id;
    else		rep.dfltKbdFB= XkbXINone;
    if (dev->leds)	rep.dfltLedFB= dev->leds->ctrl.id;
    else		rep.dfltLedFB= XkbXINone;

    ledClass= stuff->ledClass;
    ledID= stuff->ledID;

    rep.firstBtnWanted= rep.nBtnsWanted= 0;
    rep.firstBtnRtrn= rep.nBtnsRtrn= 0;
    if (wanted&XkbXI_ButtonActionsMask) {
	if (stuff->allBtns) {
	    stuff->firstBtn= 0;
	    stuff->nBtns= dev->button->numButtons;
	}

	if ((stuff->firstBtn+stuff->nBtns)>dev->button->numButtons) {
	    client->errorValue = _XkbErrCode4(0x02,dev->button->numButtons,
							stuff->firstBtn,
							stuff->nBtns);
	    return BadValue;
	}
	else {
	    rep.firstBtnWanted= stuff->firstBtn;
	    rep.nBtnsWanted= stuff->nBtns;
	    if (dev->button->xkb_acts!=NULL) {
		XkbAction *act;
		register int i;

		rep.firstBtnRtrn= stuff->firstBtn;
		rep.nBtnsRtrn= stuff->nBtns;
		act= &dev->button->xkb_acts[rep.firstBtnWanted];
		for (i=0;i<rep.nBtnsRtrn;i++,act++) {
		    if (act->type!=XkbSA_NoAction)
			break;
		}
		rep.firstBtnRtrn+=	i;
		rep.nBtnsRtrn-=		i;
		act= &dev->button->xkb_acts[rep.firstBtnRtrn+rep.nBtnsRtrn-1];
		for (i=0;i<rep.nBtnsRtrn;i++,act--) {
		    if (act->type!=XkbSA_NoAction)
			break;
		}
		rep.nBtnsRtrn-=		i;
	    }
	    rep.length+= (rep.nBtnsRtrn*SIZEOF(xkbActionWireDesc))/4;
	}
    }

    if (wanted&XkbXI_IndicatorsMask) {
	status= CheckDeviceLedFBs(dev,ledClass,ledID,&rep,client);
	if (status!=Success)
	    return status;
    }
    length= rep.length*4;
    supported= rep.supported;
    nDeviceLedFBs = rep.nDeviceLedFBs;
    if (client->swapped) {
	register int n;
	swaps(&rep.sequenceNumber,n);
	swapl(&rep.length,n);
	swaps(&rep.present,n);
	swaps(&rep.supported,n);
	swaps(&rep.unsupported,n);
	swaps(&rep.nDeviceLedFBs,n);
	swapl(&rep.type,n);
    }
    WriteToClient(client,SIZEOF(xkbGetDeviceInfoReply), (char *)&rep);

    str= (char*) xalloc(nameLen);
    if (!str) 
	return BadAlloc;
    XkbWriteCountedString(str,dev->name,client->swapped);
    WriteToClient(client,nameLen,str);
    xfree(str);
    length-= nameLen;

    if (rep.nBtnsRtrn>0) {
	int			sz;
	xkbActionWireDesc *	awire;
	sz= rep.nBtnsRtrn*SIZEOF(xkbActionWireDesc);
	awire= (xkbActionWireDesc *)&dev->button->xkb_acts[rep.firstBtnRtrn];
	WriteToClient(client,sz,(char *)awire);
	length-= sz;
    }
    if (nDeviceLedFBs>0) {
	status= SendDeviceLedFBs(dev,ledClass,ledID,length,client);
	if (status!=Success)
	    return status;
    }
    else if (length!=0)  {
	ErrorF("[xkb] Internal Error!  BadLength in ProcXkbGetDeviceInfo\n");
	ErrorF("[xkb]                  Wrote %d fewer bytes than expected\n",length);
	return BadLength;
    }
    if (stuff->wanted&(~supported)) {
	xkbExtensionDeviceNotify ed;
	bzero((char *)&ed,SIZEOF(xkbExtensionDeviceNotify));
	ed.ledClass=		ledClass;
	ed.ledID=		ledID;
	ed.ledsDefined= 	0;
	ed.ledState=		0;
	ed.firstBtn= ed.nBtns=	0;
	ed.reason=		XkbXI_UnsupportedFeatureMask;
	ed.supported=		supported;
	ed.unsupported=		stuff->wanted&(~supported);
	XkbSendExtensionDeviceNotify(dev,client,&ed);
    }
    return client->noClientException;
}