#include <linux/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
int fd;
int wrote;
static int OLED_WRITE_CHAR(int fd, unsigned char chr){
	int written;
	unsigned char buf[2];
	buf[0] = 0x05;
	buf[1] = chr;
	written = write(fd,buf,2);
	return written;
}
static void OLED_FILL(int fd, unsigned char chr){
	unsigned char buf[1];
	buf[0] = 0x03;
	buf[1] = chr;
	write(fd,buf,2);
}
static void OLED_SETCURSOR(int fd, uint8_t y, uint8_t x){
	unsigned char buf[2];
	buf[0] = 0x01;
	buf[1] = y;
	buf[2] = x;
	write(fd,buf,3);
}
static void OLED_WRITE_STRING(int fd, int length ,unsigned char *inBuff){
unsigned char outBuff[100];
if(length<=99){
memcpy(&outBuff[1],inBuff,length);
outBuff[0]= 0x02;
//printf("%s",(char*)outBuff);
write(fd,outBuff,length+1);	
}else{
	fprintf(stderr,"too long input");
}


}

int main(){
fd = open("/proc/dev/usr",0666);
wrote = 0;
// char message[100];
// scanf("%100[^\n]", message);
// int length = strlen(message);
// printf("%s %d\n",message,length);
// 
// OLED_SETCURSOR(fd, 0x00, 0x00);
// OLED_WRITE_STRING(fd,length,(unsigned char*)message);
// char fill[25] = {32};
// while(1){
// OLED_SETCURSOR(fd, 0x00, 0x00);
// //OLED_WRITE_STRING(fd,25,(unsigned char*)fill);
// OLED_SETCURSOR(fd, 0x00, 0x00);
// for(int i = 0 ; i < 25 ; i++){
	// OLED_WRITE_CHAR(fd,0x3F);
// }
// }
//OLED_FILL(fd,0x00);
unsigned char chr[1] = {0x80};
unsigned char fill[1] = {0x00};
//0x7F = the filled in character
OLED_FILL(fd, 0x00);
while(1){
OLED_SETCURSOR(fd,0x00,0x00);
//fill with hearts
for(uint8_t j = 0 ; j < 7 ; j++){
for(uint8_t i = 0 ; i < 25 ; i++){
	OLED_SETCURSOR(fd, j, i*5);
	OLED_WRITE_STRING(fd,1,chr);
}
}
//clear of hearts
OLED_SETCURSOR(fd,0x00,0x00);
for(uint8_t j = 0 ; j < 7 ; j++){
for(uint8_t i = 0 ; i < 25 ; i++){
	OLED_SETCURSOR(fd, j, i*5);
	OLED_WRITE_STRING(fd,1,fill);
}
}

}
//printf("fd:%d bytes written:%d\n",fd,wrote);
	return 0;
}
