thunar_transfer_job_execute (ExoJob  *job,
                             GError **error)
{
  ThunarThumbnailCache *thumbnail_cache;
  ThunarTransferNode   *node;
  ThunarApplication    *application;
  ThunarJobResponse     response;
  ThunarTransferJob    *transfer_job = THUNAR_TRANSFER_JOB (job);
  GFileInfo            *info;
  gboolean              parent_exists;
  GError               *err = NULL;
  GList                *new_files_list = NULL;
  GList                *snext;
  GList                *sp;
  GList                *tnext;
  GList                *tp;
  GFile                *target_parent;
  gchar                *base_name;
  gchar                *parent_display_name;

  _thunar_return_val_if_fail (THUNAR_IS_TRANSFER_JOB (job), FALSE);
  _thunar_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (exo_job_set_error_if_cancelled (job, error))
    return FALSE;

  exo_job_info_message (job, _("Collecting files..."));

  /* take a reference on the thumbnail cache */
  application = thunar_application_get ();
  thumbnail_cache = thunar_application_get_thumbnail_cache (application);
  g_object_unref (application);

  for (sp = transfer_job->source_node_list, tp = transfer_job->target_file_list;
       sp != NULL && tp != NULL && err == NULL;
       sp = snext, tp = tnext)
    {
      /* determine the next list items */
      snext = sp->next;
      tnext = tp->next;

      /* determine the current source transfer node */
      node = sp->data;

      info = g_file_query_info (node->source_file,
                                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                exo_job_get_cancellable (job),
                                &err);

      if (G_UNLIKELY (info == NULL))
        break;

      /* check if we are moving a file out of the trash */
      if (transfer_job->type == THUNAR_TRANSFER_JOB_MOVE 
          && thunar_g_file_is_trashed (node->source_file))
        {
          /* update progress information */
          exo_job_info_message (job, _("Trying to restore \"%s\""),
                                g_file_info_get_display_name (info));

          /* determine the parent file */
          target_parent = g_file_get_parent (tp->data);

          /* check if the parent exists */
          if (target_parent != NULL)
            parent_exists = g_file_query_exists (target_parent, exo_job_get_cancellable (job));

          /* abort on cancellation */
          if (exo_job_set_error_if_cancelled (job, &err))
            {
              g_object_unref (target_parent);
              break;
            }

          if (target_parent != NULL && !parent_exists)
            {
              /* determine the display name of the parent */
              base_name = g_file_get_basename (target_parent);
              parent_display_name = g_filename_display_name (base_name);
              g_free (base_name);

              /* ask the user whether he wants to create the parent folder because its gone */
              response = thunar_job_ask_create (THUNAR_JOB (job),
                                                _("The folder \"%s\" does not exist anymore but is "
                                                  "required to restore the file \"%s\" from the "
                                                  "trash"),
                                                parent_display_name, 
                                                g_file_info_get_display_name (info));

              /* abort if cancelled */
              if (G_UNLIKELY (response == THUNAR_JOB_RESPONSE_CANCEL))
                {
                  g_object_unref (target_parent);
                  g_free (parent_display_name);
                  break;
                }

              /* try to create the parent directory */
              if (!g_file_make_directory_with_parents (target_parent, 
                                                       exo_job_get_cancellable (job),
                                                       &err))
                {
                  if (!exo_job_is_cancelled (job))
                    {
                      g_clear_error (&err);

                      /* overwrite the internal GIO error with something more user-friendly */
                      g_set_error (&err, G_IO_ERROR, G_IO_ERROR_FAILED,
                                   _("Failed to restore the folder \"%s\""), 
                                   parent_display_name);
                    }

                  g_object_unref (target_parent);
                  g_free (parent_display_name);
                  break;
                }

              /* clean up */
              g_free (parent_display_name);
            }

          if (target_parent != NULL)
            g_object_unref (target_parent);
        }
      
      if (transfer_job->type == THUNAR_TRANSFER_JOB_MOVE)
        {
          /* update progress information */
          exo_job_info_message (job, _("Trying to move \"%s\""), 
                                g_file_info_get_display_name (info));

          if (g_file_move (node->source_file, tp->data, 
                           G_FILE_COPY_NOFOLLOW_SYMLINKS 
                           | G_FILE_COPY_NO_FALLBACK_FOR_MOVE,
                           exo_job_get_cancellable (job),
                           NULL, NULL, &err))
            {
              /* notify the thumbnail cache of the move operation */
              thunar_thumbnail_cache_move_file (thumbnail_cache, 
                                                node->source_file, 
                                                tp->data);

              /* add the target file to the new files list */
              new_files_list = thunar_g_file_list_prepend (new_files_list, tp->data);

              /* release source and target files */
              thunar_transfer_node_free (node);
              g_object_unref (tp->data);

              /* drop the matching list items */
              transfer_job->source_node_list = g_list_delete_link (transfer_job->source_node_list, sp);
              transfer_job->target_file_list = g_list_delete_link (transfer_job->target_file_list, tp);
            }
          else if (!exo_job_is_cancelled (job))
            {
              g_clear_error (&err);

              /* update progress information */
              exo_job_info_message (job, _("Could not move \"%s\" directly. "
                                           "Collecting files for copying..."),
                                    g_file_info_get_display_name (info));

              if (!thunar_transfer_job_collect_node (transfer_job, node, &err))
                {
                  /* failed to collect, cannot continue */
                  g_object_unref (info);
                  break;
                }
            }
        }
      else if (transfer_job->type == THUNAR_TRANSFER_JOB_COPY)
        {
          if (!thunar_transfer_job_collect_node (THUNAR_TRANSFER_JOB (job), node, &err))
            break;
        }

      g_object_unref (info);
    }

  /* release the thumbnail cache */
  g_object_unref (thumbnail_cache);

  /* continue if there were no errors yet */
  if (G_LIKELY (err == NULL))
    {
      /* perform the copy recursively for all source transfer nodes */
      for (sp = transfer_job->source_node_list, tp = transfer_job->target_file_list;
           sp != NULL && tp != NULL && err == NULL;
           sp = sp->next, tp = tp->next)
        {
          thunar_transfer_job_copy_node (transfer_job, sp->data, tp->data, NULL, 
                                         &new_files_list, &err);
        }
    }

  /* check if we failed */
  if (G_UNLIKELY (err != NULL))
    {
      g_propagate_error (error, err);
      return FALSE;
    }
  else
    {
      thunar_job_new_files (THUNAR_JOB (job), new_files_list);
      thunar_g_file_list_free (new_files_list);
      return TRUE;
    }
}