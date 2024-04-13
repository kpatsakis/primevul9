static void rtreeMatchArgFree(void *pArg){
  int i;
  RtreeMatchArg *p = (RtreeMatchArg*)pArg;
  for(i=0; i<p->nParam; i++){
    sqlite3_value_free(p->apSqlParam[i]);
  }
  sqlite3_free(p);
}