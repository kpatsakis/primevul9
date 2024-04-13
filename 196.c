void QuotaManager::NotifyStorageModified(
    QuotaClient::ID client_id,
    const GURL& origin, StorageType type, int64 delta) {
  NotifyStorageModifiedInternal(client_id, origin, type, delta,
                                base::Time::Now());
}
