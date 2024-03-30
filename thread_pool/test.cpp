#include <string>
#include <iostream>
#include "thread_pool.hpp"

std::mutex printf_;
void Task1();
void Task2(const std::string &name);
void Task3();
int main(){
    ThreadPool pool(4);
    pool.AddTask(Task1);
    pool.AddTask(Task2,"task2");
    pool.AddTask(Task3);
    return 0;
}

void Task1(){
    std::cout<<"performing task1\n";
}
void Task2(const std::string &name){
    std::cout<<"performing task2\n";
}
void Task3(){
    std::cout<<"performing task3\n";
}


