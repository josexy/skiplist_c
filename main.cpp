#include<iostream>
#include<map>
#include"skiplist.h"
#include"ClockTest.h"
using namespace std;

int main(int argc, char const *argv[]){
    int n=10;
    srand(time(nullptr));
    // 测试skiplist insert
    CClock::start_timeclock();
    skiplist<int,int> sk(0.5,TAIL_INFINITY_KEY,0,n);
    for (int i = n; i >= 0; --i){
        sk.insert(Element<int,int>{rand()%n,i});
    }
    CClock::stop_timeclock();
    auto skiplist_insert_time=CClock::time_duration();
    
    // 测试map insert
    CClock::start_timeclock();
    map<int,int> mp;
    for (int i = n; i >=0; --i){
        mp.insert(pair<int,int>(rand()%n,i));
    }
    CClock::stop_timeclock();
    auto map_insert_time=CClock::time_duration();
    
    // 测试skiplist find
    CClock::start_timeclock();
    for (int i = n; i >=0; --i){
        sk.find(rand()%n);
    }
    CClock::stop_timeclock();
    auto skip_find_time=CClock::time_duration();

    // 测试map find
    CClock::start_timeclock();
    for (int i = n; i >=0; --i){
        mp.find(rand()%n);
    }
    CClock::stop_timeclock();
    auto map_find_time=CClock::time_duration();
    
    // 测试skiplist erase
    CClock::start_timeclock();
    for (int i = n; i >=0; --i){
        sk.erase(rand()%n);
    }
    CClock::stop_timeclock();
    auto skip_erase_time=CClock::time_duration();

    // 测试map erase
    CClock::start_timeclock();
    for (int i = n; i >=0; --i){
        mp.erase(rand()%n);
    }
    CClock::stop_timeclock();
    auto map_erase_time=CClock::time_duration();
    

    cout<<"SkipList insert time: "<<skiplist_insert_time<<endl;
    cout<<"SkipList find time: "<<skip_find_time<<endl;
    cout<<"SkipList erase time: "<<skip_erase_time<<endl;
    cout<<endl;
    cout<<"Map insert time: "<<map_insert_time<<endl;
    cout<<"Map find time: "<<map_find_time<<endl;
    cout<<"Map erase time: "<<map_erase_time<<endl;

    cout<<"SkipList size: "<<sk.size()<<endl;
    cout<<"Map size: "<<mp.size()<<endl;
    return 0;
}
