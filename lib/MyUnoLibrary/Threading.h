#include <Arduino.h>
#include <stdint.h>



class Kernel{
    struct Thread{
        void(*_func)();
        uint8_t _priority;
        uint16_t _period;
        uint16_t _dueTime;

        Thread(){};
        Thread(void(*func)(), uint8_t priority, uint16_t period){
            _func = func;
            _priority = priority;
            _period = period;
        }
    };

    Thread *thread;
    uint16_t threads = 0;

    public:
    Kernel(){

    }
    ~Kernel(){
        delete[] thread;
    }

    void spawnThread(void(*func)(), uint8_t prio, uint16_t periodTime){
        Thread *new_thread = new Thread[threads + 1];
        for(uint16_t i = 0; i < threads; i++){
            new_thread[i] = thread[i];
        }
        new_thread[threads + 1] = Thread(func, prio, periodTime);
        threads++;
        thread = new_thread;
        delete[] new_thread;
    }

    void deleteThread(void(*func)()){
        if(threads != 0){
            uint8_t a = 0;
            Thread *new_thread = new Thread[threads - 1];
            for(uint16_t i = 0; i < threads - 1; i++){
                if(thread[i]._func == func) a = 1;
                new_thread[i] = thread[i + a];
            }
            threads--;
            thread = new_thread;
            delete[] new_thread;
        }
    }

    void sleepThisThread(){

    }

    void run(){
        for(uint16_t i = 0; i < threads; i++){
            thread[i]._func();
        }
    }    
    friend struct Thread;
        
};

void funk1();
void funk2();

Kernel kernel;

void setup(){
    kernel.spawnThread(funk1, 2, 500);
    kernel.spawnThread(funk2, 3, 1000);

}
void loop(){
    kernel.run();
}

void funk1(){

}
void funk2(){

}






