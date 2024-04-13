_XkbBell(ClientPtr client, DeviceIntPtr dev, WindowPtr pWin,
         int bellClass, int bellID, int pitch, int duration,
         int percent, int forceSound, int eventOnly, Atom name)
{
    int         base;
    pointer     ctrl;
    int         oldPitch, oldDuration;
    int         newPercent;

    if (bellClass == KbdFeedbackClass) {
        KbdFeedbackPtr	k;
        if (bellID==XkbDfltXIId)
            k= dev->kbdfeed;
        else {
            for (k=dev->kbdfeed; k; k=k->next) {
                if (k->ctrl.id == bellID)
                    break;
            }
        }
        if (!k) {
            client->errorValue = _XkbErrCode2(0x5,bellID);
            return BadValue;
        }
        base = k->ctrl.bell;
        ctrl = (pointer) &(k->ctrl);
        oldPitch= k->ctrl.bell_pitch;
        oldDuration= k->ctrl.bell_duration;
        if (pitch!=0) {
            if (pitch==-1)
                k->ctrl.bell_pitch= defaultKeyboardControl.bell_pitch;
            else k->ctrl.bell_pitch= pitch;
        }
        if (duration!=0) {
            if (duration==-1)
                k->ctrl.bell_duration= defaultKeyboardControl.bell_duration;
            else k->ctrl.bell_duration= duration;
        }
    }
    else if (bellClass == BellFeedbackClass) {
        BellFeedbackPtr	b;
        if (bellID==XkbDfltXIId)
            b= dev->bell;
        else {
            for (b=dev->bell; b; b=b->next) {
                if (b->ctrl.id == bellID)
                    break;
            }
        }
        if (!b) {
            client->errorValue = _XkbErrCode2(0x6,bellID);
            return BadValue;
        }
        base = b->ctrl.percent;
        ctrl = (pointer) &(b->ctrl);
        oldPitch= b->ctrl.pitch;
        oldDuration= b->ctrl.duration;
        if (pitch!=0) {
            if (pitch==-1)
                b->ctrl.pitch= defaultKeyboardControl.bell_pitch;
            else b->ctrl.pitch= pitch;
        }
        if (duration!=0) {
            if (duration==-1)
                b->ctrl.duration= defaultKeyboardControl.bell_duration;
            else b->ctrl.duration= duration;
        }
    }
    else {
        client->errorValue = _XkbErrCode2(0x7, bellClass);;
        return BadValue;
    }

    newPercent = (base * percent)/100;
    if (percent < 0)
         newPercent = base + newPercent;
    else newPercent = base - newPercent + percent;

    XkbHandleBell(forceSound, eventOnly,
                  dev, newPercent, ctrl, bellClass,
                  name, pWin, client);
    if ((pitch!=0)||(duration!=0)) {
        if (bellClass == KbdFeedbackClass) {
            KbdFeedbackPtr      k;
            k= (KbdFeedbackPtr)ctrl;
            if (pitch!=0)
                k->ctrl.bell_pitch= oldPitch;
            if (duration!=0)
                k->ctrl.bell_duration= oldDuration;
        }
        else {
            BellFeedbackPtr     b;
            b= (BellFeedbackPtr)ctrl;
            if (pitch!=0)
                b->ctrl.pitch= oldPitch;
            if (duration!=0)
                b->ctrl.duration= oldDuration;
        }
    }

    return Success;
}