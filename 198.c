void QuotaManager::NotifyStorageModifiedInternal(
    QuotaClient::ID client_id,
    const GURL& origin,
    StorageType type,
    int64 delta,
    base::Time modified_time) {
  LazyInitialize();
  GetUsageTracker(type)->UpdateUsageCache(client_id, origin, delta);
  make_scoped_refptr(new UpdateModifiedTimeTask(
      this, origin, type, modified_time))->Start();
}
