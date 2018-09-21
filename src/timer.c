#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

volatile int count = 0;

void wakeup(union sigval data)
{
    ++count;
}

#define US  1000
#define MS  (1000 * US)
#define SEC (1000 * MS)

int main(int argc, char *argv[])
{
    struct sigevent se;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_signo = SIGALRM;
    se.sigev_value.sival_int = 42;
    se.sigev_notify_function = wakeup;
    se.sigev_notify_attributes = NULL;

    struct itimerspec ts;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 1 * MS;
    ts.it_interval.tv_sec = 0;    
    ts.it_interval.tv_nsec = SEC / 8000;

    timer_t timer;
    int status = timer_create(CLOCK_REALTIME, &se, &timer);
    if (status == -1) {
	puts("Create timer\n");
    }
    status = timer_settime(timer, 0, &ts, 0);
    if (status == -1){
	puts("Set timer\n");
    }

    while (count < 8000);
    puts("Booh!");
}
