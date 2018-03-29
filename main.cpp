#include <iostream>
#include <thread>
#include <deque>
#include <chrono> // std::chrono::seconds
#include <mutex>
#include <vector>

using namespace std;

std::mutex mu;
void display(deque<int> &waiting_queue,int &status,int &peopleBarbering,int selfNum,bool leaving){
    if(selfNum!=-1)
        cout<<"id created:"<<selfNum<<endl;
    int nowStatus=0;
    int nowPeopleBarbering;
    vector<int> waitingList;
    while(status==0&&!waiting_queue.empty()){
        std::this_thread::sleep_for (chrono::microseconds(10));
    }
    if(status==1){
        std::unique_lock<mutex> locker(mu);
        nowPeopleBarbering=peopleBarbering;
        nowStatus=1;
        while (!waiting_queue.empty()){
            int people=waiting_queue.front();
            waiting_queue.pop_front();
            waitingList.push_back(people);
        }
        for(auto w:waitingList)
            waiting_queue.push_back(w);
        locker.unlock();
        cout<<"Barber cutting the hair of customer "<<nowPeopleBarbering<<endl;
        cout<<"Waiting room:";
        for(auto w:waitingList)
            cout<<w<<" ";
        cout<<endl;
        
    } else{
        cout<<"Barber sleeping"<<endl;
        cout<<"Waiting room:"<<endl;
    }
    if(leaving)
        cout<<"Customer "<<selfNum<<" leaving"<<endl;
    cout<<endl;
    
    
}
//barber situation
void barber(deque<int> &waiting_queue,int &status,int &peopleBarbering){
    display(waiting_queue,status,peopleBarbering,-1,0);
    while(1){
        std::this_thread::sleep_for (chrono::microseconds(10));
        std::unique_lock<mutex> locker1(mu);
        if(waiting_queue.empty()){
            status=0;
            locker1.unlock();
            continue;
        }
        locker1.unlock();
        std::unique_lock<mutex> locker2(mu);
        status=1;
        peopleBarbering=waiting_queue.front();
        waiting_queue.pop_front();
        locker2.unlock();
        int barberTime=1+rand()%5;
        std::this_thread::sleep_for (chrono::seconds(barberTime));
    }
}
//leave guest
void creat_customer(deque<int> &dq,int id,int numSeats,int &status,int &peopleBarbering){
    bool leave=false;
    std::unique_lock<mutex> locker(mu);
    if(dq.size()<numSeats){
        dq.push_back(id);
    } else
        leave=true;
    locker.unlock();
    display(dq,status,peopleBarbering,id,leave);
}

void creatThread(int numSeats){
    int id=0;
    deque<int> dq;
    int status=0;
    int peopleBarbering=0;
    thread thBarber(barber,ref(dq),ref(status),ref(peopleBarbering));
    for(int i=1;;++i){
        id=i;
        thread thCustomer(creat_customer,ref(dq),id,numSeats,ref(status),ref(peopleBarbering));
        std::this_thread::sleep_for (chrono::seconds(3));//waiting for 3 seconds
        thCustomer.join();
    }
    thBarber.join();
    
}
int main() {
    int m;
    cout<<"Enter the number of seats \n";
    cin>>m;
    thread thCreatTh(creatThread,m);
    thCreatTh.join();
    return 0;
}

