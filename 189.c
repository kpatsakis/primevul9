void QuotaManager::NotifyOriginInUse(const GURL& origin) {
  DCHECK(io_thread_->BelongsToCurrentThread());
  origins_in_use_[origin]++;
}
