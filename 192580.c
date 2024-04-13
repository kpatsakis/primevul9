hybiPayloadStart(ws_ctx_t *wsctx)
{
  return wsctx->codeBufDecode + wsctx->header.headerLen;
}