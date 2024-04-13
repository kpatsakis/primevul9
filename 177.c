void QuotaManager::GetStatistics(
    std::map<std::string, std::string>* statistics) {
  DCHECK(statistics);
  if (temporary_storage_evictor_.get()) {
    std::map<std::string, int64> stats;
    temporary_storage_evictor_->GetStatistics(&stats);
    for (std::map<std::string, int64>::iterator p = stats.begin();
         p != stats.end();
         ++p)
      (*statistics)[p->first] = base::Int64ToString(p->second);
  }
}
