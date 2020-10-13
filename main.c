#include <stdio.h>
#include <string.h>

#define length 50 //default length for string
#define packetlen 255

int main()
{
    char str[length];
    printf("Enter your string: ");
    scanf("%[^\n]", str);
    int packet[packetlen];
    packet[0] = 170; //AA start byte

    int size = 3;  //Default has 3
    int i;
    for(i = 0; i < str[i]; i++)
    {
        size++;
    }

    packet[1] = size;  //saves size + 3

    //assigns letters to int array by ascii value
    for(i = 0; i < str[i] ; i++)
    {
        packet[i+2] = str[i];  //assigning message part
    }


    //Calculating checksum
    int sum = 0;
    for(i = 0; i < size -1; i++)
    {
        sum ^= packet[i];  //XOR calculator
    }
    packet[size-1] = sum;

    //prints out the packet in hex
    for(i = 0; i < size; i++)  //Size -1 because of the checkSum
    {
        printf("0x%02X ", packet[i]);
    }

    return 0;
}
