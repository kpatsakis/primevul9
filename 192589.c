hybiDecodeCleanupComplete(ws_ctx_t *wsctx)
{
  hybiDecodeCleanupBasics(wsctx);
  wsctx->continuation_opcode = WS_OPCODE_INVALID;
  ws_dbg("cleaned up wsctx completely\n");
}