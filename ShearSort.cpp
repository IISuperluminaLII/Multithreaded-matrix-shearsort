#include <pthread.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <vector>


//Matrix class that handelels the initilization of the shared memory space
class Matrix{
public:
    int * t_id;
    int **shearSort_mtx;

    explicit Matrix(int n){
        shearSort_mtx = new int*[n];
        for(int a = 0; a < n; a++){
            shearSort_mtx[a] = new int[n];
        }
    };
    void set_matrix(int **mtx){
        this->shearSort_mtx = mtx;
    }
    void destroy_matrix(int n){
        for(int i = 0; i < n; i++)
            delete[] shearSort_mtx[i];
        delete[] shearSort_mtx;
    }
};

//Class definition for the class methods
class Sort{
public:
    //static void * static_singlethreaded(void *);
    static void memSafe_bubble_sort_row(int **arr, int thread, bool reverse);
    static void memSafe_bubble_sort_col(int **arr, int thread);
    static void * shearSort_multithreaded(void * c);
};



pthread_mutex_t     lock1;
pthread_cond_t      cond_1;
int                 worker_thread_done = 0;
int                 n;

/*
 *
 * Method will print the 2D array given a address pointer to the adresss pointers of the elements
 * Uses tabs to space ouput
 *
 */
void print_array(int ** arr){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d\t",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//_____________________________SINGLE THREADED________________________________
//void bubble_sort_row_major(int * arr[n]){
//    int c,d;
//    int * swap_v;
//    for (c = 0 ; c < n - 1; c++)
//    {
//        for (d = 0 ; d < n - c - 1; d++)
//        {
//            if ((arr[d] && arr[d+1]) && *arr[d] <= *arr[d+1]) /* For decreasing order use < */
//            {
//                swap_v   = arr[d];
//                arr[d]   = arr[d+1];
//                arr[d+1] = swap_v;
//            }
//        }
//    }
//}
//
//void bubble_sort(int * arr[n]){
//    int c,d;
//    int * swap_v;
//    for (c = 0 ; c < n - 1; c++)
//    {
//        for (d = 0 ; d < n - c - 1; d++)
//        {
//            if (*arr[d] > *arr[d+1]) /* For decreasing order use < */
//            {
//                swap_v   = arr[d];
//                arr[d]   = arr[d+1];
//                arr[d+1] = swap_v;
//            }
//        }
//    }
//}
//
//void * row_phase_sort_right(Matrix * m){
//
//
//    Matrix *z = (Matrix *) m;
//    int *mtx_cpy[n][n];
//    int *leftrightsnake[n];
//    sig_atomic_t i = 0;
//
//    //Sort the alternating rows
//    //Wait for the blocking call to end to send the next iteration to another thread?
//    while(i < n){
//        for(int j =0; j < n;j++){
//            leftrightsnake[j] = new int(z->shearSort_mtx[i][j]);
//        }
//        //quickSort(leftrightsnake,0,NUM_COLS-1,0);
//        bubble_sort(leftrightsnake);
//        //bubble_sort(leftrightsnake);
//        for(int j =0; j < n;j++){
//            z->shearSort_mtx[i][j] = *leftrightsnake[j];
//        }
//        i = i + 2;
//    }
//
//    return 0;
//}
//
//
//void * row_phase_sort_left(Matrix * m){
//
//
//    Matrix *z = (Matrix *) m;
//    int *mtx_cpy[n][n];
//    int *leftrightsnake[n];
//    sig_atomic_t i = 1;
//
//    while(i < n){
//        for(int j =0; j < n;j++){
//            leftrightsnake[j] = new int(z->shearSort_mtx[i][j]);
//        }
//        bubble_sort_row_major(leftrightsnake);//,0,NUM_COLS-1,1);
//        for(int j =0; j < n;j++){
//            z->shearSort_mtx[i][j] = *leftrightsnake[j];
//        }
//        i = i + 2;
//    }
//
//    return NULL;
//}
//
//void * col_phase_sort(Matrix *m){
//
//
//    Matrix *z = (Matrix *) m;
//    int *mtx_cpy[n][n];
//    int *columnorder[n];
//    sig_atomic_t j = 0;
//
//    //Sort the alternating rows
//
//    //Sort the alternating rows
//    while(j < n){
//        //pthread_mutex_lock(&lock3);
//        for(int i =0; i < n;i++){
//            columnorder[i] = new int(z->shearSort_mtx[i][j]);
//        }
//        //quickSort(columnorder,0,NUM_ROWS-1,0);
//        bubble_sort(columnorder);
//        for(int i =0; i < n;i++){
//            z->shearSort_mtx[i][j] = *columnorder[i];
//        }
//        j = j + 1;
//    }
//    return NULL;
//}
//_____________________

/*
 * Method will sort cols by swapping values at the addresses
 * Input is the pointer to the pointers of numbers and the thread that is calling this method
 *
 */

void Sort::memSafe_bubble_sort_col(int **arr, int thread){
    int c,d;
    int swap_v;
    for (c = 0 ; c < n - 1; c++)
    {
        for (d = 0 ; d < n - c - 1; d++)
        {
            if (arr[d][thread] > arr[d+1][thread])
            {
                swap_v           = arr[d][thread];
                arr[d][thread]   = arr[d+1][thread];
                arr[d+1][thread] = swap_v;
            }
        }
    }
}

/*
 * Method will sort the rows by giving the pointer to the pointers of numebrs and
 * the thread that is calling it
 * Will also sort reverse if the calling thread is even or odd
 *
 */

void Sort::memSafe_bubble_sort_row(int **arr, int thread, bool reverse){
    int c,d;
    int swap_v;
    for (c = 0 ; c < n - 1; c++)
    {
        for (d = 0 ; d < n - c - 1; d++)
        {
            if (!reverse && arr[thread][d] <= arr[thread][d+1])
            {
                swap_v   = arr[thread][d];
                arr[thread][d]   = arr[thread][d+1];
                arr[thread][d+1] = swap_v;
            }else if(reverse && arr[thread][d] > arr[thread][d+1]){
                swap_v           = arr[thread][d];
                arr[thread][d]   = arr[thread][d+1];
                arr[thread][d+1] = swap_v;
            }
        }
    }
}

/*
 *
 * This method is the one that will be called by all the calling threads
 * Input: type void *c which is the shared memory input parameter
 *
 */

void * Sort::shearSort_multithreaded(void *c){

    //int thread = *(int *)c;
    Matrix *z = (Matrix *)c;
    int thread = *((Matrix *)c)->t_id;
    int i = 0;
    double num_phases = log2(n*n)+1;
    //printf("Thread %d started working\n",thread);
    if(thread < 0 || thread > n){
        printf("Error, Thread bounds > matrix size");
    }
    while(i < num_phases) {
        if ((i % 2) == 0) {
            //pthread_mutex_lock(&lock1);
            memSafe_bubble_sort_row(z->shearSort_mtx, thread, ((thread % 2) == 0));
            pthread_mutex_lock(&lock1);
            worker_thread_done++;
            if(worker_thread_done == n){
                worker_thread_done = 0;
                printf("MT::Phase:%d \n",i);
                print_array(z->shearSort_mtx);
                pthread_mutex_unlock(&lock1);
                //Broadcast
                pthread_cond_broadcast(&cond_1);
            }else{
                pthread_cond_wait(&cond_1,&lock1);
                pthread_mutex_unlock(&lock1);
            }
            i = i+1;
        }
        //Same idea as above but with col sorting
        else{
            //pthread_mutex_lock(&lock1);
            memSafe_bubble_sort_col(z->shearSort_mtx, thread);
            pthread_mutex_lock(&lock1);
            worker_thread_done++;
            if(worker_thread_done != n){
                pthread_cond_wait(&cond_1, &lock1);
                pthread_mutex_unlock(&lock1);
            }
            else{
                worker_thread_done = 0;
                //printf("OS sched released this this Thread %d last\n",thread);
                printf("MT::Phase:%d \n",i);
                print_array(z->shearSort_mtx);
                pthread_mutex_unlock(&lock1);
                pthread_cond_broadcast(&cond_1);
            }
            //BouncebbyBounce
            i = i+1;
        }
        //printf("Thread %d persued around here\n",thread);
    }
    //printf("Thread %d Exited\n",thread);
    pthread_exit((void *)1);
    //return (new int(1));
}

/*
 * This main method creates all the threads that run the multithreaded shearsort algorithm with shared memory
 *
 */
int main(int argc, char **argv) {

    pthread_mutex_init(&lock1, NULL);
    pthread_cond_init(&cond_1, NULL);

    int row = 0;
    std::vector<int> myVec;
    std::ifstream in("input.txt");
    std::string line,temp;
    bool first_line_done = false;
    //Iterate though
    while(std::getline(in, line))
    {
        if(row > n)
            throw std::invalid_argument( "Too many columns" );
        int column = 0;
        std::istringstream iss(line);
        while(std::getline(iss,temp,' '))
        {
            if(temp.empty() || temp == " ")
                continue;
            myVec.push_back(std::stoi(temp));
            column++;
            if(column > n && first_line_done)
                throw std::invalid_argument( "Too many rows" );
        }
        if(column < n)
            throw std::invalid_argument( "Dim row less than dim A" );
        if(!first_line_done){
            n = column;
            first_line_done = true;
        }
        row++;
    }
    if(row > n)
        throw std::invalid_argument( "Dim row less than dim A" );
    in.close();
    Matrix * m = new Matrix(n);
    int v = 0;
    int d = 0;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            m->shearSort_mtx[i][j] = myVec[(i*n)+j];
        }
    }
    pthread_t threads[n];
    printf("Initial Matrix\n");
    print_array(m->shearSort_mtx);

//    clock_t start = clock();

    int thread_create_status = 0;
    for(int j = 0; j < n;j++){
        Matrix * we_the_best = new Matrix(n);
        we_the_best->t_id = new int(j);
        we_the_best->shearSort_mtx = m->shearSort_mtx;
        thread_create_status = pthread_create(&threads[j], NULL, Sort::shearSort_multithreaded, (void *)we_the_best);
        if(thread_create_status < 0)
            printf("Thread exited with status: %d",thread_create_status);
    }
    for (pthread_t &thread : threads) {
        pthread_join(thread,NULL);
    }
//    printf("_______________\n");
//    printf("\nFinal Output\n");

//    printf("Total elements = %d\n",n*n);

//    clock_t stop = clock();
//    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
//    printf("\nTime elapsed: %.5f\n", (elapsed));


    m->destroy_matrix(n);

    return 0;
}
