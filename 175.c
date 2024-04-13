void QuotaManager::GetOriginsModifiedSince(
    StorageType type,
    base::Time modified_since,
    GetOriginsCallback* callback) {
  LazyInitialize();
  make_scoped_refptr(new GetModifiedSinceTask(
      this, type, modified_since, callback))->Start();
}
