ProcXkbLatchLockState(ClientPtr client)
{
    int status;
    DeviceIntPtr dev, tmpd;
    XkbStateRec	oldState,*newState;
    CARD16 changed;
    xkbStateNotify sn;
    XkbEventCauseRec cause;

    REQUEST(xkbLatchLockStateReq);
    REQUEST_SIZE_MATCH(xkbLatchLockStateReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixSetAttrAccess);
    CHK_MASK_MATCH(0x01, stuff->affectModLocks, stuff->modLocks);
    CHK_MASK_MATCH(0x01, stuff->affectModLatches, stuff->modLatches);

    status = Success;

    for (tmpd = inputInfo.devices; tmpd; tmpd = tmpd->next) {
        if ((dev == inputInfo.keyboard && tmpd->key && tmpd->coreEvents) ||
            tmpd == dev) {
            if (!tmpd->key->xkbInfo)
                continue;

            oldState = tmpd->key->xkbInfo->state;
            newState = &tmpd->key->xkbInfo->state;
            if (stuff->affectModLocks) {
                newState->locked_mods &= ~stuff->affectModLocks;
                newState->locked_mods |= (stuff->affectModLocks & stuff->modLocks);
            }
            if (status == Success && stuff->lockGroup)
                newState->locked_group = stuff->groupLock;
            if (status == Success && stuff->affectModLatches)
                status = XkbLatchModifiers(tmpd, stuff->affectModLatches,
                                           stuff->modLatches);
            if (status == Success && stuff->latchGroup)
                status = XkbLatchGroup(tmpd, stuff->groupLatch);

            if (status != Success)
                return status;

            XkbComputeDerivedState(tmpd->key->xkbInfo);
            tmpd->key->state = XkbStateFieldFromRec(newState);

            changed = XkbStateChangedFlags(&oldState, newState);
            if (changed) {
                sn.keycode = 0;
                sn.eventType = 0;
                sn.requestMajor = XkbReqCode;
                sn.requestMinor = X_kbLatchLockState;
                sn.changed = changed;
                XkbSendStateNotify(tmpd, &sn);
                changed = XkbIndicatorsToUpdate(tmpd, changed, False);
                if (changed) {
                    XkbSetCauseXkbReq(&cause, X_kbLatchLockState, client);
                    XkbUpdateIndicators(tmpd, changed, True, NULL, &cause);
	        }
            }
        }
    }

    return client->noClientException;
}