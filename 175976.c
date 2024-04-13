thunar_transfer_job_copy_file (ThunarTransferJob *job,
                               GFile             *source_file,
                               GFile             *target_file,
                               GError           **error)
{
  ThunarJobResponse response;
  GFileCopyFlags    copy_flags = G_FILE_COPY_NOFOLLOW_SYMLINKS;
  GError           *err = NULL;
  gint              n;

  _thunar_return_val_if_fail (THUNAR_IS_TRANSFER_JOB (job), NULL);
  _thunar_return_val_if_fail (G_IS_FILE (source_file), NULL);
  _thunar_return_val_if_fail (G_IS_FILE (target_file), NULL);
  _thunar_return_val_if_fail (error == NULL || *error == NULL, NULL);

  /* abort on cancellation */
  if (exo_job_set_error_if_cancelled (EXO_JOB (job), error))
    return NULL;

  /* various attempts to copy the file */
  while (err == NULL)
    {
      if (G_LIKELY (!g_file_equal (source_file, target_file)))
        {
          /* try to copy the file from source_file to the target_file */
          if (ttj_copy_file (job, source_file, target_file, copy_flags, TRUE, &err))
            {
              /* return the real target file */
              return g_object_ref (target_file);
            }
        }
      else
        {
          for (n = 1; err == NULL; ++n)
            {
              GFile *duplicate_file = thunar_io_jobs_util_next_duplicate_file (THUNAR_JOB (job),
                                                                               source_file, 
                                                                               TRUE, n, 
                                                                               &err);

              if (err == NULL)
                {
                  /* try to copy the file from source file to the duplicate file */
                  if (ttj_copy_file (job, source_file, duplicate_file, copy_flags, TRUE, &err))
                    {
                      /* return the real target file */
                      return duplicate_file;
                    }
                  
                  g_object_unref (duplicate_file);
                }

              if (err != NULL && err->domain == G_IO_ERROR && err->code == G_IO_ERROR_EXISTS)
                {
                  /* this duplicate already exists => clear the error to try the next alternative */
                  g_clear_error (&err);
                }
            }
        }

      /* check if we can recover from this error */
      if (err->domain == G_IO_ERROR && err->code == G_IO_ERROR_EXISTS)
        {
          /* reset the error */
          g_clear_error (&err);

          /* ask the user whether to replace the target file */
          response = thunar_job_ask_replace (THUNAR_JOB (job), source_file, 
                                             target_file, &err);

          if (err != NULL)
            break;

          /* check if we should retry */
          if (response == THUNAR_JOB_RESPONSE_RETRY)
            continue;

          /* add overwrite flag and retry if we should overwrite */
          if (response == THUNAR_JOB_RESPONSE_YES)
            {
              copy_flags |= G_FILE_COPY_OVERWRITE;
              continue;
            }

          /* tell the caller we skipped the file if the user 
           * doesn't want to retry/overwrite */
          if (response == THUNAR_JOB_RESPONSE_NO)
            return g_object_ref (source_file);
        }
    }

  _thunar_assert (err != NULL);

  g_propagate_error (error, err);
  return NULL;
}