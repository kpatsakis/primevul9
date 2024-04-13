void QuotaManager::NotifyStorageAccessed(
    QuotaClient::ID client_id,
    const GURL& origin, StorageType type) {
  NotifyStorageAccessedInternal(client_id, origin, type, base::Time::Now());
}
