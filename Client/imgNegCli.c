#include <stdio.h>
#include <imageUtils.h>
#include <imgNeg.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/shm.h>
#include <walltime.h>

void semWait(struct sembuf *sop, int sem_id) {
  sop->sem_num = 0;
  sop->sem_op = -1;
  sop->sem_flg = SEM_UNDO; //SEM_UNDO
  semop(sem_id, sop, 1);
}

void semSignal(struct sembuf *sop, int sem_id) {
  sop->sem_num = 0;
  sop->sem_op = 1;
  sop->sem_flg = SEM_UNDO; //SEM_UNDO
  semop(sem_id, sop, 1);
}

int main(int argc, char **argv) {
  assert(argc == 3);
  char *filename = argv[1];
  Image in = readImage(filename, 0);
  if (!in) {
    fprintf(stderr, "Unable to read given file\n");
    exit(-1);
  }
  // link semaphores
  struct sembuf sop_act, sop_shm;
  int sem_act_id, sem_shm_id;
  sem_act_id = semget(SEM_ACT_KEY, 0, 0);
  sem_shm_id = semget(SEM_SHM_KEY, 0, 0);
  if (sem_act_id < 0 || sem_shm_id < 0) {
    perror("Could not obtain semaphore");
    exit(3);
  }
  
  // Init shm
  int shmid;
  void *shared_memory = (void *)0;
  //srand( (unsigned int)getpid());
  shmid = shmget(SHM_KEY, sizeof(struct Image) + MAX_BUF_SIZE, 0666 | IPC_CREAT);
  if (shmid == -1) {
    fprintf(stderr, "shmget failed\n");
    exit(-1);
  }
  shared_memory = shmat(shmid,(void *)0, 0);
  if (shared_memory == (void *)-1) {
    fprintf(stderr, "shmat failed\n");
    exit(-1);
  }

  printf("Memory Attached at %p\n", shared_memory);
  Image shm_img = (Image) shared_memory;
  unsigned char *img_offset = shared_memory + sizeof(struct Image);
  
  // wait for activate
  printf("Waiting to connect to server....\n");
  semWait(&sop_act, sem_act_id);
#ifdef PROFILING
  double start, end;
  double clockZero = 0.0;
  start = walltime ( &clockZero );
#endif
  // write into shm
  printf("Connected. Writing to shared mem...\n");
  memcpy(shm_img, in, sizeof(struct Image));
  memcpy(img_offset, in->data, in->height * in->width * in->channel);
  // signal sem_shm
  semSignal(&sop_shm, sem_shm_id);
  // wait for sem_shm
  printf("Waiting for server to complete...\n");
  semWait(&sop_shm, sem_shm_id);
  // read from shm to in 
  shm_img->data = img_offset;
  writeImage(shm_img, argv[2]);
/*#ifndef PROFILING
  sleep(3); // To visualize how multiple processes are scheduled.
#endif*/
  printf("Output written to %s.\n", argv[2]);
  if (shmdt(shared_memory) == -1) { // Detach
    fprintf(stderr, "shmdt failed\n");
    exit(6);
  }
  printf("Done\n");
#ifdef PROFILING
  end = walltime ( &start );
  printf("\n[PROFILING] Time taken: %6.4f ms \n", end*1000.0);
#endif
  // signal activate
  semSignal(&sop_act, sem_act_id);
  
  freeImage(in);
}
