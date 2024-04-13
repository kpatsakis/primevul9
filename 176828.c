cluster_status_get_nick (ClusterStatus status)
{
  switch (status) {
    case CLUSTER_STATUS_NONE:
      return "none";
    case CLUSTER_STATUS_STARTS_WITH_KEYFRAME:
      return "key";
    case CLUSTER_STATUS_STARTS_WITH_DELTAUNIT:
      return "delta";
  }
  return "???";
}