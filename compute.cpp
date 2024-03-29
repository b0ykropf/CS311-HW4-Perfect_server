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

int perfects_list[50];
int list_crawl = 0;
const char* hostname;

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
void *sig_listen(void *args){ //Function for threads to call
    cout << "Listener on duty!" << endl;
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    struct hostent *server;
    server = gethostbyname(hostname);
    bcopy((char *)server->h_addr,(char *) &servaddr.sin_addr.s_addr,
    server->h_length);
    servaddr.sin_port = htons(PORT);
    memset(&(servaddr.sin_zero),'\0', 8);
    connect(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    char msg_snd[1024];
    sprintf(msg_snd, "RDY");
    write(sock_fd,msg_snd,1024);
    char msg_rcv[1024];
    read(sock_fd, msg_rcv, 1024);
    close(sock_fd);
    exit(0);
}

int test_suite(int range_start, int range_end){
    int perfects_found = 0;
    int current_test = 0;
    int seek_byte = 0;
    int seek_bit =0;
    for(int i = range_start; i <= range_end; i++){    
         current_test = perfect_test(i);
         if(current_test == 1){
             perfects_list[list_crawl] = i;
             perfects_found++;
             list_crawl++;
         }
    }
    return perfects_found;
}


int ask_range(int sock_fd, int ops, int time_ops, int *r_end){
    int r_start;
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
        istringstream(msg_parts[2]) >> *r_end;
        cout << "Start and End Recieved: " << r_start << " , " << *r_end << endl;
        return r_start;
    }
    else{
        cout << "Message Recieved did not contain ranges. \n";
    }
}

int return_perfects(int sock_fd, int num_p){
    char msg_snd[1024];
    int next_p;
    sprintf(msg_snd, "RTNP#%d#", num_p);
    write(sock_fd, msg_snd, 1024);
    for(int i = 0; i < num_p; i++){
        next_p = perfects_list[i];
        sprintf(msg_snd, "%d#",next_p);
        write(sock_fd, msg_snd, 1024);
    }
}


int main(int argc, char **argv){
    if(argc != 2){
        cout << "Need argument for Hostname" << endl;
    }

    hostname = argv[1];

    int ops = 1000000000;

    int perfects_total =0;
    int perfects_found = 0;
    int range_start = 0;
    int range_end = 0;

// SETUP SOCKET ***************************************
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    struct hostent *server;
    server = gethostbyname(hostname);
    bcopy((char *)server->h_addr,(char *) &servaddr.sin_addr.s_addr,
    server->h_length);
    servaddr.sin_port = htons(PORT);
    memset(&(servaddr.sin_zero),'\0', 8);
    connect(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
//******************************************************

/* Test Own Performance *******************************/
    cout << "Computing client started. Testing this system's performance. \n";
    double time_ops = performance(ops);
    int time_ops_int = time_ops * 100;
    cout <<"Performance is " << ops << " operations in " << time_ops << "seconds.\n";
/******************************************************/

//THREAD STUFF ***************************************

    int rc;
    int t_arg = 0;
    pthread_t sig_mon;

    rc = pthread_create(&sig_mon,NULL, sig_listen,(void *)t_arg);   
    cout<<"RC is " << rc << endl;
//******************************************************

    for(;;){
 
        range_start = ask_range(sock_fd, ops, time_ops_int, &range_end);
        perfects_found = test_suite(range_start, range_end);
        perfects_total += perfects_found;
        cout << "Perfects found: " << perfects_found << endl;
        return_perfects(sock_fd, perfects_found);   
    }


    return 0;
}
