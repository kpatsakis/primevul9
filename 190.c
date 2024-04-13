void QuotaManagerProxy::NotifyOriginInUse(
    const GURL& origin) {
  if (!io_thread_->BelongsToCurrentThread()) {
    io_thread_->PostTask(FROM_HERE, NewRunnableMethod(
        this, &QuotaManagerProxy::NotifyOriginInUse, origin));
    return;
  }
  if (manager_)
    manager_->NotifyOriginInUse(origin);
}
