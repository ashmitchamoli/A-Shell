#ifndef __JOBS_H
#define __JOBS_H

void A_Shell_jobs(int flags);
void A_Shell_sig(int job_index, int signal_);
void A_Shell_fg(int job_index);
void A_Shell_bg(int job_index);

#endif