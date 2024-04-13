hybiRemaining(ws_ctx_t *wsctx)
{
  return wsctx->header.payloadLen - wsctx->nReadPayload;
}