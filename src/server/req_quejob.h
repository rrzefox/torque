#ifndef _REQ_QUEJOB_H
#define _REQ_QUEJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */


void sum_select_mem_request(job * pj);

int req_jobcredential(struct batch_request *preq);

int req_jobscript(batch_request *preq, bool perform_commit);

int req_mvjobfile(struct batch_request *preq);

int req_rdytocommit(struct batch_request *preq);

int req_commit(struct batch_request *preq);
int req_commit2(struct batch_request *preq);

/* static job *locate_new_job(int sock, char *jobid); */

#ifdef PNOT
int user_account_verify(char *arguser, char *argaccount);

char *user_account_default(char *arguser);

int user_account_read_user(char *arguser);
#endif /* PNOT */

#endif /* _REQ_QUEJOB_H */
