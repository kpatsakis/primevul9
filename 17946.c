_XkbSetIndicatorMap(ClientPtr client, DeviceIntPtr dev,
                    int which, xkbIndicatorMapWireDesc *desc)
{
    XkbSrvInfoPtr       xkbi;
    XkbSrvLedInfoPtr    sli;
    XkbEventCauseRec    cause;
    int                 i, bit;

    xkbi = dev->key->xkbInfo;

    sli= XkbFindSrvLedInfo(dev, XkbDfltXIClass, XkbDfltXIId,
						XkbXI_IndicatorMapsMask);
    if (!sli)
	return BadAlloc;

    for (i = 0, bit = 1; i < XkbNumIndicators; i++, bit <<= 1) {
	if (which & bit) {
	    sli->maps[i].flags = desc->flags;
	    sli->maps[i].which_groups = desc->whichGroups;
	    sli->maps[i].groups = desc->groups;
	    sli->maps[i].which_mods = desc->whichMods;
	    sli->maps[i].mods.mask = desc->mods;
	    sli->maps[i].mods.real_mods = desc->mods;
	    sli->maps[i].mods.vmods= desc->virtualMods;
	    sli->maps[i].ctrls = desc->ctrls;
	    if (desc->virtualMods!=0) {
		unsigned tmp;
		tmp= XkbMaskForVMask(xkbi->desc,desc->virtualMods);
		sli->maps[i].mods.mask= desc->mods|tmp;
	    }
	    desc++;
	}
    }

    XkbSetCauseXkbReq(&cause,X_kbSetIndicatorMap,client);
    XkbApplyLedMapChanges(dev,sli,which,NULL,NULL,&cause);

    return Success;
}