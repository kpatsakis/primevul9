XkbExtensionInit(void)
{
    ExtensionEntry *extEntry;

    if ((extEntry = AddExtension(XkbName, XkbNumberEvents, XkbNumberErrors,
				 ProcXkbDispatch, SProcXkbDispatch,
				 NULL, StandardMinorOpcode))) {
	XkbReqCode = (unsigned char)extEntry->base;
	XkbEventBase = (unsigned char)extEntry->eventBase;
	XkbErrorBase = (unsigned char)extEntry->errorBase;
	XkbKeyboardErrorCode = XkbErrorBase+XkbKeyboard;
	RT_XKBCLIENT = CreateNewResourceType(XkbClientGone);
    }
    return;
}