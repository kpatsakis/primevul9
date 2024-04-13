static void rtreeCheckMapping(
  RtreeCheck *pCheck,             /* RtreeCheck object */
  int bLeaf,                      /* True for a leaf cell, false for interior */
  i64 iKey,                       /* Key for mapping */
  i64 iVal                        /* Expected value for mapping */
){
  int rc;
  sqlite3_stmt *pStmt;
  const char *azSql[2] = {
    "SELECT parentnode FROM %Q.'%q_parent' WHERE nodeno=?1",
    "SELECT nodeno FROM %Q.'%q_rowid' WHERE rowid=?1"
  };

  assert( bLeaf==0 || bLeaf==1 );
  if( pCheck->aCheckMapping[bLeaf]==0 ){
    pCheck->aCheckMapping[bLeaf] = rtreeCheckPrepare(pCheck,
        azSql[bLeaf], pCheck->zDb, pCheck->zTab
    );
  }
  if( pCheck->rc!=SQLITE_OK ) return;

  pStmt = pCheck->aCheckMapping[bLeaf];
  sqlite3_bind_int64(pStmt, 1, iKey);
  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_DONE ){
    rtreeCheckAppendMsg(pCheck, "Mapping (%lld -> %lld) missing from %s table",
        iKey, iVal, (bLeaf ? "%_rowid" : "%_parent")
    );
  }else if( rc==SQLITE_ROW ){
    i64 ii = sqlite3_column_int64(pStmt, 0);
    if( ii!=iVal ){
      rtreeCheckAppendMsg(pCheck, 
          "Found (%lld -> %lld) in %s table, expected (%lld -> %lld)",
          iKey, ii, (bLeaf ? "%_rowid" : "%_parent"), iKey, iVal
      );
    }
  }
  rtreeCheckReset(pCheck, pStmt);
}