#include<iostream>
#include<vector>
#include"RPcustomKW18parser.hxx"
#include<ctime>

extern "C" int kw18_parse(char *);


int main(int arch, char *argv[]){
    std::clock_t start;
    start = std::clock();

    kw18_parse(argv[1]);

    std::cout<<"Time : "<<(std::clock()-start)/(double) (CLOCKS_PER_SEC/1000)<< " ms"<<std::endl;

}

