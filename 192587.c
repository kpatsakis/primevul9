hybiWsFrameComplete(ws_ctx_t *wsctx)
{
  return wsctx != NULL && hybiRemaining(wsctx) == 0;
}