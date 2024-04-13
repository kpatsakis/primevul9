hybiDecodeCleanupBasics(ws_ctx_t *wsctx)
{
  /* keep opcode, cleanup rest */
  wsctx->header.opcode = WS_OPCODE_INVALID;
  wsctx->header.payloadLen = 0;
  wsctx->header.mask.u = 0;
  wsctx->header.headerLen = 0;
  wsctx->header.data = NULL;
  wsctx->header.nRead = 0;
  wsctx->nReadPayload = 0;
  wsctx->carrylen = 0;
  wsctx->readPos = (unsigned char *)wsctx->codeBufDecode;
  wsctx->readlen = 0;
  wsctx->hybiDecodeState = WS_HYBI_STATE_HEADER_PENDING;
  wsctx->writePos = NULL;
}