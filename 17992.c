ProcXkbSetControls(ClientPtr client)
{
    DeviceIntPtr 	dev, tmpd;
    XkbSrvInfoPtr	xkbi;
    XkbControlsPtr	ctrl;
    XkbControlsRec	new,old;
    xkbControlsNotify	cn;
    XkbEventCauseRec	cause;
    XkbSrvLedInfoPtr	sli;

    REQUEST(xkbSetControlsReq);
    REQUEST_SIZE_MATCH(xkbSetControlsReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixManageAccess);
    CHK_MASK_LEGAL(0x01, stuff->changeCtrls, XkbAllControlsMask);

    for (tmpd = inputInfo.keyboard; tmpd; tmpd = tmpd->next) {
        if ((dev == inputInfo.keyboard && tmpd->key && tmpd->coreEvents) ||
            tmpd == dev) {

            xkbi = tmpd->key->xkbInfo;
            ctrl = xkbi->desc->ctrls;
            new = *ctrl;
            XkbSetCauseXkbReq(&cause, X_kbSetControls, client);

            if (stuff->changeCtrls & XkbInternalModsMask) {
                CHK_MASK_MATCH(0x02, stuff->affectInternalMods,
                               stuff->internalMods);
                CHK_MASK_MATCH(0x03, stuff->affectInternalVMods,
                               stuff->internalVMods);

                new.internal.real_mods &= ~(stuff->affectInternalMods);
                new.internal.real_mods |= (stuff->affectInternalMods &
                                           stuff->internalMods);
                new.internal.vmods &= ~(stuff->affectInternalVMods);
                new.internal.vmods |= (stuff->affectInternalVMods &
                                       stuff->internalVMods);
                new.internal.mask = new.internal.real_mods |
                                    XkbMaskForVMask(xkbi->desc,
                                                    new.internal.vmods);
            }

            if (stuff->changeCtrls & XkbIgnoreLockModsMask) {
                CHK_MASK_MATCH(0x4, stuff->affectIgnoreLockMods,
                               stuff->ignoreLockMods);
                CHK_MASK_MATCH(0x5, stuff->affectIgnoreLockVMods,
                               stuff->ignoreLockVMods);

                new.ignore_lock.real_mods &= ~(stuff->affectIgnoreLockMods);
                new.ignore_lock.real_mods |= (stuff->affectIgnoreLockMods &
                                              stuff->ignoreLockMods);
                new.ignore_lock.vmods &= ~(stuff->affectIgnoreLockVMods);
                new.ignore_lock.vmods |= (stuff->affectIgnoreLockVMods &
                                          stuff->ignoreLockVMods);
                new.ignore_lock.mask = new.ignore_lock.real_mods |
                                       XkbMaskForVMask(xkbi->desc,
                                                       new.ignore_lock.vmods);
            }

            CHK_MASK_MATCH(0x06, stuff->affectEnabledCtrls,
                           stuff->enabledCtrls);
            if (stuff->affectEnabledCtrls) {
                CHK_MASK_LEGAL(0x07, stuff->affectEnabledCtrls,
                               XkbAllBooleanCtrlsMask);

                new.enabled_ctrls &= ~(stuff->affectEnabledCtrls);
                new.enabled_ctrls |= (stuff->affectEnabledCtrls &
                                      stuff->enabledCtrls);
            }

            if (stuff->changeCtrls & XkbRepeatKeysMask) {
                if (stuff->repeatDelay < 1 || stuff->repeatInterval < 1) {
                    client->errorValue = _XkbErrCode3(0x08, stuff->repeatDelay,
                                                      stuff->repeatInterval);
                    return BadValue;
                }

                new.repeat_delay = stuff->repeatDelay;
                new.repeat_interval = stuff->repeatInterval;
            }

            if (stuff->changeCtrls & XkbSlowKeysMask) {
                if (stuff->slowKeysDelay < 1) {
                    client->errorValue = _XkbErrCode2(0x09,
                                                      stuff->slowKeysDelay);
                    return BadValue;
                }

                new.slow_keys_delay = stuff->slowKeysDelay;
            }

            if (stuff->changeCtrls & XkbBounceKeysMask) {
                if (stuff->debounceDelay < 1) {
                    client->errorValue = _XkbErrCode2(0x0A,
                                                      stuff->debounceDelay);
                    return BadValue;
                }

                new.debounce_delay = stuff->debounceDelay;
            }

            if (stuff->changeCtrls & XkbMouseKeysMask) {
                if (stuff->mkDfltBtn > XkbMaxMouseKeysBtn) {
                    client->errorValue = _XkbErrCode2(0x0B, stuff->mkDfltBtn);
                    return BadValue;
                }

                new.mk_dflt_btn = stuff->mkDfltBtn;
            }

            if (stuff->changeCtrls & XkbMouseKeysAccelMask) {
                if (stuff->mkDelay < 1 || stuff->mkInterval < 1 ||
                    stuff->mkTimeToMax < 1 || stuff->mkMaxSpeed < 1 ||
                    stuff->mkCurve < -1000) {
                    client->errorValue = _XkbErrCode2(0x0C,0);
                    return BadValue;
                }

                new.mk_delay = stuff->mkDelay;
                new.mk_interval = stuff->mkInterval;
                new.mk_time_to_max = stuff->mkTimeToMax;
                new.mk_max_speed = stuff->mkMaxSpeed;
                new.mk_curve = stuff->mkCurve;
                AccessXComputeCurveFactor(xkbi, &new);
            }

            if (stuff->changeCtrls & XkbGroupsWrapMask) {
                unsigned act, num;

                act = XkbOutOfRangeGroupAction(stuff->groupsWrap);
                switch (act) {
                case XkbRedirectIntoRange:
                    num = XkbOutOfRangeGroupNumber(stuff->groupsWrap);
                    if (num >= new.num_groups) {
                        client->errorValue = _XkbErrCode3(0x0D, new.num_groups,
                                                          num);
                        return BadValue;
                    }
                case XkbWrapIntoRange:
                case XkbClampIntoRange:
                    break;
                default:
                    client->errorValue = _XkbErrCode2(0x0E, act);
                    return BadValue;
                }

                new.groups_wrap= stuff->groupsWrap;
            }

            CHK_MASK_LEGAL(0x0F, stuff->axOptions, XkbAX_AllOptionsMask);
            if (stuff->changeCtrls & XkbAccessXKeysMask) {
                new.ax_options = stuff->axOptions & XkbAX_AllOptionsMask;
            }
            else {
                if (stuff->changeCtrls & XkbStickyKeysMask) {
                    new.ax_options &= ~(XkbAX_SKOptionsMask);
                    new.ax_options |= (stuff->axOptions & XkbAX_SKOptionsMask);
                }
            
                if (stuff->changeCtrls & XkbAccessXFeedbackMask) {
                    new.ax_options &= ~(XkbAX_FBOptionsMask);
                    new.ax_options |= (stuff->axOptions & XkbAX_FBOptionsMask);
                }
            }

            if (stuff->changeCtrls & XkbAccessXTimeoutMask) {
                if (stuff->axTimeout < 1) {
                    client->errorValue = _XkbErrCode2(0x10, stuff->axTimeout);
                    return BadValue;
                }
                CHK_MASK_MATCH(0x11, stuff->axtCtrlsMask,
                               stuff->axtCtrlsValues);
                CHK_MASK_LEGAL(0x12, stuff->axtCtrlsMask,
                               XkbAllBooleanCtrlsMask);
                CHK_MASK_MATCH(0x13, stuff->axtOptsMask, stuff->axtOptsValues);
                CHK_MASK_LEGAL(0x14, stuff->axtOptsMask, XkbAX_AllOptionsMask);
                new.ax_timeout = stuff->axTimeout;
                new.axt_ctrls_mask = stuff->axtCtrlsMask;
                new.axt_ctrls_values = (stuff->axtCtrlsValues &
                                        stuff->axtCtrlsMask);
                new.axt_opts_mask = stuff->axtOptsMask;
                new.axt_opts_values = (stuff->axtOptsValues &
                                       stuff->axtOptsMask);
            }

            if (stuff->changeCtrls & XkbPerKeyRepeatMask)
                memcpy(new.per_key_repeat, stuff->perKeyRepeat,
                       XkbPerKeyBitArraySize);

            old= *ctrl;
            *ctrl= new;
            XkbDDXChangeControls(tmpd, &old, ctrl);

            if (XkbComputeControlsNotify(tmpd, &old, ctrl, &cn, False)) {
                cn.keycode = 0;
                cn.eventType = 0;
                cn.requestMajor = XkbReqCode;
                cn.requestMinor = X_kbSetControls;
                XkbSendControlsNotify(tmpd, &cn);
            }

            sli = XkbFindSrvLedInfo(tmpd, XkbDfltXIClass, XkbDfltXIId, 0);
            if (sli)
                XkbUpdateIndicators(tmpd, sli->usesControls, True, NULL,
                                    &cause);

            /* If sticky keys were disabled, clear all locks and latches */
            if ((old.enabled_ctrls & XkbStickyKeysMask) &&
                !(ctrl->enabled_ctrls & XkbStickyKeysMask))
                XkbClearAllLatchesAndLocks(tmpd, xkbi, True, &cause);
        }
    }

    return client->noClientException;
}