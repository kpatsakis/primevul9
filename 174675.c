AddVTAtoms(CallbackListPtr *pcbl, void *data, void *screen)
{
#define VT_ATOM_NAME         "XFree86_VT"
    int err, HasVT = 1;
    ScreenPtr pScreen = screen;
    Atom VTAtom = MakeAtom(VT_ATOM_NAME, sizeof(VT_ATOM_NAME) - 1, TRUE);
    Atom HasVTAtom = MakeAtom(HAS_VT_ATOM_NAME, sizeof(HAS_VT_ATOM_NAME) - 1,
                              TRUE);

    err = dixChangeWindowProperty(serverClient, pScreen->root, VTAtom,
                                  XA_INTEGER, 32, PropModeReplace, 1,
                                  &xf86Info.vtno, FALSE);

    err |= dixChangeWindowProperty(serverClient, pScreen->root, HasVTAtom,
                                   XA_INTEGER, 32, PropModeReplace, 1,
                                   &HasVT, FALSE);

    if (err != Success)
        xf86DrvMsg(pScreen->myNum, X_WARNING,
                   "Failed to register VT properties\n");
}