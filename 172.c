void QuotaManager::GetLRUOrigin(
    StorageType type,
    GetLRUOriginCallback* callback) {
  LazyInitialize();
  DCHECK(!lru_origin_callback_.get());
  lru_origin_callback_.reset(callback);
  if (db_disabled_) {
    lru_origin_callback_->Run(GURL());
    lru_origin_callback_.reset();
    return;
  }
  scoped_refptr<GetLRUOriginTask> task(new GetLRUOriginTask(
      this, type, origins_in_use_,
      origins_in_error_, callback_factory_.NewCallback(
          &QuotaManager::DidGetDatabaseLRUOrigin)));
  task->Start();
}
