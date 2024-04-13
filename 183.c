  InitializeTemporaryOriginsInfoTask(
      QuotaManager* manager,
      UsageTracker* temporary_usage_tracker)
      : DatabaseTaskBase(manager),
        has_registered_origins_(false) {
    DCHECK(temporary_usage_tracker);
    temporary_usage_tracker->GetCachedOrigins(&origins_);
  }
