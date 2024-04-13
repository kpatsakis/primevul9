AddSeatId(CallbackListPtr *pcbl, void *data, void *screen)
{
    ScreenPtr pScreen = screen;
    Atom SeatAtom = MakeAtom(SEAT_ATOM_NAME, sizeof(SEAT_ATOM_NAME) - 1, TRUE);
    int err;

    err = dixChangeWindowProperty(serverClient, pScreen->root, SeatAtom,
                                  XA_STRING, 8, PropModeReplace,
                                  strlen(data) + 1, data, FALSE);

    if (err != Success)
        xf86DrvMsg(pScreen->myNum, X_WARNING,
                   "Failed to register seat property\n");
}