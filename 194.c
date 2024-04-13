void QuotaManagerProxy::NotifyStorageAccessed(
    QuotaClient::ID client_id,
    const GURL& origin,
    StorageType type) {
  if (!io_thread_->BelongsToCurrentThread()) {
    io_thread_->PostTask(FROM_HERE, NewRunnableMethod(
        this, &QuotaManagerProxy::NotifyStorageAccessed,
        client_id, origin, type));
    return;
  }
  if (manager_)
    manager_->NotifyStorageAccessed(client_id, origin, type);
}
