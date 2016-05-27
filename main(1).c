#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockAir = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condTravelers=PTHREAD_COND_INITIALIZER,condAirPlane=PTHREAD_COND_INITIALIZER,condTravelersDis = PTHREAD_COND_INITIALIZER;
int nrOfTrav=0;
int N,MAX;

void *travelers(void *args)
{
    int id=-1;
    id = *((int *)args);
    pthread_mutex_lock(&lock);
    if(nrOfTrav >= MAX)
    {
        pthread_cond_wait(&condTravelers,&lock);
    }
    nrOfTrav++;
    pthread_mutex_unlock(&lock);

    printf("Boarding person %d\n",id);
    if(nrOfTrav == MAX)
    {
        pthread_cond_signal(&condAirPlane);
    }



}

void *airplane(void *args)
{

    if(pthread_mutex_lock(&lockAir)!=0)
    {
        printf("Cannot take lock");
    }
    printf("Taking off\n");
    sleep(3);
    printf("landed\n");
    pthread_cond_signal(&condTravelers);
    pthread_mutex_unlock(&lockAir);
}

int main(int argc,char* argv[])
{

    if(argc!=3)
    {
        perror("Invalid number of parameters");
        exit(1);
    }

    N = atoi(argv[1]);
    MAX = atoi(argv[2]);

    pthread_t travs[N];

    for(int i=1; i<=N; i++)
    {
        pthread_create(&travs[i],NULL,travelers,&i);
        sleep(1);
    }

    for(int i=1; i<=N; i++)
    {
        if(pthread_join(travs[i],NULL)!=0)
        {
            perror("Error joining threads");
            exit(2);
        }
    }
    pthread_t plane;

    pthread_create(&plane,NULL,airplane,&MAX);

    pthread_join(plane,NULL);
}
