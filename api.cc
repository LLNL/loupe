#include <sys/types.h>
#include <dlfcn.h>
#include <mpi.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stddef.h>

#include <sys/time.h>
#include "mpid.hh"

#include <map>
#include <vector>
#include <string>


mpid_data_stats* g_mpi_stats;

void dump(MPI_Comm comm, long app, long mpi);

#define f_mpi_init 0x0
#define f_mpi_finalize 0x1
#define f_mpi_comm_size 0x2
#define f_mpi_comm_rank 0x3
#define f_mpi_send 0x4
#define f_mpi_isend 0x5
#define f_mpi_recv 0x6
#define f_mpi_irecv 0x7
#define f_mpi_wait 0x8
#define f_mpi_waitall 0x9
#define f_mpi_waitany 0xa
#define f_mpi_allreduce 0xb
#define f_mpi_bcast 0xc
#define f_mpi_comm_split 0xd
#define f_mpi_alltoall 0xe
#define f_mpi_send_init 0xf
#define f_mpi_recv_init 0x10


extern "C" int MPI_Init( int *argc, char ***argv ){

    int ret;
    ret =PMPI_Init(argc,argv);
    g_mpi_stats = new mpid_data_stats("global");
    g_mpi_stats->mpid_init();
    return ret;
} 

extern "C" int MPI_Init_thread( int *argc, char ***argv, int required, int *provided ){

    int ret;
    g_mpi_stats = new mpid_data_stats("global");
    g_mpi_stats->mpid_init();
    ret =PMPI_Init_thread(argc,argv, required, provided);
    return ret;
}


extern "C" int MPI_Finalize( void ){
    int ret;
    g_mpi_stats->mpid_finalize();
    delete g_mpi_stats;
    ret = PMPI_Finalize();
    return ret;
}

extern "C" int MPI_Comm_size( MPI_Comm comm, int *size ){
    int ret;

    ret = PMPI_Comm_size(comm, size);
    return ret;
}


extern "C" int MPI_Comm_rank(MPI_Comm comm, int *rank ){

    int ret;
    
    ret = PMPI_Comm_rank(comm, rank);
    return ret;

}

extern "C" int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm){

    int ret;

    ret = PMPI_Comm_split(comm, color, key, newcomm);
    g_mpi_stats->mpid_add_communicator(newcomm); 

    return ret;

}

extern "C" int MPI_Send(const void *buf, int count, int datatype, int dest, int tag, MPI_Comm comm){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;
    
    gettimeofday(&timer_start, NULL);
    ret = PMPI_Send(buf, count, datatype, dest, tag, comm);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_send);
    g_mpi_stats->mpid_traffic_pattern(dest, count, datatype, comm, f_mpi_send);
    return ret;
}

extern "C" int MPI_Isend(const void *buf, int count, int datatype, int dest, int tag, MPI_Comm comm, int* request){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);


    //printf("Called MPI ISEND %ld\n",elapsed);
    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_isend);
    g_mpi_stats->mpid_traffic_pattern(dest, count, datatype, comm, f_mpi_isend);
    return ret;
}

extern "C" int MPI_Irecv(void *buf, int count, int datatype, int source, int tag, MPI_Comm comm, int *request){

    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_irecv);

    return ret;
}

extern "C" int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status){

    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);


    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_recv);
    return ret;
}


extern "C" int MPI_Waitany(int count, MPI_Request *array_of_requests, int *indx, MPI_Status *status){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Waitany(count, array_of_requests, indx, status);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(0, MPI_UNSIGNED, elapsed, f_mpi_waitany);
    return ret;
}

extern "C" int MPI_Wait(MPI_Request *request, MPI_Status *status){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Wait(request, status);
    gettimeofday(&timer_end, NULL);

    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);
    g_mpi_stats->mpid_call_stats(0, MPI_UNSIGNED, elapsed, f_mpi_wait);
    return ret;
}

extern "C" int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(0, MPI_UNSIGNED, elapsed, f_mpi_waitall);
    return ret;
}

extern "C" int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm ){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Bcast(buffer, count, datatype, root, comm);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_bcast);
    g_mpi_stats->mpid_traffic_pattern(-1, count, datatype, comm, f_mpi_bcast);
    return ret;
}

extern "C" int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm){ 

    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(sendcount, sendtype, elapsed, f_mpi_alltoall);
    g_mpi_stats->mpid_traffic_pattern(-1, sendcount, sendtype, comm, f_mpi_alltoall);
    return ret;

}

extern "C" int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, int datatype, int op, int comm){
    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_allreduce);
    g_mpi_stats->mpid_traffic_pattern(-1, count, datatype, comm, f_mpi_allreduce);
    return ret;
}

extern "C" int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request){

    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);


    //printf("Called MPI ISEND %ld\n",elapsed);
    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_send_init);
    g_mpi_stats->mpid_traffic_pattern(dest, count, datatype, comm, f_mpi_send_init);
    return ret;
}

extern "C" int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request){

    int ret;
    struct timeval timer_start, timer_end;
    unsigned long elapsed;

    gettimeofday(&timer_start, NULL);
    ret = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    gettimeofday(&timer_end, NULL);
    elapsed = (timer_end.tv_sec - timer_start.tv_sec)*1e6 + (timer_end.tv_usec - timer_start.tv_usec);

    g_mpi_stats->mpid_call_stats(count, datatype, elapsed, f_mpi_recv_init);

    return ret;
}
