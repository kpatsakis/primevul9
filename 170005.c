static int rtreeDisconnect(sqlite3_vtab *pVtab){
  rtreeRelease((Rtree *)pVtab);
  return SQLITE_OK;
}