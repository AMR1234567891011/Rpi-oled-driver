///Linux Driver for yellow and blue .96 OLED I2C module can also be used for I2C microphone
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

/*************************************************************************************************/
#define I2C_BUS (1)
#define SLAVE_NAME ("MY I2C")
#define ADDRESS (0x3c)
#define MAX_CURSOR (127)
#define MAX_LINE (7)
/*************************************************************************************************/
static struct proc_dir_entry *dir;
static struct proc_dir_entry *myEntry;
static struct i2c_adapter *my_i2c_adapter = NULL;
static struct i2c_client *my_i2c_client = NULL;
static uint8_t CURSOR = 0;
static uint8_t HEIGHT = 0;
unsigned char myBuf[100];
/*************************************************************************************************/
//Fonts directly from here: https://embetronicx.com/tutorials/linux/device-drivers/i2c-linux-device-driver-using-raspberry-pi/#SSD1306_OLED
static const unsigned char SSD1306_font[][5]= 
{	
    {0x00, 0x00, 0x00, 0x00, 0x00},   // space 
    {0x00, 0x00, 0x2f, 0x00, 0x00},   // ! 
    {0x00, 0x07, 0x00, 0x07, 0x00},   // " 
    {0x14, 0x7f, 0x14, 0x7f, 0x14},   // # 
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},   // $ 
    {0x23, 0x13, 0x08, 0x64, 0x62},   // %
    {0x36, 0x49, 0x55, 0x22, 0x50},   // & 
    {0x00, 0x05, 0x03, 0x00, 0x00},   // ' 
    {0x00, 0x1c, 0x22, 0x41, 0x00},   // (
    {0x00, 0x41, 0x22, 0x1c, 0x00},   // )
    {0x14, 0x08, 0x3E, 0x08, 0x14},   // *
    {0x08, 0x08, 0x3E, 0x08, 0x08},   // +
    {0x00, 0x00, 0xA0, 0x60, 0x00},   // ,
    {0x08, 0x08, 0x08, 0x08, 0x08},   // -
    {0x00, 0x60, 0x60, 0x00, 0x00},   // .
    {0x20, 0x10, 0x08, 0x04, 0x02},   // /

    {0x3E, 0x51, 0x49, 0x45, 0x3E},   // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},   // 1
    {0x42, 0x61, 0x51, 0x49, 0x46},   // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31},   // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},   // 4
    {0x27, 0x45, 0x45, 0x45, 0x39},   // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30},   // 6
    {0x01, 0x71, 0x09, 0x05, 0x03},   // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},   // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},   // 9

    {0x00, 0x36, 0x36, 0x00, 0x00},   // :
    {0x00, 0x56, 0x36, 0x00, 0x00},   // ;
    {0x08, 0x14, 0x22, 0x41, 0x00},   // <
    {0x14, 0x14, 0x14, 0x14, 0x14},   // =
    {0x00, 0x41, 0x22, 0x14, 0x08},   // >
    {0x02, 0x01, 0x51, 0x09, 0x06},   // ?
    {0x32, 0x49, 0x59, 0x51, 0x3E},   // @

    {0x7C, 0x12, 0x11, 0x12, 0x7C},   // A
    {0x7F, 0x49, 0x49, 0x49, 0x36},   // B
    {0x3E, 0x41, 0x41, 0x41, 0x22},   // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C},   // D
    {0x7F, 0x49, 0x49, 0x49, 0x41},   // E
    {0x7F, 0x09, 0x09, 0x09, 0x01},   // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A},   // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},   // H
    {0x00, 0x41, 0x7F, 0x41, 0x00},   // I
    {0x20, 0x40, 0x41, 0x3F, 0x01},   // J
    {0x7F, 0x08, 0x14, 0x22, 0x41},   // K
    {0x7F, 0x40, 0x40, 0x40, 0x40},   // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},   // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},   // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},   // O
    {0x7F, 0x09, 0x09, 0x09, 0x06},   // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},   // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},   // R
    {0x46, 0x49, 0x49, 0x49, 0x31},   // S
    {0x01, 0x01, 0x7F, 0x01, 0x01},   // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},   // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},   // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F},   // W
    {0x63, 0x14, 0x08, 0x14, 0x63},   // X
    {0x07, 0x08, 0x70, 0x08, 0x07},   // Y
    {0x61, 0x51, 0x49, 0x45, 0x43},   // Z

    {0x00, 0x7F, 0x41, 0x41, 0x00},   // [
    {0x55, 0xAA, 0x55, 0xAA, 0x55},   // Backslash (Checker pattern)
    {0x00, 0x41, 0x41, 0x7F, 0x00},   // ]
    {0x04, 0x02, 0x01, 0x02, 0x04},   // ^
    {0x40, 0x40, 0x40, 0x40, 0x40},   // _
    {0x00, 0x03, 0x05, 0x00, 0x00},   // `

    {0x20, 0x54, 0x54, 0x54, 0x78},   // a
    {0x7F, 0x48, 0x44, 0x44, 0x38},   // b
    {0x38, 0x44, 0x44, 0x44, 0x20},   // c
    {0x38, 0x44, 0x44, 0x48, 0x7F},   // d
    {0x38, 0x54, 0x54, 0x54, 0x18},   // e
    {0x08, 0x7E, 0x09, 0x01, 0x02},   // f
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C},   // g
    {0x7F, 0x08, 0x04, 0x04, 0x78},   // h
    {0x00, 0x44, 0x7D, 0x40, 0x00},   // i
    {0x40, 0x80, 0x84, 0x7D, 0x00},   // j
    {0x7F, 0x10, 0x28, 0x44, 0x00},   // k
    {0x00, 0x41, 0x7F, 0x40, 0x00},   // l
    {0x7C, 0x04, 0x18, 0x04, 0x78},   // m
    {0x7C, 0x08, 0x04, 0x04, 0x78},   // n
    {0x38, 0x44, 0x44, 0x44, 0x38},   // o
    {0xFC, 0x24, 0x24, 0x24, 0x18},   // p
    {0x18, 0x24, 0x24, 0x18, 0xFC},   // q
    {0x7C, 0x08, 0x04, 0x04, 0x08},   // r
    {0x48, 0x54, 0x54, 0x54, 0x20},   // s
    {0x04, 0x3F, 0x44, 0x40, 0x20},   // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C},   // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C},   // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C},   // w
    {0x44, 0x28, 0x10, 0x28, 0x44},   // x
    {0x1C, 0xA0, 0xA0, 0xA0, 0x7C},   // y
    {0x44, 0x64, 0x54, 0x4C, 0x44},   // z

    {0x00, 0x10, 0x7C, 0x82, 0x00},   // {
    {0x00, 0x00, 0xFF, 0x00, 0x00},   // |
    {0x00, 0x82, 0x7C, 0x10, 0x00},   // }
    {0x00, 0x06, 0x09, 0x09, 0x06},    // ~ (Degrees)
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},    // filled in completely char
    {0x20, 0x70, 0xE0, 0x70, 0x20}	  // heart char
};
/*************************************************************************************************/
//I2C Functions

