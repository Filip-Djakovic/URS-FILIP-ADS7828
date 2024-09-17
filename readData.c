#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define GPIO1_REGS_BASE         (0xff709000)
#define GPIO1_REGS_SIZE         (0x00001000)
#define GPIO1_REGS_DATA_OFFSET  (0x0)
#define GPIO1_REGS_DIR_OFFSET   (0x4)

#define HPS_LED_PIN	        (24U)

int turnOnOffLed(float voltageX)
{
    volatile unsigned int* hps_led_dir_addr = NULL;
    volatile unsigned int* hps_led_port_addr = NULL;
    void* virtual_base;
    int fd;
    fd = open("/dev/mem", (O_RDWR | O_SYNC));

    if (-1 == fd)
    {
        printf("Error when opening /dev/mem! Exiting...\n");
        return 1;
    }
    virtual_base = mmap(NULL, GPIO1_REGS_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, GPIO1_REGS_BASE);

    if (MAP_FAILED == virtual_base)
    {
        printf("Error when trying to mmap virtual to physical GPIO address! Exiting...\n");
        close(fd);
        return 1;
    }
    hps_led_dir_addr = (unsigned int *)(virtual_base + GPIO1_REGS_DIR_OFFSET);
    hps_led_port_addr = (unsigned int *)(virtual_base + GPIO1_REGS_DATA_OFFSET);
    *hps_led_dir_addr |= (1 << HPS_LED_PIN);
    *hps_led_port_addr &= ~(1 << HPS_LED_PIN);
    /*while(1)
    {
        *hps_led_port_addr &= ~(1 << HPS_LED_PIN); 
        usleep(500000);
        *hps_led_port_addr |= (1 << HPS_LED_PIN); 
        usleep(500000);
    }*/
    if(voltageX > 0.7)
    	*hps_led_port_addr &= ~(1 << HPS_LED_PIN);
    else
    	*hps_led_port_addr |= (1 << HPS_LED_PIN);
    
    if(0 != munmap(virtual_base, GPIO1_REGS_SIZE))
    {
        printf("Error when trying to munmap previously mapped addresses! Exiting...\n");
        close(fd);
        return 1;
    }
    close(fd);
}

int ads(FILE* fp, char path[], int channel)
{
    int adc_value;
    float voltage;
    const float VREF = 3.3; // Reference voltage in Volts

    // Open the file for reading
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read the integer value from the file
    if (fscanf(fp, "%d", &adc_value) != 1) {
        perror("Error reading value");
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Close the file
    fclose(fp);

    // Convert ADC value to voltage
    voltage = (adc_value / 4095.0) * VREF;
    
    if(channel == 0)
       turnOnOffLed(voltage);
    else
    	printf("Voltage on axis y: %.3f", voltage);
}


int main() {
    FILE *fp;
    char path[] = "/sys/class/hwmon/hwmon0/in0_input";
    /*FILE *fp1;
    char path1[] = "/sys/class/hwmon/hwmon0/in1_input";
    ads(fp0,path0,0);
    ads(fp1,path1,1);*/
    int adc_value;
    float voltage;
    const float VREF = 3.3; // Reference voltage in Volts

    // Open the file for reading
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read the integer value from the file
    if (fscanf(fp, "%d", &adc_value) != 1) {
        perror("Error reading value");
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Close the file
    fclose(fp);

    // Convert ADC value to voltage
    voltage = (adc_value / 4095.0) * VREF;
    turnOnOffLed(voltage);
    
    
    FILE *fp1;
    char path1[] = "/sys/class/hwmon/hwmon0/in1_input";
    
    fp1 = fopen(path1, "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read the integer value from the file
    if (fscanf(fp1, "%d", &adc_value) != 1) {
        perror("Error reading value");
        fclose(fp1);
        return EXIT_FAILURE;
    }

    // Close the file
    fclose(fp1);

    // Convert ADC value to voltage
    voltage = (adc_value / 4095.0) * VREF;
    printf("Voltage on y axis: %.3f", voltage);
    return EXIT_SUCCESS;
}
