ProcXkbSelectEvents(ClientPtr client)
{
    unsigned		legal;
    DeviceIntPtr 	dev;
    XkbInterestPtr	masks;
    REQUEST(xkbSelectEventsReq);

    REQUEST_AT_LEAST_SIZE(xkbSelectEventsReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_ANY_DEVICE(dev, stuff->deviceSpec, client, DixUseAccess);

    if (((stuff->affectWhich&XkbMapNotifyMask)!=0)&&(stuff->affectMap)) {
	client->mapNotifyMask&= ~stuff->affectMap;
	client->mapNotifyMask|= (stuff->affectMap&stuff->map);
    }
    if ((stuff->affectWhich&(~XkbMapNotifyMask))==0) 
	return client->noClientException;

    masks = XkbFindClientResource((DevicePtr)dev,client);
    if (!masks){
	XID id = FakeClientID(client->index);
	AddResource(id,RT_XKBCLIENT,dev);
	masks= XkbAddClientResource((DevicePtr)dev,client,id);
    }
    if (masks) {
	union {
	    CARD8	*c8;
	    CARD16	*c16;
	    CARD32	*c32;
	} from,to;
	register unsigned bit,ndx,maskLeft,dataLeft,size;

	from.c8= (CARD8 *)&stuff[1];
	dataLeft= (stuff->length*4)-SIZEOF(xkbSelectEventsReq);
	maskLeft= (stuff->affectWhich&(~XkbMapNotifyMask));
	for (ndx=0,bit=1; (maskLeft!=0); ndx++, bit<<=1) {
	    if ((bit&maskLeft)==0)
		continue;
	    maskLeft&= ~bit;
	    switch (ndx) {
		case XkbNewKeyboardNotify:
		    to.c16= &client->newKeyboardNotifyMask;
		    legal= XkbAllNewKeyboardEventsMask;
		    size= 2;
		    break;
		case XkbStateNotify:
		    to.c16= &masks->stateNotifyMask;
		    legal= XkbAllStateEventsMask;
		    size= 2;
		    break;
		case XkbControlsNotify:
		    to.c32= &masks->ctrlsNotifyMask;
		    legal= XkbAllControlEventsMask;
		    size= 4;
		    break;
		case XkbIndicatorStateNotify:
		    to.c32= &masks->iStateNotifyMask;
		    legal= XkbAllIndicatorEventsMask;
		    size= 4;
		    break;
		case XkbIndicatorMapNotify:
		    to.c32= &masks->iMapNotifyMask;
		    legal= XkbAllIndicatorEventsMask;
		    size= 4;
		    break;
		case XkbNamesNotify:
		    to.c16= &masks->namesNotifyMask;
		    legal= XkbAllNameEventsMask;
		    size= 2;
		    break;
		case XkbCompatMapNotify:
		    to.c8= &masks->compatNotifyMask;
		    legal= XkbAllCompatMapEventsMask;
		    size= 1;
		    break;
		case XkbBellNotify:
		    to.c8= &masks->bellNotifyMask;
		    legal= XkbAllBellEventsMask;
		    size= 1;
		    break;
		case XkbActionMessage:
		    to.c8= &masks->actionMessageMask;
		    legal= XkbAllActionMessagesMask;
		    size= 1;
		    break;
		case XkbAccessXNotify:
		    to.c16= &masks->accessXNotifyMask;
		    legal= XkbAllAccessXEventsMask;
		    size= 2;
		    break;
		case XkbExtensionDeviceNotify:
		    to.c16= &masks->extDevNotifyMask;
		    legal= XkbAllExtensionDeviceEventsMask;
		    size= 2;
		    break;
		default:
		    client->errorValue = _XkbErrCode2(33,bit);
		    return BadValue;
	    }

	    if (stuff->clear&bit) {
		if (size==2)		to.c16[0]= 0;
		else if (size==4)	to.c32[0]= 0;
		else			to.c8[0]=  0;
	    }
	    else if (stuff->selectAll&bit) {
		if (size==2)		to.c16[0]= ~0;
		else if (size==4)	to.c32[0]= ~0;
		else			to.c8[0]=  ~0;
	    }
	    else {
		if (dataLeft<(size*2))
		    return BadLength;
		if (size==2) {
		    CHK_MASK_MATCH(ndx,from.c16[0],from.c16[1]);
		    CHK_MASK_LEGAL(ndx,from.c16[0],legal);
		    to.c16[0]&= ~from.c16[0];
		    to.c16[0]|= (from.c16[0]&from.c16[1]);
		}
		else if (size==4) {
		    CHK_MASK_MATCH(ndx,from.c32[0],from.c32[1]);
		    CHK_MASK_LEGAL(ndx,from.c32[0],legal);
		    to.c32[0]&= ~from.c32[0];
		    to.c32[0]|= (from.c32[0]&from.c32[1]);
		}
		else  {
		    CHK_MASK_MATCH(ndx,from.c8[0],from.c8[1]);
		    CHK_MASK_LEGAL(ndx,from.c8[0],legal);
		    to.c8[0]&= ~from.c8[0];
		    to.c8[0]|= (from.c8[0]&from.c8[1]);
		    size= 2;
		}
		from.c8+= (size*2);
		dataLeft-= (size*2);
	    }
	}
	if (dataLeft>2) {
	    ErrorF("[xkb] Extra data (%d bytes) after SelectEvents\n",dataLeft);
	    return BadLength;
	}
	return client->noClientException;
    }
    return BadAlloc;
}