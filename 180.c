void QuotaManager::GetUsageAndQuotaForEviction(
    GetUsageAndQuotaForEvictionCallback* callback) {
  DCHECK(io_thread_->BelongsToCurrentThread());
  DCHECK(!eviction_context_.get_usage_and_quota_callback.get());

  eviction_context_.get_usage_and_quota_callback.reset(callback);
  GetGlobalUsage(kStorageTypeTemporary, callback_factory_.
      NewCallback(&QuotaManager::DidGetGlobalUsageForEviction));
}
