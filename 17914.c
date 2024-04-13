ProcXkbGetControls(ClientPtr client)
{
    xkbGetControlsReply rep;
    XkbControlsPtr	xkb;
    DeviceIntPtr 	dev;
    register int 	n;

    REQUEST(xkbGetControlsReq);
    REQUEST_SIZE_MATCH(xkbGetControlsReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    
    xkb = dev->key->xkbInfo->desc->ctrls;
    rep.type = X_Reply;
    rep.length = (SIZEOF(xkbGetControlsReply)-
		  SIZEOF(xGenericReply)) >> 2;
    rep.sequenceNumber = client->sequence;
    rep.deviceID = ((DeviceIntPtr)dev)->id;
    rep.numGroups = xkb->num_groups;
    rep.groupsWrap = xkb->groups_wrap;
    rep.internalMods = xkb->internal.mask;
    rep.ignoreLockMods = xkb->ignore_lock.mask;
    rep.internalRealMods = xkb->internal.real_mods;
    rep.ignoreLockRealMods = xkb->ignore_lock.real_mods;
    rep.internalVMods = xkb->internal.vmods;
    rep.ignoreLockVMods = xkb->ignore_lock.vmods;
    rep.enabledCtrls = xkb->enabled_ctrls;
    rep.repeatDelay = xkb->repeat_delay;
    rep.repeatInterval = xkb->repeat_interval;
    rep.slowKeysDelay = xkb->slow_keys_delay;
    rep.debounceDelay = xkb->debounce_delay;
    rep.mkDelay = xkb->mk_delay;
    rep.mkInterval = xkb->mk_interval;
    rep.mkTimeToMax = xkb->mk_time_to_max;
    rep.mkMaxSpeed = xkb->mk_max_speed;
    rep.mkCurve = xkb->mk_curve;
    rep.mkDfltBtn = xkb->mk_dflt_btn;
    rep.axTimeout = xkb->ax_timeout;
    rep.axtCtrlsMask = xkb->axt_ctrls_mask;
    rep.axtCtrlsValues = xkb->axt_ctrls_values;
    rep.axtOptsMask = xkb->axt_opts_mask;
    rep.axtOptsValues = xkb->axt_opts_values;
    rep.axOptions = xkb->ax_options;
    memcpy(rep.perKeyRepeat,xkb->per_key_repeat,XkbPerKeyBitArraySize);
    if (client->swapped) {
    	swaps(&rep.sequenceNumber, n);
	swapl(&rep.length,n);
	swaps(&rep.internalVMods, n);
	swaps(&rep.ignoreLockVMods, n);
	swapl(&rep.enabledCtrls, n);
	swaps(&rep.repeatDelay, n);
	swaps(&rep.repeatInterval, n);
	swaps(&rep.slowKeysDelay, n);
	swaps(&rep.debounceDelay, n);
	swaps(&rep.mkDelay, n);
	swaps(&rep.mkInterval, n);
	swaps(&rep.mkTimeToMax, n);
	swaps(&rep.mkMaxSpeed, n);
	swaps(&rep.mkCurve, n);
	swaps(&rep.axTimeout, n);
	swapl(&rep.axtCtrlsMask, n);
	swapl(&rep.axtCtrlsValues, n);
	swaps(&rep.axtOptsMask, n);
	swaps(&rep.axtOptsValues, n);
	swaps(&rep.axOptions, n);
    }
    WriteToClient(client, SIZEOF(xkbGetControlsReply), (char *)&rep);
    return(client->noClientException);
}