thunar_transfer_job_class_init (ThunarTransferJobClass *klass)
{
  GObjectClass *gobject_class;
  ExoJobClass  *exojob_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_transfer_job_finalize; 

  exojob_class = EXO_JOB_CLASS (klass);
  exojob_class->execute = thunar_transfer_job_execute;
}