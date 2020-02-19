#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <Vector.hpp>

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

//Threads struct
struct Threads{
  pthread_t thread;
  size_t ID;
  size_t failures;
  Vector* shared_vector;
  size_t* shared_elements;
  size_t* shared_looptime;
};

void* writer(void* arg){

  Threads* thisThread = static_cast<Threads*>(arg);

  //setAndTest of shared vector over 100 iterations
  for(size_t i = 0;i < 100; i++){
      if(thisThread->shared_vector->setAndTest(*(thisThread->shared_elements)+thisThread->ID)){
        printf("setAndTest in thread %lu Returned True.\n", thisThread->ID);
      }else{
        printf("setAndTest in thread %lu Returned False.\n", thisThread->ID);
        thisThread->failures++;
      }
      //Sleep for x us.
      usleep(*(thisThread->shared_looptime));
  }
  return nullptr;
}

int main(int argc, char* argv[]){
  //Check if two arguments passed
  if(argc != 4){
    std::cout << "Pass a number of threads, elements and microseconds" << std::endl;
    return 0;
  }
  //Checking if arguments are numbers
  else if(!isNumber(argv[1]) || !isNumber(argv[2]) || !isNumber(argv[3])){
    std::cout << "Only numbers" << std::endl;
    return 0;
  }
  //Converting passed arguments to integer
  size_t numthreads = std::stoi(argv[1]);
  size_t numelements = std::stoi(argv[2]);
  size_t looptime_us = std::stoi(argv[3]);
  //Checking if number of threads is between 1-100
  if(numthreads > 100 || numthreads < 1){
    std::cout << "Number of threads has to be between 1 and 100." << std::endl;
    return 0;
  }else{
      std::cout << "Creating " << numthreads << " Threads" << std::endl;
  }

  //Vector and total failures to be shared
  Vector shared_vector;

  //Creating threads
  int err;
  Threads myThreads[numthreads];

  for(size_t i = 0; i < numthreads; i++){
    myThreads[i].ID = i+1;
    myThreads[i].shared_vector = &shared_vector;
    myThreads[i].shared_elements = &numelements;
    myThreads[i].shared_looptime = &looptime_us;
    myThreads[i].failures = 0;
    err = pthread_create(&myThreads[i].thread, NULL, writer, &myThreads[i]);
    if(err!=0){
      std::cout << "Error in creating thread" << std::endl;
    }
  }
  //Joining threads
  for(size_t i = 0; i < numthreads; i++){
    pthread_join(myThreads[i].thread, NULL);
  }
  //Calculation total failures
  size_t totalFailures = 0;
  for (size_t i = 0; i < numthreads; i++) {
    totalFailures += myThreads[i].failures;
  }
  std::cout << "\nsetAndTest over 100 iterations:" << std::endl;
  std::cout << "Number of threads: " << numthreads << std::endl;
  std::cout << "Number of elements (set value) : " << numelements << std::endl;
  std::cout << "Sleep time: " << looptime_us << " us" << std::endl;
  std::cout << "Total number of Failures: " << totalFailures << std::endl;
}