//Basic write function
static int Write(unsigned char *buffer, int length){
    return i2c_master_send(my_i2c_client, buffer, length);
}

static void OLED_WRITE(bool cmd, unsigned char chr){
int tempp = 0;
unsigned char buffer[2];
buffer[0]=0;
buffer[1]=0;
if(cmd){
    buffer[0]=0x00;//the control byte sets a byte as a command if it is 0x00 or 0000 0000
}else{
    buffer[0]=0x40;//the control byte sets a byte as data if it is 0x40 or 0100 0000
}
buffer[1] = chr;
tempp = Write(buffer, 2);
}

static void OLED_FILL(unsigned char chr){//0x00
    int total = 128*8;//128*8= area of display
    for(int i = 0; i < total; i++){
        OLED_WRITE(false, chr);
    }
}

static void OLED_SETCURSOR(uint8_t linePOS, uint8_t cursorPOS){
if((linePOS<7)&&(cursorPOS<127)){
    HEIGHT = linePOS;
    CURSOR = cursorPOS;

    OLED_WRITE(true, 0x21);//command for column address
    OLED_WRITE(true, cursorPOS);//start addr
    OLED_WRITE(true, 127);//end addr
    OLED_WRITE(true, 0x22);//command for page addr
    OLED_WRITE(true, linePOS);//start addr
    OLED_WRITE(true, 7);//end addr
    }
}
static void OLED_NEWLINE(void){
	CURSOR=0;
	OLED_WRITE(true, 0x22);
	HEIGHT++;
	OLED_WRITE(true,HEIGHT);
	OLED_WRITE(true, 7);
}
static void OLED_WRITE_CHAR(unsigned char chr){//ADD fonts so this shit works
    if(chr == '\n'||(CURSOR+5)>=127){
       OLED_NEWLINE();
    }else{
    unsigned char byte;
    chr -= 0x20;
    pr_info("byte: %d",chr);
    for(uint8_t tmp = 0; tmp < 5; tmp++){
        byte = SSD1306_font[chr][tmp];
        OLED_WRITE(false, byte);
        CURSOR++;
    }
    OLED_WRITE(false, 0x00);
    //CURSOR++;
    }
}
static int OLED_WRITE_CHARS(unsigned char* chars){//new function that writes string to display instead of making multiple I2C calls which is silly
    int length = strlen(chars);
    char buffer[161] = {0x40, 0x00};//0x40 is the data byte
    int index = 1;
    unsigned char chr;
    if(length == 0||length >= 33 ){
        return -1;
    }
    for(int i = 0; i < length; i++){
        chr = chars[i];
        chr -= 0x20;
        for(uint8_t tmp = 0; tmp < 5; tmp++){
            buffer[index] = SSD1306_font[chr][tmp];
            index++;
        }
    }
    return i2c_master_send(my_i2c_client, buffer, index);
}

