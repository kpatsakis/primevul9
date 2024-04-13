static int rtreeClose(sqlite3_vtab_cursor *cur){
  Rtree *pRtree = (Rtree *)(cur->pVtab);
  int ii;
  RtreeCursor *pCsr = (RtreeCursor *)cur;
  assert( pRtree->nCursor>0 );
  freeCursorConstraints(pCsr);
  sqlite3_finalize(pCsr->pReadAux);
  sqlite3_free(pCsr->aPoint);
  for(ii=0; ii<RTREE_CACHE_SZ; ii++) nodeRelease(pRtree, pCsr->aNode[ii]);
  sqlite3_free(pCsr);
  pRtree->nCursor--;
  nodeBlobReset(pRtree);
  return SQLITE_OK;
}