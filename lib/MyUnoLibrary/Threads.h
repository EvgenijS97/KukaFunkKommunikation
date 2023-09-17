#include <Arduino.h>
#include <stdint.h>

struct Timer{
    private:
    uint32_t timeStamp;
    bool start = true;

    public:
    bool run(uint16_t us){
        if(start){
            timeStamp = micros();
            start = false;
        }

        if(micros() - timeStamp >= us){
            start = true;
            return true;
        }
        return false;
    }
};

class Thread{
    protected:
    uint32_t timeStamp;
    bool startTimer;
    uint16_t elapsedTime;
    bool work = true;
    uint8_t priority;
    uint16_t period;
    uint16_t dueTime;

    bool runOnce = true;

    public:
    Thread(){}
    Thread(uint8_t prio, uint16_t periodTime){
        priority = prio;
        period = periodTime;
        
    }

    virtual void setup() = 0;
    virtual bool loop() = 0;

    void run(){
        if(runOnce){
            setup();
            runOnce = false;
        }
        while(work){
            uint32_t ms = millis();
            work = loop();
            elapsedTime = millis() - ms;
        }
    }
    friend class Kernel;
    
};


class Kernel{
    Thread *thread;
    Timer *timer;
    uint8_t threads;
    public:
    Kernel(Thread *threadArr, uint8_t n){
        thread = threadArr;
        timer = new Timer[n];
        threads = n;
        
    }
    ~Kernel(){
        delete[] thread;
        delete[] timer;
    }
    void run(){
        for(uint8_t i = 0; i < threads; i++){
            thread[i].run();
            thread[i].work = false;
        }
    }    
    friend class Thread;    
};








