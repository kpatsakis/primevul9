void QuotaManagerProxy::NotifyOriginNoLongerInUse(
    const GURL& origin) {
  if (!io_thread_->BelongsToCurrentThread()) {
    io_thread_->PostTask(FROM_HERE, NewRunnableMethod(
        this, &QuotaManagerProxy::NotifyOriginNoLongerInUse, origin));
    return;
  }
  if (manager_)
    manager_->NotifyOriginNoLongerInUse(origin);
}
