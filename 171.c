int64 GetInitialTemporaryStorageQuotaSize(const FilePath& path,
                                          bool is_incognito) {
  int64 free_space = base::SysInfo::AmountOfFreeDiskSpace(path);
  UMA_HISTOGRAM_MBYTES("Quota.FreeDiskSpaceForProfile", free_space);

  if (free_space < QuotaManager::kTemporaryStorageQuotaDefaultSize * 2)
    return 0;

  if (is_incognito)
    return QuotaManager::kIncognitoDefaultTemporaryQuota;

  if (free_space < QuotaManager::kTemporaryStorageQuotaDefaultSize * 20)
    return QuotaManager::kTemporaryStorageQuotaDefaultSize;

  if (free_space < QuotaManager::kTemporaryStorageQuotaMaxSize * 20)
    return free_space / 20;

  return QuotaManager::kTemporaryStorageQuotaMaxSize;
}
