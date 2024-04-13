CheckDeviceLedFBs(	DeviceIntPtr			dev,
			int				class,
			int				id,
			xkbGetDeviceInfoReply *		rep,
			ClientPtr			client)
{
int			nFBs= 0;
int			length= 0;
Bool			classOk;

    if (class==XkbDfltXIClass) {
	if (dev->kbdfeed)	class= KbdFeedbackClass;
	else if (dev->leds)	class= LedFeedbackClass;
	else {
	    client->errorValue= _XkbErrCode2(XkbErr_BadClass,class);
	    return XkbKeyboardErrorCode;
	}
    }
    classOk= False;
    if ((dev->kbdfeed)&&((class==KbdFeedbackClass)||(class==XkbAllXIClasses))) {
	KbdFeedbackPtr kf;
	classOk= True;
	for (kf= dev->kbdfeed;(kf);kf=kf->next) {
	    if ((id!=XkbAllXIIds)&&(id!=XkbDfltXIId)&&(id!=kf->ctrl.id))
		continue;
	    nFBs++;
	    length+= SIZEOF(xkbDeviceLedsWireDesc);
	    if (!kf->xkb_sli)
		kf->xkb_sli= XkbAllocSrvLedInfo(dev,kf,NULL,0);
	    length+= ComputeDeviceLedInfoSize(dev,rep->present,kf->xkb_sli);
	    if (id!=XkbAllXIIds)
		break;
	}
    }
    if ((dev->leds)&&((class==LedFeedbackClass)||(class==XkbAllXIClasses))) {
	LedFeedbackPtr lf;
	classOk= True;
	for (lf= dev->leds;(lf);lf=lf->next) {
	    if ((id!=XkbAllXIIds)&&(id!=XkbDfltXIId)&&(id!=lf->ctrl.id))
		continue;
	    nFBs++;
	    length+= SIZEOF(xkbDeviceLedsWireDesc);
	    if (!lf->xkb_sli)
		lf->xkb_sli= XkbAllocSrvLedInfo(dev,NULL,lf,0);
	    length+= ComputeDeviceLedInfoSize(dev,rep->present,lf->xkb_sli);
	    if (id!=XkbAllXIIds)
		break;
	}
    }
    if (nFBs>0) {
	if (rep->supported&XkbXI_IndicatorsMask) {
	    rep->nDeviceLedFBs= nFBs;
	    rep->length+= (length/4);
	}
	return Success;
    }
    if (classOk) client->errorValue= _XkbErrCode2(XkbErr_BadId,id);
    else	 client->errorValue= _XkbErrCode2(XkbErr_BadClass,class);
    return XkbKeyboardErrorCode;
}