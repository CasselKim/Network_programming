#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 50

void merge(int nums[], int l, int m, int r);
void *mergeSort(void *arg);

struct threadArgs{int* nums; int l; int r;};

int buffer[BUF_SIZE];
pthread_mutex_t mutex;

int main(){
    pthread_t thread_id[2];
    int nums[] = {6,1,7,8,3,8,21,67,32,70,2,60,6};
    int num_size = sizeof(nums)/sizeof(int);
    int i;
    int l=0;
    int r=num_size-1;
    int m = (l+r)/2;

    struct threadArgs *targs1,*targs2;
    targs1 = (struct threadArgs *)malloc(sizeof(struct threadArgs));
    targs2 = (struct threadArgs *)malloc(sizeof(struct threadArgs));

    targs1->nums=nums;
    targs1->l=l;
    targs1->r=m;
    targs2->nums=nums;
    targs2->l=m+1;
    targs2->r=r;
    
    for(i=0;i<num_size;i++){
        printf("%d ",nums[i]);
    }
    printf("\n");

    pthread_mutex_init(&mutex,NULL);
    
    pthread_create(&(thread_id[0]), NULL, mergeSort, (void *)targs1);
    pthread_create(&(thread_id[1]), NULL, mergeSort, (void *)targs2);
    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);

    merge(nums, l, m, r);
    
    for(i=0;i<num_size;i++){
        printf("%d ",nums[i]);
    }
    printf("\n");

    pthread_mutex_destroy(&mutex);
    free(targs1);
    free(targs2);
    return 0;
}

void *mergeSort(void *arg){
    struct threadArgs * targs = (struct threadArgs *)arg;
    struct threadArgs * temp1 = (struct threadArgs *)malloc(sizeof(struct threadArgs));
    struct threadArgs * temp2 = (struct threadArgs *)malloc(sizeof(struct threadArgs));
    int *nums = targs->nums;
    int l=targs->l;
    int r=targs->r;
    int m = (l+r)/2;
    if (l<r){
        temp1->nums=nums;
        temp1->l=l;
        temp1->r=m;
        temp2->nums=nums;
        temp2->l=m+1;
        temp2->r=r;

        mergeSort((void*)temp1);
        mergeSort((void*)temp2);
        merge(nums,l,m,r);
    }
    free(temp1);
    free(temp2);
    return NULL;
}

void merge(int nums[], int l, int m, int r){
    int i,j,k;
    i=l; j=m+1; k=0;
    pthread_mutex_lock(&mutex);
    while(i<=m && j<=r){
        if(nums[i]<=nums[j])
          buffer[k++] = nums[i++];
        else
          buffer[k++] = nums[j++];
    }
    
    while(i<=m){
        buffer[k++]=nums[i++];
    }
    while(j<=r){
        buffer[k++]=nums[j++];
    }
    
    for(i=l,k=0;i<=r;i++,k++){
        nums[i]=buffer[k];
    }
    pthread_mutex_unlock(&mutex);
}
