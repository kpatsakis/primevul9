AbortDDX(enum ExitCode error)
{
    int i;

    OsBlockSIGIO();

    /*
     * try to restore the original video state
     */
#ifdef DPMSExtension            /* Turn screens back on */
    if (DPMSPowerLevel != DPMSModeOn)
        DPMSSet(serverClient, DPMSModeOn);
#endif
    if (xf86Screens) {
        for (i = 0; i < xf86NumScreens; i++)
            if (xf86Screens[i]->vtSema) {
                /*
                 * if we are aborting before ScreenInit() has finished
                 * we might not have been wrapped yet. Therefore enable
                 * screen explicitely.
                 */
                xf86VGAarbiterLock(xf86Screens[i]);
                (xf86Screens[i]->LeaveVT) (xf86Screens[i]);
                xf86VGAarbiterUnlock(xf86Screens[i]);
            }
    }

    xf86AccessLeave();

    /*
     * This is needed for an abnormal server exit, since the normal exit stuff
     * MUST also be performed (i.e. the vt must be left in a defined state)
     */
    ddxGiveUp(error);
}