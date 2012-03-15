//Perfect number client
#define _POSIX_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <vector>

#define PORT 26505



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


int ask_range(int sock_fd, int ops, int time_ops){
    int r_start;
    int r_end;
    char msg_snd[1024];
    sprintf(msg_snd, "REQWSPEC#%d#%d#", ops, time_ops);
    write(sock_fd, msg_snd, 1024);
    char msg_rcv[1024];
    vector <string> msg_parts(3);
    read(sock_fd, msg_rcv, 1024);
    string message= string(msg_rcv);
    string part;
    stringstream stream(message);
    for(int i = 0; i < 3 ; i++){
        getline(stream,part,'#');
        msg_parts[i] = part;
    }
    if(msg_parts[0] == "FPN"){
        istringstream(msg_parts[1]) >> r_start;
        istringstream(msg_parts[2]) >> r_end;
        cout << "Start and End Recieved: " << r_start << " , " << r_end << endl;
    }
    else{
        cout << "Message Recieved did not contain ranges. \n";
    }
}


int main(int argc, char **argv){
    int ops = 1000000000;
    int perfects_found = 0;
    int range_start = 0;
    int range_end = 0;

    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

/* Test Own Performance *******************************/
    cout << "Computing client started. Testing this system's performance. \n";
    double time_ops = performance(ops);
    int time_ops_int = time_ops * 100;
    cout <<"Performance is " << ops << " operations in " << time_ops << "seconds.\n";
/******************************************************/
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    struct hostent *server;
    server = gethostbyname("127.0.0.1");
    //server = gethostbyname("os-class.engr.oregonstate.edu");
    bcopy((char *)server->h_addr,(char *) &servaddr.sin_addr.s_addr,
    server->h_length);
    servaddr.sin_port = htons(PORT);
    memset(&(servaddr.sin_zero),'\0', 8);
    connect(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    for(int i = 0; i < 10 ; i ++){
 
        ask_range(sock_fd, ops, time_ops_int);   
    }
/* THREAD STUFF ***************************************

    int rc;
    pthread_t sig_mon[1];
    thread_data_array[0].argument = 0;

    rc = pthread_create(&sig_mon[0],NULL, sig_listen,(void *) &thread_data_array[0]);   
cout<<"RC is " << rc << endl;
******************************************************/

    return 0;
}
