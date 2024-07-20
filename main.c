/*
Simple program to demonstrate communicating with a BMI270 over SPI on an MSP430.
This example uses the MSP-EXP430FR6989 Launchpad and its EUSCI_B0 SPI interface,
which has the following pinout:

P1.4: UCB0CLK (serial clock) -> BMI270 pin 13
P1.5: CSB (chip select bar) -> BMI270 pin 12
P1.6: UCB0SIMO (peripheral in, controller out) -> BMI270 pin 14
P1.7: UCB0SOMI (peripheral out, controller in) -> BMI270 pin 1
*/

#include <stdio.h>
#include "gpio.h"
#include <driverlib.h>
#include <math.h>
#include "BMI270_SensorAPI/bmi270.h"
#include "bmi270_spi.h"
#include "util.h"
#include "cs.h"
#include "classification/analyze.h"
#include "classification/classify.h"
#include "commons/data_formats.h"

 // 200hz * 20sec
#define DATA_LEN 1000

#pragma PERSISTENT(sensor_data)
static struct BMI2SensData sensor_data[DATA_LEN] = { { { 0 } } };

/******************************************************************************/
/*!                Macro definition                                           */

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/*! Macros to select the sensors                   */
#define ACCEL          UINT8_C(0x00)
#define GYRO           UINT8_C(0x01)

/******************************************************************************/
/*!           Static Function Declaration                                     */

/*!
 *  @brief This internal API is used to set configurations for accel.
 *
 *  @param[in] bmi       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi);

/*!
 *  @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 *  range 2G, 4G, 8G or 16G.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] g_range   : Gravity range.
 *  @param[in] bit_width : Resolution for accel.
 *
 *  @return Accel values in meter per second squared.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);

/*!
 *  @brief This function converts lsb to degree per second for 16 bit gyro at
 *  range 125, 250, 500, 1000 or 2000dps.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] dps       : Degree per second.
 *  @param[in] bit_width : Resolution for gyro.
 *
 *  @return Degree per second.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

/******************************************************************************/
/*!            Functions                                        */

void init_spi() {
    // Set pins P1.6 and P1.4 as UCB0SIMO and UCB0CLK respectively
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN4,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Set pin P1.7 as UCB0SOMI
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN7,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // While it is possible to set this as an SPI chip select pin (UCB0STE), it should instead
    // be set just as a normal GPIO output, so that it doesn't get driven low after every write.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);
    __delay_cycles(100);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    EUSCI_B_SPI_initMasterParam param = {
        .selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .clockSourceFrequency = CS_getSMCLK(),
        .desiredSpiClock = 2000000,
        // Per the datasheet, the BMI270 supports either 00 (the current setting) or 11 for clockPhase and clockPolarity.
        // This is automatically detected by the BMI270.
        .clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
        .clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
        .msbFirst = EUSCI_B_SPI_MSB_FIRST,
        .spiMode = EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW
    };
    EUSCI_B_SPI_initMaster(SPI_BASE, &param);
    // Honestly I have no idea what this next line does, it might do nothing
    EUSCI_B_SPI_select4PinFunctionality(SPI_BASE, EUSCI_B_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);
    EUSCI_B_SPI_enable(SPI_BASE);
}

void init_clk() {
    // Set DCO Frequency to 8 MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_3);

    // Configure MCLK, SMCLK to be sourced by DCOCLK
    CS_initClockSignal(CS_MCLK,  CS_DCOCLK_SELECT,  CS_CLOCK_DIVIDER_1); // 8 MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT,  CS_CLOCK_DIVIDER_1); // 8 MHz

    //Set external clock frequency to 32.768 KHz
    // CS_setExternalClockSource(32768, 0);
    // //Set ACLK=XT1
    // CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // //Start XT1 with no time out
    // CS_turnOnLFXT(CS_LFXT_DRIVE_0);
}

