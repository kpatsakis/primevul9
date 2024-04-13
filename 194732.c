save_dialog (void)
{
  GtkWidget *dialog;
  GtkWidget *frame;
  gint       run;

  dialog = gimp_export_dialog_new (_("PSP"), PLUG_IN_BINARY, SAVE_PROC);

  /*  file save type  */
  frame = gimp_int_radio_group_new (TRUE, _("Data Compression"),
                                    G_CALLBACK (gimp_radio_button_update),
                                    &psvals.compression, psvals.compression,

                                    _("None"), PSP_COMP_NONE, NULL,
                                    _("RLE"),  PSP_COMP_RLE,  NULL,
                                    _("LZ77"), PSP_COMP_LZ77, NULL,

                                    NULL);

  gtk_container_set_border_width (GTK_CONTAINER (frame), 12);
  gtk_box_pack_start (GTK_BOX (gimp_export_dialog_get_content_area (dialog)),
                      frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run;
}