static int OLED_INIT(void){
    //from embedtronicx article on SSD1306 here:https://embetronicx.com/tutorials/linux/device-drivers/ssd1306-i2c-linux-device-driver-using-raspberry-pi/ 
  OLED_WRITE(true, 0xAE); // Entire Display OFF
  OLED_WRITE(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
  OLED_WRITE(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
  OLED_WRITE(true, 0xA8); // Set Multiplex Ratio
  OLED_WRITE(true, 0x3F); // 64 COM lines
  OLED_WRITE(true, 0xD3); // Set display offset
  OLED_WRITE(true, 0x00); // 0 offset
  OLED_WRITE(true, 0x40); // Set first line as the start line of the display
  OLED_WRITE(true, 0x8D); // Charge pump
  OLED_WRITE(true, 0x14); // Enable charge dump during display on
  OLED_WRITE(true, 0x20); // Set memory addressing mode
  OLED_WRITE(true, 0x00); // Horizontal addressing mode
  OLED_WRITE(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
  OLED_WRITE(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
  OLED_WRITE(true, 0xDA); // Set com pins hardware configuration
  OLED_WRITE(true, 0x12); // Alternative com pin configuration, disable com left/right remap
  OLED_WRITE(true, 0x81); // Set contrast control
  OLED_WRITE(true, 0x80); // Set Contrast to 128
  OLED_WRITE(true, 0xD9); // Set pre-charge period
  OLED_WRITE(true, 0x22); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
  OLED_WRITE(true, 0xDB); // Set Vcomh deselect level
  OLED_WRITE(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
  OLED_WRITE(true, 0xA4); // Entire display ON, resume to RAM content display
  OLED_WRITE(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
  OLED_WRITE(true, 0x2E); // Deactivate scroll
  OLED_WRITE(true, 0xAF); // Display ON in normal mode
  //Clear the display
  OLED_FILL(0x00);
  return 0;
}
/*************************************************************************************************/
//Driver init and exit
static const struct i2c_device_id id[] = {
    {SLAVE_NAME, 0},{}
};
MODULE_DEVICE_TABLE(i2c, id);

static void remove(struct i2c_client *myClient){
    OLED_SETCURSOR(0,0);
    OLED_FILL(0xFF);
    OLED_WRITE(true, 0xAE);
    pr_info("bye bye haha\n");
}
static int probe(struct i2c_client *myClient, const struct i2c_device_id *id){
	unsigned char buffer[20] = "my first driver!"; 
    OLED_INIT();
    OLED_SETCURSOR(0,0);
   	for(int i = 0;i < 20;i++){OLED_WRITE_CHAR(buffer[i]);}
    pr_info("probe haha!\n");
    return 0;
}

static struct i2c_driver my_i2c_driver = {.driver = {.name = SLAVE_NAME, .owner = THIS_MODULE,},
.probe = probe,
.remove = remove,
.id_table = id,
};


/*************************************************************************************************/
//ProcFS Functions 
/*While the below userWrite and userOppen functions may look like duplicate of functions futher below, 
These functions are called by the User and are essentially wrappers for the below functions that ensure
that pointers from users are safe to use. */

/*takes input from the SSD1306 write wrapper the first byte will be set to a value by the wrapper that will determine what static function to execute here.
val 0x01 is the OLED_SETCURSOR function. 
val 0x02 is the OLED_WRITE_STRING function.
val 0x03 is the OLED_FILL function.
val 0x04 is the OLED_RAW_WRITE function.
val 0x05 is the OLED_WRITE_CHAR function
*/
static int userWrite(struct file *file, const char __user *userBuf, size_t size, loff_t *ppos){
	int bytesMissed;
	pr_info("userWrite succesfully Called WOOT WOOT\n");  
	bytesMissed = copy_from_user((void*) myBuf, (void*)userBuf, (unsigned long) size);
	if(bytesMissed==0){
	switch (myBuf[0]){
		case 0x01://OLED_SETCURSOR
			if(sizeof(myBuf)<2){
				pr_info("too few arguments for OLED_SETCURSOR");
				return -1;
			}
			OLED_SETCURSOR((uint8_t)myBuf[1],(uint8_t)myBuf[2]);
		return 0;
		break;
		
		case 0x02://OLED_WRITE_STRING
			for(int i = 1;i < size;i++){
				OLED_WRITE_CHAR(myBuf[i]);
			}
		break;
		
		case 0x03://OLED_FILL
			if(sizeof(myBuf)<1){
				pr_info("too few arguments for OLED_FILL");
				return -1;
			}
			OLED_FILL(myBuf[1]);
			return 0;
		break;
		
		case 0x04://OLED_WRITE_CHAR
			if(sizeof(myBuf)<1){
				pr_info("too few arguments for OLED_WRITE_CHAR");
				return -1;
			}
			OLED_WRITE_CHAR(myBuf[1]);
			return 0;
		break;
		
		default:
		return -1;
	}
		return 0;
	}
	return -1; 
}
static int userOpen(struct inode* usrNode, struct file* fp){
	pr_info("userOpen sucessfully Called WOOT WOOT");
	return 0;
}
static struct proc_ops userOps = {
	.proc_open = userOpen,
	.proc_write = userWrite

};
//Structs and Functions for Linux drivers
static struct i2c_board_info my_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_NAME, ADDRESS)
};

static int __init driver_driver_init(void){
    int ReturnVAL = -1;
   	
    my_i2c_adapter = i2c_get_adapter(I2C_BUS);
    if(my_i2c_adapter != NULL){
        my_i2c_client = i2c_new_client_device(my_i2c_adapter, &my_i2c_board_info);
        if(my_i2c_client != NULL){
            i2c_add_driver(&my_i2c_driver);
            ReturnVAL = 0;
        }
        i2c_put_adapter(my_i2c_adapter);
    }
    pr_info("Added the Driver haha!!!\n");
    dir = proc_mkdir("dev",NULL);
    if(dir == NULL){
    	pr_info("proc dir couldnt be created\n");
    }
    myEntry = proc_create("usr",0666,dir, &userOps);
    return ReturnVAL;
}

static void __exit driver_exit(void){
    i2c_unregister_device(my_i2c_client);
    i2c_del_driver(&my_i2c_driver);
    proc_remove(myEntry);
    proc_remove(dir);
    pr_info("No more Driver haha!!!\n");
}

/*************************************************************************************************/
module_init(driver_driver_init);
module_exit(driver_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Asher Rosen");
MODULE_DESCRIPTION("SSD1306 I2C Driver");
MODULE_VERSION("1.0");
