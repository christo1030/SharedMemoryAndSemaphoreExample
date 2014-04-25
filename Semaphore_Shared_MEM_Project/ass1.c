//
//  Author: Christo Pettas
//
//  Assignment: program assignment 1
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>



/* change the key number */
#define SHMKEY ((key_t) 7815)


typedef struct
{
  int value;
}shared_mem;

shared_mem *total;




/*----------------------------------------------------------------------*
 * This function increases the value of shared variable "total"
 *  by one all the way to 100000
 *----------------------------------------------------------------------*/

void process1()
{
  int k = 0;

  while (k < 100000)
    {
      k++;
      total->value = total->value + 1;
    }
  printf ("From process1 total = %d\n", total->value);
}


/*----------------------------------------------------------------------*
 * This function increases the vlaue of shared memory variable "total"
 *  by one all the way to 200000
 *----------------------------------------------------------------------*/

void process2 ()
{
  int k = 0;

  while (k < 200000)
    {
      k++;

     total->value = total->value + 1;
    }

  printf ("From process2 total = %d\n", total->value);

}

void process3()
{
  int k = 0;

  while (k < 300000)
    {
      k++;

     total->value = total->value + 1;
    }

  printf ("From process3 total = %d\n", total->value);


}



/*----------------------------------------------------------------------*
 * MAIN()
 *----------------------------------------------------------------------*/

main()
{
  int   shmid;
  int   pid1;
  int   pid2;
  int   pid3;
  int   ID;
  int	status;
  char *shmadd;
  shmadd = (char *) 0;
  /* place semaphore in shared memory */
  sem_t *sem = mmap(NULL, sizeof(sem),
      PROT_READ |PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,
      -1, 0);

    //checks to see if error for placing semaphone in shared mem
    if (sem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
       }

  // initialize semaphore to 1 and checks for errors
    if ( sem_init(sem, 1, 1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
  }



/* Create and connect to a shared memory segmentt*/

  if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
      perror ("shmget");
      exit (1);
    }


 if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
    {
      perror ("shmat");
      exit (0);
    }


  total->value = 0;
    // child process created
  if ((pid1 = fork()) == 0){

        sem_wait(sem);
        process1();
        sem_post(sem);
    }

  //child 2
  if ((pid1 != 0) && (pid2 = fork()) == 0){
        sem_wait(sem);
        process2();
        sem_post(sem);
  }
  // child 3
  if ((pid1 != 0) && (pid2 != 0) && (pid3 = fork()) == 0)
     { sem_wait(sem);
       process3();
       sem_post(sem);
     }
            // parent proccess only
      if ((pid1 != 0) && (pid2 != 0) && (pid3 != 0)){
            wait(NULL);
            wait(NULL);
            wait(NULL);
            //waitpid(childPid, &returnStatus, 0);
            printf("Child 1 Pid: %d \n", pid1);
            printf("Child 2 Pid: %d \n", pid2);
            printf ("Child 3 Pid: %d \n", pid3);
            printf("Parent Pid: %d \n", getpid());

            // distroys semaphore and checks error
            if (sem_destroy(sem) < 0) {
                perror("sem_destroy failed");
                exit(EXIT_FAILURE);
                }
             // distroys *sem pointer in sharred memory
            if (munmap(sem, sizeof(sem)) < 0) {
                perror("munmap failed");
                exit(EXIT_FAILURE);
                }

            if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
            {
                perror ("shmctl");
                exit (-1);
            }

            printf ("\t\t  End of Program.\n");

      }
    }



/***** Note:  loop for parent to wait for child processes to finish and print ID of each child*****/

