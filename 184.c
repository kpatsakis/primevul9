void QuotaManager::LazyInitialize() {
  DCHECK(io_thread_->BelongsToCurrentThread());
  if (database_.get()) {
    return;
  }

  database_.reset(new QuotaDatabase(is_incognito_ ? FilePath() :
      profile_path_.AppendASCII(kDatabaseName)));

  temporary_usage_tracker_.reset(
      new UsageTracker(clients_, kStorageTypeTemporary,
                       special_storage_policy_));
  persistent_usage_tracker_.reset(
      new UsageTracker(clients_, kStorageTypePersistent,
                       special_storage_policy_));

  scoped_refptr<InitializeTask> task(
      new InitializeTask(this, profile_path_, is_incognito_));
  task->Start();
}
