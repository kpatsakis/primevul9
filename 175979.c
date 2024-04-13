thunar_transfer_job_progress (goffset  current_num_bytes,
                              goffset  total_num_bytes,
                              gpointer user_data)
{
  guint64 new_percentage;

  ThunarTransferJob *job = user_data;

  _thunar_return_if_fail (THUNAR_IS_TRANSFER_JOB (job));
  
  if (G_LIKELY (job->total_size > 0))
    {
      /* update total progress */
      job->total_progress += (current_num_bytes - job->file_progress);

      /* update file progress */
      job->file_progress = current_num_bytes;

      /* compute the new percentage after the progress we've made */
      new_percentage = (job->total_progress * 100.0) / job->total_size;

      /* notify callers about the progress only if we have advanced by
       * at least 0.01 percent since the last signal emission */
      if (new_percentage >= (job->previous_percentage + 0.01))
        {
          /* emit the percent signal */
          exo_job_percent (EXO_JOB (job), new_percentage);

          /* remember the percentage */
          job->previous_percentage = new_percentage;
        }
    }
}