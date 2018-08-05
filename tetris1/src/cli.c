#include <sys/timerfd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <tetris.h>
#include <def.h>
#include <keyboard.h>


int main( void )
{
        int time;
        printf("请选择等级 1 初级 2 中级 3 高级\n");
        scanf("%d",&level);
        switch(level){
          case 1: time=900000000;break;
          case 2: time=700000000;break;
          case 3: time=500000000;break;
          default:break;
        }
        int cfd = socket(AF_INET, SOCK_STREAM, 0);
        if (cfd == -1 ) perror("socket"),exit(1);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        inet_aton("127.0.0.1", &addr.sin_addr);
        int r = connect(cfd, (struct sockaddr*)&addr, sizeof addr);
        if ( r == -1 ) perror("connect"),exit(1);
        
        srand(getpid());
        init_keyboard();

        int tfd = -1;
        tfd = timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
        if ( tfd == -1 ) perror("timerfd_create"),exit(EXIT_FAILURE);

        struct itimerspec is;
        memset(&is, 0x00, sizeof is);
        is.it_value.tv_nsec = 1;
        is.it_interval.tv_nsec = time;
        timerfd_settime(tfd, 0, &is, NULL);
        int kfd = fileno(stdin);

        int maxfd = tfd > kfd ? tfd : kfd;
        maxfd = maxfd > cfd ? maxfd : cfd;
        fd_set rset;

        draw_back(background);
        next = rand() % 10;
        colour=rand()%6+1;
        show_next(next,&grade);

        while ( 1 ) {
                FD_ZERO(&rset);
                FD_SET(tfd, &rset);
                FD_SET(kfd, &rset);
                FD_SET(cfd, &rset);
                if (select(maxfd+1, &rset, NULL, NULL, NULL) <= 0 )
                        perror("select"),exit(1);
                if ( FD_ISSET(cfd, &rset) ) {
                        struct info inf;
                        memset(&inf, 0x00, sizeof(inf));
                        r = read(cfd, &inf, sizeof inf);
                        if ( r <= 0 ) break;
                        if ( ntohl(inf.type) == 1) {
                                draw_back1();
                                show_next1(inf.next,&inf.grade3);
                                draw_shape1(inf.t.x, inf.t.y, inf.s, inf.colour);
                        } else { // type = 2

                               memcpy(background2, inf.background, sizeof inf.background);
                               colour1=inf.colour;
                                show_next1(inf.next,&inf.grade3);
                                draw_back1();
                        }
                }
                if ( FD_ISSET(tfd, &rset)) {
                        uint64_t clk;
                        read(tfd, &clk, sizeof clk);
                        timer_tetris(&t,background,level,&grade);
                }

                if ( FD_ISSET(kfd, &rset) ) {
                      tetris(&t,background,&grade);             
                 }
        }
      recover_keyboard();
}


