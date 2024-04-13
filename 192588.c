hybiReadHeader(ws_ctx_t *wsctx, int *sockRet, int *nPayload)
{
  int ret;
  char *headerDst = wsctx->codeBufDecode + wsctx->header.nRead;
  int n = ((uint64_t)WSHLENMAX) - wsctx->header.nRead;


  ws_dbg("header_read to %p with len=%d\n", headerDst, n);
  ret = wsctx->ctxInfo.readFunc(wsctx->ctxInfo.ctxPtr, headerDst, n);
  ws_dbg("read %d bytes from socket\n", ret);
  if (ret <= 0) {
    if (-1 == ret) {
      /* save errno because rfbErr() will tamper it */
      int olderrno = errno;
      rfbErr("%s: read; %s\n", __func__, strerror(errno));
      errno = olderrno;
      goto err_cleanup_state;
    } else {
      *sockRet = 0;
      goto err_cleanup_state_sock_closed;
    }
  }

  wsctx->header.nRead += ret;
  if (wsctx->header.nRead < 2) {
    /* cannot decode header with less than two bytes */
    goto ret_header_pending;
  }

  /* first two header bytes received; interpret header data and get rest */
  wsctx->header.data = (ws_header_t *)wsctx->codeBufDecode;

  wsctx->header.opcode = wsctx->header.data->b0 & 0x0f;
  wsctx->header.fin = (wsctx->header.data->b0 & 0x80) >> 7;
  if (isControlFrame(wsctx)) {
    ws_dbg("is control frame\n");
    /* is a control frame, leave remembered continuation opcode unchanged;
     * just check if there is a wrong fragmentation */
    if (wsctx->header.fin == 0) {

      /* we only accept text/binary continuation frames; RFC6455:
       * Control frames (see Section 5.5) MAY be injected in the middle of
       * a fragmented message.  Control frames themselves MUST NOT be
       * fragmented. */
      rfbErr("control frame with FIN bit cleared received, aborting\n");
      errno = EPROTO;
      goto err_cleanup_state;
    }
  } else {
    ws_dbg("not a control frame\n");
    /* not a control frame, check for continuation opcode */
    if (wsctx->header.opcode == WS_OPCODE_CONTINUATION) {
      ws_dbg("cont_frame\n");
      /* do we have state (i.e., opcode) for continuation frame? */
      if (wsctx->continuation_opcode == WS_OPCODE_INVALID) {
        rfbErr("no continuation state\n");
        errno = EPROTO;
        goto err_cleanup_state;
      }

      /* otherwise, set opcode = continuation_opcode */
      wsctx->header.opcode = wsctx->continuation_opcode;
      ws_dbg("set opcode to continuation_opcode: %d\n", wsctx->header.opcode);
    } else {
      if (wsctx->header.fin == 0) {
        wsctx->continuation_opcode = wsctx->header.opcode;
      } else {
        wsctx->continuation_opcode = WS_OPCODE_INVALID;
      }
      ws_dbg("set continuation_opcode to %d\n", wsctx->continuation_opcode);
    }
  }

  wsctx->header.payloadLen = (uint64_t)(wsctx->header.data->b1 & 0x7f);
  ws_dbg("first header bytes received; opcode=%d lenbyte=%d fin=%d\n", wsctx->header.opcode, wsctx->header.payloadLen, wsctx->header.fin);

  /*
   * 4.3. Client-to-Server Masking
   *
   * The client MUST mask all frames sent to the server.  A server MUST
   * close the connection upon receiving a frame with the MASK bit set to 0.
  **/
  if (!(wsctx->header.data->b1 & 0x80)) {
    rfbErr("%s: got frame without mask; ret=%d\n", __func__, ret);
    errno = EPROTO;
    goto err_cleanup_state;
  }


  if (wsctx->header.payloadLen < 126 && wsctx->header.nRead >= 6) {
    wsctx->header.headerLen = WS_HYBI_HEADER_LEN_SHORT;
    wsctx->header.mask = wsctx->header.data->u.m;
  } else if (wsctx->header.payloadLen == 126 && 8 <= wsctx->header.nRead) {
    wsctx->header.headerLen = WS_HYBI_HEADER_LEN_EXTENDED;
    wsctx->header.payloadLen = WS_NTOH16(wsctx->header.data->u.s16.l16);
    wsctx->header.mask = wsctx->header.data->u.s16.m16;
  } else if (wsctx->header.payloadLen == 127 && 14 <= wsctx->header.nRead) {
    wsctx->header.headerLen = WS_HYBI_HEADER_LEN_LONG;
    wsctx->header.payloadLen = WS_NTOH64(wsctx->header.data->u.s64.l64);
    wsctx->header.mask = wsctx->header.data->u.s64.m64;
  } else {
    /* Incomplete frame header, try again */
    rfbErr("%s: incomplete frame header; ret=%d\n", __func__, ret);
    goto ret_header_pending;
  }

  char *h = wsctx->codeBufDecode;
  int i;
  ws_dbg("Header:\n");
  for (i=0; i <10; i++) {
    ws_dbg("0x%02X\n", (unsigned char)h[i]);
  }
  ws_dbg("\n");

  /* while RFC 6455 mandates that lengths MUST be encoded with the minimum
   * number of bytes, it does not specify for the server how to react on
   * 'wrongly' encoded frames --- this implementation rejects them*/
  if ((wsctx->header.headerLen > WS_HYBI_HEADER_LEN_SHORT
      && wsctx->header.payloadLen < (uint64_t)126)
      || (wsctx->header.headerLen > WS_HYBI_HEADER_LEN_EXTENDED
        && wsctx->header.payloadLen < (uint64_t)65536)) {
    rfbErr("%s: invalid length field; headerLen=%d payloadLen=%llu\n", __func__, wsctx->header.headerLen, wsctx->header.payloadLen);
    errno = EPROTO;
    goto err_cleanup_state;
  }

  /* update write position for next bytes */
  wsctx->writePos = wsctx->codeBufDecode + wsctx->header.nRead;

  /* set payload pointer just after header */
  wsctx->readPos = (unsigned char *)(wsctx->codeBufDecode + wsctx->header.headerLen);

  *nPayload = wsctx->header.nRead - wsctx->header.headerLen;
  wsctx->nReadPayload = *nPayload;

  ws_dbg("header complete: state=%d headerlen=%d payloadlen=%llu writeTo=%p nPayload=%d\n", wsctx->hybiDecodeState, wsctx->header.headerLen, wsctx->header.payloadLen, wsctx->writePos, *nPayload);

  return WS_HYBI_STATE_DATA_NEEDED;

ret_header_pending:
  errno = EAGAIN;
  *sockRet = -1;
  return WS_HYBI_STATE_HEADER_PENDING;

err_cleanup_state:
  *sockRet = -1;
err_cleanup_state_sock_closed:
  hybiDecodeCleanupComplete(wsctx);
  return WS_HYBI_STATE_ERR;
}