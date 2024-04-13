rfbSendSupportedMessages(rfbClientPtr cl)
{
    rfbFramebufferUpdateRectHeader rect;
    rfbSupportedMessages msgs;

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader
                  + sz_rfbSupportedMessages > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }

    rect.encoding = Swap32IfLE(rfbEncodingSupportedMessages);
    rect.r.x = 0;
    rect.r.y = 0;
    rect.r.w = Swap16IfLE(sz_rfbSupportedMessages);
    rect.r.h = 0;

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
        sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    memset((char *)&msgs, 0, sz_rfbSupportedMessages);
    rfbSetBit(msgs.client2server, rfbSetPixelFormat);
    rfbSetBit(msgs.client2server, rfbFixColourMapEntries);
    rfbSetBit(msgs.client2server, rfbSetEncodings);
    rfbSetBit(msgs.client2server, rfbFramebufferUpdateRequest);
    rfbSetBit(msgs.client2server, rfbKeyEvent);
    rfbSetBit(msgs.client2server, rfbPointerEvent);
    rfbSetBit(msgs.client2server, rfbClientCutText);
    rfbSetBit(msgs.client2server, rfbFileTransfer);
    rfbSetBit(msgs.client2server, rfbSetScale);
    /*rfbSetBit(msgs.client2server, rfbSetServerInput);  */
    /*rfbSetBit(msgs.client2server, rfbSetSW);           */
    /*rfbSetBit(msgs.client2server, rfbTextChat);        */
    rfbSetBit(msgs.client2server, rfbPalmVNCSetScaleFactor);

    rfbSetBit(msgs.server2client, rfbFramebufferUpdate);
    rfbSetBit(msgs.server2client, rfbSetColourMapEntries);
    rfbSetBit(msgs.server2client, rfbBell);
    rfbSetBit(msgs.server2client, rfbServerCutText);
    rfbSetBit(msgs.server2client, rfbResizeFrameBuffer);
    rfbSetBit(msgs.server2client, rfbPalmVNCReSizeFrameBuffer);

    if (cl->screen->xvpHook) {
        rfbSetBit(msgs.client2server, rfbXvp);
        rfbSetBit(msgs.server2client, rfbXvp);
    }

    memcpy(&cl->updateBuf[cl->ublen], (char *)&msgs, sz_rfbSupportedMessages);
    cl->ublen += sz_rfbSupportedMessages;

    rfbStatRecordEncodingSent(cl, rfbEncodingSupportedMessages,
        sz_rfbFramebufferUpdateRectHeader+sz_rfbSupportedMessages,
        sz_rfbFramebufferUpdateRectHeader+sz_rfbSupportedMessages);
    if (!rfbSendUpdateBuf(cl))
        return FALSE;

    return TRUE;
}