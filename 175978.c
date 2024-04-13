thunar_transfer_job_init (ThunarTransferJob *job)
{
  job->type = 0;
  job->source_node_list = NULL;
  job->target_file_list = NULL;
  job->total_size = 0;
  job->total_progress = 0;
  job->file_progress = 0;
  job->previous_percentage = 0.0;
}