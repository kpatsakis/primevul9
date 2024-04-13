isControlFrame(ws_ctx_t *wsctx)
{
  return 0 != (wsctx->header.opcode & 0x08);
}