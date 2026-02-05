/*********************************************************************************\
*
* MMMMMMMMMMMM   SSSSSSSSSSSS   WW   WW   WW   MECHATRONIK
* MM   MM   MM   SS             WW   WW   WW   SCHULE
* MM   MM   MM   SSSSSSSSSSSS   WW   WW   WW   WINTERTHUR
* MM   MM   MM             SS   WW   WW   WW   
* MM   MM   MM   SSSSSSSSSSSS   WWWWWWWWWWWW   www.msw.ch
*
*
* Dateiname: ESP32_Board_Treiber.c
*
* Projekt  : ESP32 Board Treiber 
* Hardware : ESP32-S3
*
* Copyright: MSW, E4
*
* Beschreibung:
* =============
* Treiber für das ESP32 Board
*
* Portbelegung:
* =============
* Siehe Hardwarestruktur
*
* Verlauf:
* ========
* Datum:      Autor:         Version   Grund der Änderung:
* 23.08.2024  S. Huruvarshan V1.0      Neuerstellung
*
\*********************************************************************************/
#include "ESP-Driver.h"

#define INPUT_BTN0 21 
#define INPUT_BTN1 35 
#define INPUT_BTN2 36 
#define INPUT_BTN3 37
#define GPIO_INPUT_SEL ((1ULL << INPUT_BTN0) | (1ULL << INPUT_BTN1) | (1ULL << INPUT_BTN2) | (1ULL << INPUT_BTN3)); 

#define ADDR_IC2 0x20 
#define ADDR_IC3 0x21 
#define SCL 46 
#define SDA 47
#define FREQ_I2C 100000

#define IOCON_BANK_MCP23017 0x05
#define IOCON_BANK_RST_MCP23017 0x00
#define IODIRA_MCP23017 0x00
#define PORT_OUTPUT_MC23017 0x00
#define PORT_INPUT_MC23017 0xff
#define IODIRB_MCP23017 0x01
#define OLATA_MCP23017 0x14
#define OLATB_MCP23017 0x15
#define GPIOA_MCP23017 0x12
#define GPIOB_MCP23017 0x13

// Function prototypes
void initI2C (void); 
void mcp23017WriteRegister(uint8_t address, uint8_t reg, uint8_t data);
void mcp23017InitIC2(void);
void mcp23017InitIC3(void); 
void ledWriteAll(uint16_t bitMask); 
uint8_t mcp23017ReadRegister(uint8_t address, uint8_t reg); 
uint8_t swtichReadAll();
uint8_t buttonReadAll(); 

// New I2C driver variables
static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static i2c_master_dev_handle_t i2c_dev_handle_ic2 = NULL;
static i2c_master_dev_handle_t i2c_dev_handle_ic3 = NULL;


void initBoard(uint8_t startAnimation){
    gpio_config_t io_conf = {}; // GPIO configuration
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupt
    io_conf.mode = GPIO_MODE_INPUT; // Set GPIO mode to input
    io_conf.pin_bit_mask = GPIO_INPUT_SEL; // Set the GPIO pin bit mask
    io_conf.pull_down_en = 0; // Disable pull-down resistor
    io_conf.pull_up_en = 0; // Disable pull-up resistor
    gpio_config(&io_conf); // Configure the GPIO

    initI2C(); // Initialize I2C
    mcp23017InitIC3(); // Initialize MCP23017
    mcp23017InitIC2(); // Initialize MCP23017
} 

void initI2C (void){ 
    // Old I2C driver initialization (commented out)
    /*
    int i2c_master_port = 0; // I2C port number for master dev
    i2c_config_t i2c_config = { // I2C master configuration
        .mode = I2C_MODE_MASTER, // I2C mode
        .sda_io_num = SDA, // GPIO number for I2C data signal
        .sda_pullup_en = GPIO_PULLUP_ENABLE, // enable pull-up resistors
        .scl_io_num = SCL, // GPIO number for I2C clock signal
        .scl_pullup_en = GPIO_PULLUP_ENABLE, // enable pull-up resistors
        .master.clk_speed = FREQ_I2C, // I2C clock frequency for master mode
        .clk_flags = 0, // I2C clock flags
    };    
    i2c_param_config(I2C_NUM_0, &i2c_config); // Configure the I2C master driver
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0); // Install the I2C master driver
    */

    gpio_reset_pin(SDA);
    gpio_reset_pin(SCL);


    // New I2C driver initialization
    i2c_master_bus_config_t i2c_bus_config = {0};
        i2c_bus_config.i2c_port = I2C_NUM_0;
        i2c_bus_config.sda_io_num = SDA;
        i2c_bus_config.scl_io_num = SCL;
        i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
        i2c_bus_config.glitch_ignore_cnt = 0;//7
        i2c_bus_config.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));

    i2c_device_config_t i2c_dev_config_ic2 = {0};
        i2c_dev_config_ic2.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        i2c_dev_config_ic2.device_address = ADDR_IC2;
        i2c_dev_config_ic2.scl_speed_hz = FREQ_I2C;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config_ic2, &i2c_dev_handle_ic2));

    i2c_device_config_t i2c_dev_config_ic3 = {0};
        i2c_dev_config_ic3.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        i2c_dev_config_ic3.device_address = ADDR_IC3;
        i2c_dev_config_ic3.scl_speed_hz = FREQ_I2C;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config_ic3, &i2c_dev_handle_ic3));
}

