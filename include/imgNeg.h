#define SEM_SHM_KEY 0x1234746
#define SEM_ACT_KEY 0x4565645
#define SHM_KEY 0x65803e67

// After changing this, if you get 'shmget failed: Invalid argument'
// Refer: https://stackoverflow.com/a/16630287
#define MAX_BUF_SIZE 4096*4096*3
