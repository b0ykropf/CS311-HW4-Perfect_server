//Perfect number client
#define _POSIX_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <iostream>
#include <pthread.h>
#include <time.h>

#define PORT = 26505



using namespace std;

double performance(int ops){
    time_t start;
    time_t end;
    int test;
    double time;
    start = clock();
    for(int i = 0; i < ops; i++){
        test = (i % 5);
        if(test == 0){
            test = 1;
        }
    }
    end = clock();
    time = static_cast<double>(end - start)/CLOCKS_PER_SEC;
    return time;
}

int perfect_test(int num){
    int factors[num/2];
    int factor_crawl = 0;
    int factor_sum = 0;
    for(int i = 1; i < num; i++){
        if(num % i == 0){
            factors[factor_crawl] = i;
            factor_crawl++;
        }
    }
    for(int j = 0; j < factor_crawl; j++){
        factor_sum += factors[j];
    }
    if(factor_sum == num){
        return 1;
    }
    else{
        return 0;
    }
}

struct thread_data{ //struct for storing infortmation to pass to threads
    int argument;
};

struct thread_data thread_data_array[1];// Temp data for threads


void *sig_listen(void *args){ //Function for threads to call

cout << "I am here!\n";

    pthread_exit(NULL);
}

int test_suite(int range_start, int range_end){
    int perfect_list[50];
    int perfects_found = 0;
    int current_test;
    for(int i = range_start; i <= range_end; i++){    
         current_test = perfect_test(i);
         if(current_test == 1){
             perfect_list[perfects_found] = i;
             perfects_found++;
         }
    }
    return perfects_found;
}

int main(int argc, char **argv){
    int ops = 1000000000;
    int perfects_found = 0;
    int range_start = 0;
    int range_end = 0;

    int sock_fd;
    struct sockaddr_inet servaddr;

/* Test Own Performance *******************************/
    cout << "Computing client started. Testing this system's performance. \n";
    double time_ops = performance(ops);
    cout <<"Performance is " << ops << " operations in " << time_ops << "seconds.\n";
/******************************************************/

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    connect(sock_fd, (struct sock_addr *) &servaddr, sizeof(servaddr));
    
/* THREAD STUFF ***************************************/

    int rc;
    pthread_t sig_mon[1];
    thread_data_array[0].argument = 0;

    rc = pthread_create(&sig_mon[0],NULL, sig_listen,(void *) &thread_data_array[0]);   
cout<<"RC is " << rc << endl;
/******************************************************/

    return 0;
}
