static int rtreeNext(sqlite3_vtab_cursor *pVtabCursor){
  RtreeCursor *pCsr = (RtreeCursor *)pVtabCursor;
  int rc = SQLITE_OK;

  /* Move to the next entry that matches the configured constraints. */
  RTREE_QUEUE_TRACE(pCsr, "POP-Nx:");
  if( pCsr->bAuxValid ){
    pCsr->bAuxValid = 0;
    sqlite3_reset(pCsr->pReadAux);
  }
  rtreeSearchPointPop(pCsr);
  rc = rtreeStepToLeaf(pCsr);
  return rc;
}