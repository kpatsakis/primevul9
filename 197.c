void QuotaManagerProxy::NotifyStorageModified(
    QuotaClient::ID client_id,
    const GURL& origin,
    StorageType type,
    int64 delta) {
  if (!io_thread_->BelongsToCurrentThread()) {
    io_thread_->PostTask(FROM_HERE, NewRunnableMethod(
        this, &QuotaManagerProxy::NotifyStorageModified,
        client_id, origin, type, delta));
    return;
  }
  if (manager_)
    manager_->NotifyStorageModified(client_id, origin, type, delta);
}
