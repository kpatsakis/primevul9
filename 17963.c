_XkbSetNamedIndicator(ClientPtr client, DeviceIntPtr dev,
                      xkbSetNamedIndicatorReq *stuff)
{
    unsigned int                extDevReason;
    unsigned int                statec, namec, mapc;
    XkbSrvLedInfoPtr            sli;
    int                         led = 0;
    XkbIndicatorMapPtr          map;
    DeviceIntPtr                kbd;
    XkbEventCauseRec            cause;
    xkbExtensionDeviceNotify    ed;
    XkbChangesRec               changes;
    int                         rc;

    rc = _XkbCreateIndicatorMap(dev, stuff->indicator, stuff->ledClass,
                                stuff->ledID, &map, &led, FALSE);
    if (rc != Success || !map) /* oh-oh */
        return rc;

    namec = mapc = statec = 0;
    extDevReason = 0;

    namec |= (1<<led);
    sli->namesPresent |= ((stuff->indicator != None) ? (1 << led) : 0);
    extDevReason |= XkbXI_IndicatorNamesMask;

    if (stuff->setMap) {
        map->flags = stuff->flags;
        map->which_groups = stuff->whichGroups;
        map->groups = stuff->groups;
        map->which_mods = stuff->whichMods;
        map->mods.mask = stuff->realMods;
        map->mods.real_mods = stuff->realMods;
        map->mods.vmods= stuff->virtualMods;
        map->ctrls = stuff->ctrls;
        mapc|= (1<<led);
    }

    if ((stuff->setState) && ((map->flags & XkbIM_NoExplicit) == 0))
    {
        if (stuff->on)	sli->explicitState |=  (1<<led);
        else		sli->explicitState &= ~(1<<led);
        statec |= ((sli->effectiveState ^ sli->explicitState) & (1 << led));
    }

    bzero((char *)&ed,sizeof(xkbExtensionDeviceNotify));
    bzero((char *)&changes,sizeof(XkbChangesRec));
    XkbSetCauseXkbReq(&cause,X_kbSetNamedIndicator,client);
    if (namec)
        XkbApplyLedNameChanges(dev,sli,namec,&ed,&changes,&cause);
    if (mapc)
        XkbApplyLedMapChanges(dev,sli,mapc,&ed,&changes,&cause);
    if (statec)
        XkbApplyLedStateChanges(dev,sli,statec,&ed,&changes,&cause);

    kbd = dev;
    if ((sli->flags&XkbSLI_HasOwnState)==0)
        kbd = inputInfo.keyboard;
    XkbFlushLedEvents(dev, kbd, sli, &ed, &changes, &cause);

    return Success;
}