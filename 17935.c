SendDeviceLedFBs(	DeviceIntPtr	dev,
			int		class,
			int		id,
			unsigned	wantLength,
			ClientPtr	client)
{
int			length= 0;

    if (class==XkbDfltXIClass) {
	if (dev->kbdfeed)	class= KbdFeedbackClass;
	else if (dev->leds)	class= LedFeedbackClass;
    }
    if ((dev->kbdfeed)&&
	((class==KbdFeedbackClass)||(class==XkbAllXIClasses))) {
	KbdFeedbackPtr kf;
	for (kf= dev->kbdfeed;(kf);kf=kf->next) {
	    if ((id==XkbAllXIIds)||(id==XkbDfltXIId)||(id==kf->ctrl.id)) {
		length+= SendDeviceLedInfo(kf->xkb_sli,client);
		if (id!=XkbAllXIIds)
		    break;
	    }
	}
    }
    if ((dev->leds)&&
	((class==LedFeedbackClass)||(class==XkbAllXIClasses))) {
	LedFeedbackPtr lf;
	for (lf= dev->leds;(lf);lf=lf->next) {
	    if ((id==XkbAllXIIds)||(id==XkbDfltXIId)||(id==lf->ctrl.id)) {
		length+= SendDeviceLedInfo(lf->xkb_sli,client);
		if (id!=XkbAllXIIds)
		    break;
	    }
	}
    }
    if (length==wantLength)
	 return Success;
    else return BadLength;
}