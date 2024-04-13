thunar_transfer_job_collect_node (ThunarTransferJob  *job,
                                  ThunarTransferNode *node,
                                  GError            **error)
{
  ThunarTransferNode *child_node;
  GFileInfo          *info;
  GError             *err = NULL;
  GList              *file_list;
  GList              *lp;

  _thunar_return_val_if_fail (THUNAR_IS_TRANSFER_JOB (job), FALSE);
  _thunar_return_val_if_fail (node != NULL && G_IS_FILE (node->source_file), FALSE);
  _thunar_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (exo_job_set_error_if_cancelled (EXO_JOB (job), error))
    return FALSE;

  info = g_file_query_info (node->source_file, 
                            G_FILE_ATTRIBUTE_STANDARD_SIZE ","
                            G_FILE_ATTRIBUTE_STANDARD_TYPE,
                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                            exo_job_get_cancellable (EXO_JOB (job)),
                            &err);

  if (G_UNLIKELY (info == NULL))
    return FALSE;

  job->total_size += g_file_info_get_size (info);

  /* check if we have a directory here */
  if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY)
    {
      /* scan the directory for immediate children */
      file_list = thunar_io_scan_directory (THUNAR_JOB (job), node->source_file,
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            FALSE, FALSE, &err);

      /* add children to the transfer node */
      for (lp = file_list; err == NULL && lp != NULL; lp = lp->next)
        {
          /* allocate a new transfer node for the child */
          child_node = g_slice_new0 (ThunarTransferNode);
          child_node->source_file = g_object_ref (lp->data);

          /* hook the child node into the child list */
          child_node->next = node->children;
          node->children = child_node;

          /* collect the child node */
          thunar_transfer_job_collect_node (job, child_node, &err);
        }
      
      /* release the child files */
      thunar_g_file_list_free (file_list);
    }

  /* release file info */
  g_object_unref (info);

  if (G_UNLIKELY (err != NULL))
    {
      g_propagate_error (error, err);
      return FALSE;
    }

  return TRUE;
}