/*
 * MIT License
 *
 * Copyright (c) 2018 Pierre Bouillon
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>


#define  MSG_LAUNCHED        "Pomodoro timer launched"
#define  MSG_PAUSE_BEGIN     "Time to take a break !"
#define  MSG_PAUSE_END       "Pause ended, get back to work !"
#define  MSG_PAUSE_TITLE     "Pomodoro pause"

#define  POMODORO_LONG_HOLD  15 * 60  // long  pause of 15mn
#define  POMODORO_SMALL_HOLD 5  * 60  // small pause of 5mn
#define  POMODORO_TIMER      25 * 60  // iterations of 25mn
#define  POMODORO_TRIGGER    4        // each 4 small pause, 
                                      // trigger a long one

#define  TRUE                1


void daemonize() ;
void poll() ;
void show_info(char* msg, int len) ;


int pomodoro_cpt ;

/**
 * \fn    daemonize()
 * \brief daemonize the script
 */
void daemonize()
{
    pid_t pid ;

    if ((pid = fork()) > 0)
        exit (EXIT_SUCCESS) ;

    if (pid < 0)
        exit (EXIT_FAILURE) ;     

    if (setsid() < 0)
        exit (EXIT_FAILURE) ;

    if ((pid = fork()) < 0)
        exit (EXIT_FAILURE) ;

    syslog (LOG_NOTICE, "%s", "Pomorodo's daemon started") ;
} /* daemonize */


/**
 * \fn    poll
 * \brief run the Pomodoro timer
 */
void poll()
{
    int to_sleep ;

    show_info (MSG_LAUNCHED, sizeof (MSG_LAUNCHED)) ;
    syslog (LOG_NOTICE, "%s", "Pomorodo script now polling") ;
    
    while (TRUE)
    {
        sleep (POMODORO_TIMER) ;
        {
            ++pomodoro_cpt ;
            to_sleep = POMODORO_SMALL_HOLD ;
            
            if (pomodoro_cpt > POMODORO_TRIGGER)
            {
                pomodoro_cpt = 0 ;
                to_sleep = POMODORO_LONG_HOLD ;
            }

            show_info (MSG_PAUSE_BEGIN, sizeof (MSG_PAUSE_BEGIN)) ;
            sleep (to_sleep) ;
            show_info (MSG_PAUSE_END, sizeof (MSG_PAUSE_END)) ;
        }
    }
} /* poll */

/**
 * \fn    show_info
 * \brief display a linux alert
 *
 * \param msg : [char*] message to display
 */
void show_info(char *msg, int len)
{
    char buff [150] ;

    sprintf (
        buff,
        "notify-send '%s' '%s'",
        MSG_PAUSE_TITLE,
        msg
    ) ;

    system (buff) ;
} /* show_info */

/**
 * \fn main
 */
int main(int argc, char const *argv[])
{
    pomodoro_cpt = 0 ;

    daemonize() ;

    poll() ;

    return 0 ;
} /* main */
