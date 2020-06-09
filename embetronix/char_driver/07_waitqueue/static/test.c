#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main(void)
{
	int fd,retval;
	off_t offset;
	char buf[100]="Lokesh is writing char driver";

	/* Open driver */
	fd=open("/dev/chl_chardev",O_RDWR);
	if(fd == -1)
	{
		printf("CHL Device test : Device opening error\n");
		perror("CHL Erro: ");
		printf("ERRNO : %dn",errno);
		_exit(1);
	}

	/* Write to driver */
	retval=write(fd,buf,strlen(buf)+1);
	if(retval == -1)
	{
		printf("CHL Device test : Device writing error\n");
		close(fd);
		perror("CHL Erro: ");
		printf("ERRNO : %dn",errno);
		_exit(1);
	}

	/* Set offset to 5 character from starting */
	offset=lseek(fd,5,SEEK_SET);

	retval = read(fd,buf,sizeof(buf));
	if(retval == -1)
	{
		printf("CHL Device test : Device reading error\n");
		close(fd);
		perror("CHL Erro: ");
		printf("ERRNO : %dn",errno);
		_exit(1);
	}

	printf("Read buffer : '%s' \n",buf);
	close(fd);

	return 0;
}
