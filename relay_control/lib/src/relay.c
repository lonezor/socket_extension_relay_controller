#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/input.h>

#include <rc/relay.h>

//-------------------------------------------------------------------------------------------------------------------

#define I2C_DEV_BASE_PATH "/dev/i2c-1"
#define I2C_ADDRESS_RELAY_MODULE (0x20)
#define CONTROL_REGISTER_GPIO  (0x12) // word: 0x12 - 0x13
#define CONTROL_REGISTER_IODIR (0x00) // word: 0x00 - 0x01

//-------------------------------------------------------------------------------------------------------------------

static int open_dev()
{
    int fd = open(I2C_DEV_BASE_PATH, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "error: open I2C device failed\n");
        return -1;
    }

   if (ioctl(fd, I2C_SLAVE, I2C_ADDRESS_RELAY_MODULE) < 0)
	{
        close(fd);
        fprintf(stderr, "error: configuring I2C slave address failed\n");
        return -1;
    }

    return fd;
}

//-------------------------------------------------------------------------------------------------------------------

static uint16_t rc_read_relay_state()
{
    uint16_t state = 0;

    int fd = open_dev();
    if (fd == -1) {
        return 0;
    }

    struct i2c_smbus_ioctl_data  blk;
    union i2c_smbus_data i2cdata;

    blk.read_write = 1;
    blk.command = CONTROL_REGISTER_IODIR;
    blk.size = I2C_SMBUS_I2C_BLOCK_DATA;
    blk.data = &i2cdata;
    i2cdata.block[0] = sizeof(uint16_t);

    if(ioctl(fd, I2C_SMBUS, &blk) < 0) {
        close(fd);
        fprintf(stderr, "error: reading I2C bus failed\n");
        return 0;
    }

    memcpy(&state, &i2cdata.block[1], sizeof(uint16_t));

    return state;
}

//-------------------------------------------------------------------------------------------------------------------

static void rc_write_relay_state(uint8_t cmd, uint16_t state)
{
    int fd = open_dev();
    if (fd == -1) {
        return;
    }

    struct i2c_smbus_ioctl_data  blk;
    union i2c_smbus_data i2cdata;

    i2cdata.word = state;
    blk.read_write = 0;
    blk.command = cmd;
    blk.size = I2C_SMBUS_WORD_DATA;
    blk.data = &i2cdata;

    if(ioctl(fd, I2C_SMBUS, &blk) < 0){
        close(fd);
        fprintf(stderr, "error: writing I2C bus failed\n");
        return;
    }
}

//-------------------------------------------------------------------------------------------------------------------

void rc_relay_channel_init()
{
    rc_write_relay_state(CONTROL_REGISTER_GPIO, 0xffff);
}

//-------------------------------------------------------------------------------------------------------------------

void rc_relay_channel_set(rc_relay_channel_t ch, bool enabled)
{
    // Get state for all channels (API bitfield representation)
    uint16_t state = ~rc_read_relay_state();
    
    // Update channel specific bit in the bitfield
    if (enabled) {
        state |= (uint16_t)ch;
    } else {
        state &= ~(uint16_t)ch;
    }

    rc_write_relay_state(CONTROL_REGISTER_IODIR, ~state);
}

//-------------------------------------------------------------------------------------------------------------------

bool rc_relay_channel_get(rc_relay_channel_t ch)
{
    // Get state for all channels (API bitfield representation)
    uint16_t state = ~rc_read_relay_state();

    // Determine if requested channel(s) are enabled (completely)
    uint16_t res = (state & (uint16_t)ch);

    if (ch == rc_relay_channel_all) {
        return (res == 0xffff);
    } else {
        return res > 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
