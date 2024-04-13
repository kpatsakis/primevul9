hybiDecodeCleanupForContinuation(ws_ctx_t *wsctx)
{
  hybiDecodeCleanupBasics(wsctx);
  ws_dbg("clean up frame, but expect continuation with opcode %d\n", wsctx->continuation_opcode);
}