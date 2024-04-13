thunar_transfer_job_finalize (GObject *object)
{
  ThunarTransferJob *job = THUNAR_TRANSFER_JOB (object);

  g_list_foreach (job->source_node_list, (GFunc) thunar_transfer_node_free, NULL);
  g_list_free (job->source_node_list);

  thunar_g_file_list_free (job->target_file_list);

  (*G_OBJECT_CLASS (thunar_transfer_job_parent_class)->finalize) (object);
}