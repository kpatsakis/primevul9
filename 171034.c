should_copy (GFileAttributeInfo *info,
             gboolean            copy_all_attributes,
             gboolean            skip_perms)
{
  if (skip_perms && strcmp(info->name, "unix::mode") == 0)
        return FALSE;

  if (copy_all_attributes)
    return info->flags & G_FILE_ATTRIBUTE_INFO_COPY_WHEN_MOVED;
  return info->flags & G_FILE_ATTRIBUTE_INFO_COPY_WITH_FILE;
}