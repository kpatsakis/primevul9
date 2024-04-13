static void rtreeCheckCount(RtreeCheck *pCheck, const char *zTbl, i64 nExpect){
  if( pCheck->rc==SQLITE_OK ){
    sqlite3_stmt *pCount;
    pCount = rtreeCheckPrepare(pCheck, "SELECT count(*) FROM %Q.'%q%s'",
        pCheck->zDb, pCheck->zTab, zTbl
    );
    if( pCount ){
      if( sqlite3_step(pCount)==SQLITE_ROW ){
        i64 nActual = sqlite3_column_int64(pCount, 0);
        if( nActual!=nExpect ){
          rtreeCheckAppendMsg(pCheck, "Wrong number of entries in %%%s table"
              " - expected %lld, actual %lld" , zTbl, nExpect, nActual
          );
        }
      }
      pCheck->rc = sqlite3_finalize(pCount);
    }
  }
}