/*!
 * @brief This internal API is used to set configurations for any-motion.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev)
{

    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_ANY_MOTION;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* 1LSB equals 20ms. Default is 100ms, setting to 80ms. */
        config.cfg.any_motion.duration = 0x01;

        /* 1LSB equals to 0.48mg. Default is 83mg, setting to 50mg. */
        config.cfg.any_motion.threshold = 0x180;

        /* Set new configurations. */
        rslt = bmi270_set_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

int main(void) {
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to define limit to print accel data. */
    uint32_t limit = DATA_LEN;

    /* Assign accel and gyro sensor to variable. */
    uint8_t sensor_list[3] = { BMI2_ACCEL, BMI2_GYRO, BMI2_ANY_MOTION };

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi;

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_sens_data sensor_data_temp = {0};
    
    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_ANY_MOTION, .hw_int_pin = BMI2_INT1 };

    uint32_t indx = 0;

    float acc_x = 0, acc_y = 0, acc_z = 0;
    float gyr_x = 0, gyr_y = 0, gyr_z = 0;
    struct bmi2_sens_config config;

    /* Variable to get any-motion interrupt status. */
    uint16_t int_status = 0;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    init_clk();
    init_spi();
    init_bmi_device(&bmi);

    char output[64];
    int len;

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Accel and gyro configuration settings. */
        rslt = set_accel_gyro_config(&bmi);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* NOTE:
             * Accel and Gyro enable must be done after setting configurations
             */
            rslt = bmi270_sensor_enable(sensor_list, 3, &bmi);
            bmi2_error_codes_print_result(rslt);

            if (rslt != BMI2_OK) return;

            /* Set feature configurations for any-motion. */
            rslt = set_feature_config(&bmi);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {
                config.type = BMI2_ACCEL;

                /* Get the accel configurations. */
                rslt = bmi2_get_sensor_config(&config, 1, &bmi);
                bmi2_error_codes_print_result(rslt);

                // len = sprintf(output,
                //     "Data set, Time, Accel Range, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z\r\n");
                // uart_write(0, output, len);

                /* Map the feature interrupt for any-motion. */
                rslt = bmi270_map_feat_int(&sens_int, 1, &bmi);
                bmi2_error_codes_print_result(rslt);
                printf("Move the board\n");

                /* Loop to get any-motion interrupt. */
                do
                {
                    /* Clear buffer. */
                    int_status = 0;

                    /* To get the interrupt status of any-motion. */
                    rslt = bmi2_get_int_status(&int_status, &bmi);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of any-motion. */
                    if (int_status & BMI270_ANY_MOT_STATUS_MASK)
                    {

                        indx = 0;
                        while (indx < limit)
                        {
                            rslt = bmi2_get_sensor_data(&sensor_data_temp, &bmi);
                            // bmi2_error_codes_print_result(rslt);

                            if ((rslt == BMI2_OK) && (sensor_data_temp.status & BMI2_DRDY_ACC) &&
                                (sensor_data_temp.status & BMI2_DRDY_GYR))
                            {   
                                sensor_data[indx].acc = (struct BMI2SensAxisData){
                                    .x = sensor_data_temp.acc.x,
                                    .y = sensor_data_temp.acc.y,
                                    .z = sensor_data_temp.acc.z};
                                sensor_data[indx].gyr = (struct BMI2SensAxisData){
                                    .x = sensor_data_temp.gyr.x,
                                    .y = sensor_data_temp.gyr.y,
                                    .z = sensor_data_temp.gyr.z};
                                indx++;
                            }
                        }

                        AnalysisResult result = {0};
                        printf("analyzing...\n");
                        AnalysisStatus status = analyze(sensor_data, DATA_LEN, sensor_data[0], &result);
                        
                        printf("analyzed!\n");
                        switch (result.classification) {
                            case SIT: printf("SIT\n"); break;
                            case STAND: printf("STAND\n"); break;
                            case OTHER: printf("OTHER\n"); break;
                            case LENGTH: printf("LENGTH\n"); break;
                        }
                    }
                } while (rslt == BMI2_OK);

                
            }
        }
    }
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}