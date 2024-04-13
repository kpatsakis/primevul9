thunar_transfer_job_new (GList                *source_node_list,
                         GList                *target_file_list,
                         ThunarTransferJobType type)
{
  ThunarTransferNode *node;
  ThunarTransferJob  *job;
  GList              *sp;
  GList              *tp;

  _thunar_return_val_if_fail (source_node_list != NULL, NULL);
  _thunar_return_val_if_fail (target_file_list != NULL, NULL);
  _thunar_return_val_if_fail (g_list_length (source_node_list) == g_list_length (target_file_list), NULL);

  job = g_object_new (THUNAR_TYPE_TRANSFER_JOB, NULL);
  job->type = type;

  /* add a transfer node for each source path and a matching target parent path */
  for (sp = source_node_list, tp = target_file_list; 
       sp != NULL; 
       sp = sp->next, tp = tp->next)
    {
      /* make sure we don't transfer root directories. this should be prevented in the GUI */
      if (G_UNLIKELY (thunar_g_file_is_root (sp->data) || thunar_g_file_is_root (tp->data)))
        continue;

      /* only process non-equal pairs unless we're copying */
      if (G_LIKELY (type != THUNAR_TRANSFER_JOB_MOVE || !g_file_equal (sp->data, tp->data)))
        {
          /* append transfer node for this source file */
          node = g_slice_new0 (ThunarTransferNode);
          node->source_file = g_object_ref (sp->data);
          job->source_node_list = g_list_append (job->source_node_list, node);

          /* append target file */
          job->target_file_list = thunar_g_file_list_append (job->target_file_list, tp->data);
        }
    }

  /* make sure we didn't mess things up */
  _thunar_assert (g_list_length (job->source_node_list) == g_list_length (job->target_file_list));

  return THUNAR_JOB (job);
}