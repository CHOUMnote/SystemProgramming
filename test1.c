#include "pre.h"

int main(int argc, char **argv) {
    key_t key;
    int shmid;
    struct test* shmaddr;

    key = ftok("key.key", 1);
    shmid = shmget(key, sizeof(struct test), 0);

    shmaddr = (struct test*)shmat(shmid, NULL, 0);
    shmaddr->a = 10, shmaddr->b = 10;
    kill(atoi(argv[1]), SIGUSR1);
    sleep(2);

    printf("Listener said : %d %d\n", shmaddr->a, shmaddr->b);
    system("ipcs -mo");
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);

}