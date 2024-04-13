  InitializeTask(
      QuotaManager* manager,
      const FilePath& profile_path,
      bool is_incognito)
      : DatabaseTaskBase(manager),
        profile_path_(profile_path),
        is_incognito_(is_incognito),
        need_initialize_origins_(false),
        temporary_storage_quota_(-1) {
  }
