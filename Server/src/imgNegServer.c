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
#include<sys/shm.h>

void semWait(struct sembuf *sop, int sem_id) {
  sop->sem_num = 0;
  sop->sem_op = -1;
  sop->sem_flg = 0; //SEM_UNDO
  semop(sem_id, sop, 1);
}

void semSignal(struct sembuf *sop, int sem_id) {
  sop->sem_num = 0;
  sop->sem_op = 1;
  sop->sem_flg = 0; //SEM_UNDO
  semop(sem_id, sop, 1);
}

void coreLoop() {
  // create SHM
  int shmid;
  void *shared_memory = (void *)0;
  //srand( (unsigned int)getpid());
  shmid = shmget(SHM_KEY, sizeof(struct Image) + MAX_BUF_SIZE, 0666 | IPC_CREAT);
  if (shmid == -1) {
    //fprintf(stderr, "shmget failed\n");
    perror("shmget failed");
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
  
  // Init sem. create sem_activate and init 1. sem_shm 0...
  struct sembuf sop_act, sop_shm;
  int sem_act_id, sem_shm_id;
  
  // Delete sems if exists
  sem_act_id = semget(SEM_ACT_KEY, 0, 0);
  sem_shm_id = semget(SEM_SHM_KEY, 0, 0);
  
  if (sem_act_id>=0 && semctl(sem_act_id, 0, IPC_RMID) < 0) {
    perror("Could not delete semaphore");
  }

  if (sem_shm_id>=0 && semctl(sem_shm_id, 0, IPC_RMID) < 0) {
    perror("Could not delete semaphore");
  }
  
  // Create sems
  sem_act_id = semget(SEM_ACT_KEY, 1, IPC_CREAT | IPC_EXCL | 0600);
  sem_shm_id = semget(SEM_SHM_KEY, 1, IPC_CREAT | IPC_EXCL | 0600);
  if (sem_act_id < 0 || sem_shm_id < 0) {
    perror("Could not create sem");
    exit(3);
  }

  if(semctl(sem_act_id, 0, SETVAL, 1) < 0) {
    perror("Could not set value of semaphore");
    exit(4);
  }
  if(semctl(sem_shm_id, 0, SETVAL, 0) < 0) {
    perror("Could not set value of semaphore");
    exit(4);
  }
  while(1) {    
    // wait sem_shm..
    printf("\nWaiting for a new client....\n");
    semWait(&sop_shm, sem_shm_id);
    printf("New Client connected..\n");
    // read into in from shm
    // write into shm
    Image in = shm_img;
    in->data = img_offset;
    negateImage(in);
    // signal sem_shm..
    printf("Done processing...\n");
    semSignal(&sop_shm, sem_shm_id);
  }
  if (shmdt(shared_memory) == -1) { // Detach
    fprintf(stderr, "shmdt failed\n");
    exit(6);
  }
  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    fprintf(stderr, "failed to delete\n");
    exit(7);
  }
}

int main() {
  coreLoop();
  return 0;
}