void mcp23017WriteRegister(uint8_t address, uint8_t reg, uint8_t data){
    // Old I2C driver code (commented out)
    /*
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); // Create a new I2C command link
    i2c_master_start(cmd); // Start the I2C command
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true); // Write the I2C address
    i2c_master_write_byte(cmd, reg, true); // Write the register address
    i2c_master_write_byte(cmd, data, true); // Write the data
    i2c_master_stop(cmd); // Stop the I2C command 
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000)); // Execute the I2C command
    i2c_cmd_link_delete(cmd); // Delete the I2C command link
    */

    // New I2C driver code
    i2c_master_dev_handle_t dev_handle = (address == ADDR_IC2) ? i2c_dev_handle_ic2 : i2c_dev_handle_ic3;

    if (dev_handle == NULL) {
        ESP_LOGE("I2C", "Fehler: Handle für 0x%02x ist NULL!", address);
        return;
    }

    uint8_t write_buf[2] = {reg, data};
    //ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), pdMS_TO_TICKS(1000)));
    esp_err_t ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK) {
        ESP_LOGE("I2C_DEBUG", "Adresse 0x%02X meldet Fehler: %s", address, esp_err_to_name(ret));
        return; // Nicht abstürzen, einfach weitermachen
    }
}

void mcp23017InitIC2(void){
    mcp23017WriteRegister(ADDR_IC2, IOCON_BANK_MCP23017, IOCON_BANK_RST_MCP23017); // Reset IOCON BANK on MCP23017
    mcp23017WriteRegister(ADDR_IC2, IODIRA_MCP23017, PORT_OUTPUT_MC23017);  // Set IODIRA to output
    mcp23017WriteRegister(ADDR_IC2, IODIRB_MCP23017, PORT_OUTPUT_MC23017);  // Set IODIRB to output
} 

void mcp23017InitIC3(void){
    mcp23017WriteRegister(ADDR_IC3, IOCON_BANK_MCP23017, IOCON_BANK_RST_MCP23017); // Reset IOCON BANK on MCP23017
    mcp23017WriteRegister(ADDR_IC3, IODIRA_MCP23017, PORT_INPUT_MC23017);  // Set IODIRA to input
    mcp23017WriteRegister(ADDR_IC3, IODIRB_MCP23017, PORT_INPUT_MC23017);  // Set IODIRB to input
}

void ledWriteAll(uint16_t bitMask){
    mcp23017WriteRegister(ADDR_IC2, OLATA_MCP23017, bitMask & 0xff);
    mcp23017WriteRegister(ADDR_IC2, OLATB_MCP23017, bitMask >> 8);
} 

uint8_t mcp23017ReadRegister(uint8_t address, uint8_t reg){
    // Old I2C driver code (commented out)
    /*
    uint8_t data = 0; // Initialize data variable 
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); // Create a new I2C command link 
    i2c_master_start(cmd); // Start the I2C command 
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true); // Read the I2C address 
    i2c_master_write_byte(cmd, reg, true); // write the register address 
    i2c_master_start(cmd); // Start the I2C command 
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true); // Read the I2C address 
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK); // Read the data
    i2c_master_stop(cmd); // Stop the I2C command
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000)); // Execute the I2C command 
    i2c_cmd_link_delete(cmd); // Delete the I2C command link 
    return data; // Return the data
    */

    // New I2C driver code
    uint8_t data = 0;
    i2c_master_dev_handle_t dev_handle = (address == ADDR_IC2) ? i2c_dev_handle_ic2 : i2c_dev_handle_ic3;

    if (dev_handle == NULL) {
        ESP_LOGE("I2C_READ", "Fehler: Handle für 0x%02x ist NULL!", address);
        return 0;
    }

    //ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &reg, 1, &data, 1, pdMS_TO_TICKS(1000)));
    esp_err_t ret = i2c_master_transmit_receive(dev_handle, &reg, 1, &data, 1, pdMS_TO_TICKS(1000));

    if (ret != ESP_OK) {
        ESP_LOGE("I2C_READ", "Read-Fehler bei Chip 0x%02X, Reg 0x%02X: %s", address, reg, esp_err_to_name(ret));
        return 0; // Oder ein spezifischer Fehlerwert
    }
    return data;
}

uint8_t swtichReadAll(){ 
    return mcp23017ReadRegister(ADDR_IC3, GPIOA_MCP23017); // Read the data from GPIOA and return it
}

uint8_t buttonReadAll(){
    uint8_t data = 0; // Initialize data variable 
    data = gpio_get_level(INPUT_BTN0) | gpio_get_level(INPUT_BTN1) << 1 | gpio_get_level(INPUT_BTN2) << 2 | gpio_get_level(INPUT_BTN3) << 3; // Read the data from the buttons
    return data; // Return the data
}