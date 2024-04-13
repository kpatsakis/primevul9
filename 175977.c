ttj_copy_file (ThunarTransferJob *job,
               GFile             *source_file,
               GFile             *target_file,
               GFileCopyFlags     copy_flags,
               gboolean           merge_directories,
               GError           **error)
{
  GFileType source_type;
  GFileType target_type;
  gboolean  target_exists;
  GError   *err = NULL;

  _thunar_return_val_if_fail (THUNAR_IS_TRANSFER_JOB (job), FALSE);
  _thunar_return_val_if_fail (G_IS_FILE (source_file), FALSE);
  _thunar_return_val_if_fail (G_IS_FILE (target_file), FALSE);
  _thunar_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* reset the file progress */
  job->file_progress = 0;

  if (exo_job_set_error_if_cancelled (EXO_JOB (job), error))
    return FALSE;

  source_type = g_file_query_file_type (source_file, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        exo_job_get_cancellable (EXO_JOB (job)));

  if (exo_job_set_error_if_cancelled (EXO_JOB (job), error))
    return FALSE;

  target_type = g_file_query_file_type (target_file, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        exo_job_get_cancellable (EXO_JOB (job)));

  if (exo_job_set_error_if_cancelled (EXO_JOB (job), error))
    return FALSE;

  /* check if the target is a symlink and we are in overwrite mode */
  if (target_type == G_FILE_TYPE_SYMBOLIC_LINK && (copy_flags & G_FILE_COPY_OVERWRITE) != 0)
    {
      /* try to delete the symlink */
      if (!g_file_delete (target_file, exo_job_get_cancellable (EXO_JOB (job)), &err))
        {
          g_propagate_error (error, err);
          return FALSE;
        }
    }

  /* try to copy the file */
  g_file_copy (source_file, target_file, copy_flags,
               exo_job_get_cancellable (EXO_JOB (job)),
               thunar_transfer_job_progress, job, &err);

  /* check if there were errors */
  if (G_UNLIKELY (err != NULL && err->domain == G_IO_ERROR))
    {
      if (err->code == G_IO_ERROR_WOULD_MERGE 
          || (err->code == G_IO_ERROR_EXISTS 
              && source_type == G_FILE_TYPE_DIRECTORY
              && target_type == G_FILE_TYPE_DIRECTORY))
        {
          /* we tried to overwrite a directory with a directory. this normally results 
           * in a merge. ignore the error if we actually *want* to merge */
          if (merge_directories)
            g_clear_error (&err);
        }
      else if (err->code == G_IO_ERROR_WOULD_RECURSE)
        {
          g_clear_error (&err);

          /* we tried to copy a directory and either 
           *
           * - the target did not exist which means we simple have to 
           *   create the target directory
           *
           * or
           *
           * - the target is not a directory and we tried to overwrite it in 
           *   which case we have to delete it first and then create the target
           *   directory
           */

          /* check if the target file exists */
          target_exists = g_file_query_exists (target_file,
                                               exo_job_get_cancellable (EXO_JOB (job)));

          /* abort on cancellation, continue otherwise */
          if (!exo_job_set_error_if_cancelled (EXO_JOB (job), &err))
            {
              if (target_exists)
                {
                  /* the target still exists and thus is not a directory. try to remove it */
                  g_file_delete (target_file, 
                                 exo_job_get_cancellable (EXO_JOB (job)), 
                                 &err);
                }

              /* abort on error or cancellation, continue otherwise */
              if (err == NULL)
                {
                  /* now try to create the directory */
                  g_file_make_directory (target_file, 
                                         exo_job_get_cancellable (EXO_JOB (job)), 
                                         &err);
                }
            }
        }
    }

  if (G_UNLIKELY (err != NULL))
    {
      g_propagate_error (error, err);
      return FALSE;
    }
  else
    {
      return TRUE;
    }
}