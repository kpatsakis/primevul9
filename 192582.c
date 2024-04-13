hybiReturnData(char *dst, int len, ws_ctx_t *wsctx, int *nWritten)
{
  int nextState = WS_HYBI_STATE_ERR;

  /* if we have something already decoded copy and return */
  if (wsctx->readlen > 0) {
    /* simply return what we have */
    if (wsctx->readlen > len) {
      ws_dbg("copy to %d bytes to dst buffer; readPos=%p, readLen=%d\n", len, wsctx->readPos, wsctx->readlen);
      memcpy(dst, wsctx->readPos, len);
      *nWritten = len;
      wsctx->readlen -= len;
      wsctx->readPos += len;
      nextState = WS_HYBI_STATE_DATA_AVAILABLE;
    } else {
      ws_dbg("copy to %d bytes to dst buffer; readPos=%p, readLen=%d\n", wsctx->readlen, wsctx->readPos, wsctx->readlen);
      memcpy(dst, wsctx->readPos, wsctx->readlen);
      *nWritten = wsctx->readlen;
      wsctx->readlen = 0;
      wsctx->readPos = NULL;
      if (hybiRemaining(wsctx) == 0) {
        nextState = WS_HYBI_STATE_FRAME_COMPLETE;
      } else {
        nextState = WS_HYBI_STATE_DATA_NEEDED;
      }
    }
    ws_dbg("after copy: readPos=%p, readLen=%d\n", wsctx->readPos, wsctx->readlen);
  } else {
    /* it may happen that we read some bytes but could not decode them,
     * in that case, set errno to EAGAIN and return -1 */
    nextState = wsctx->hybiDecodeState;
    errno = EAGAIN;
    *nWritten = -1;
  }
  return nextState;
}