/*
 * ultrasond.c
 *
 *  Created on: Apr 23, 2018
 *      Author: tang
 */

#include "ultrasond.h"
#define c 0x01
// Task configuration
#define ULTRASOND_TASK_PRIORITY                     1

#ifndef ULTRASOND_TASK_STACK_SIZE
#define ULTRASOND_TASK_STACK_SIZE                   1024
#endif
#define ADCBUFFERSIZE    (500)

//Board Specific
#define ADC_CHANNEl Board_ADCBUF0CHANNEL0


Task_Struct UltrasondTask;
Char UltrasondTaskStack[ULTRASOND_TASK_STACK_SIZE];

uint16_t sampleBufferOne[ADCBUFFERSIZE];
uint16_t sampleBufferTwo[ADCBUFFERSIZE];
uint32_t microVoltBuffer[ADCBUFFERSIZE];
uint32_t buffersCompletedCounter = 0;

static void ultrasond_taskFxn(UArg a0, UArg a1);
static void ultrasond_init(void);

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
                    void *completedADCBuffer, uint32_t completedChannel)
{
        uint_fast16_t i;
        uint_fast16_t uartTxBufferOffset;

        /* Adjust raw adc values and convert them to microvolts */
        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE,
            completedChannel);
        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel,
            completedADCBuffer, microVoltBuffer, ADCBUFFERSIZE);
      /*  for (i = 0; i < ADCBUFFERSIZE; i++) {
               System_printf("sample %d %u\n", i,(unsigned int)microVoltBuffer[i]);
               System_flush();
        }
    System_printf("Finished\n");
    System_flush();*/
    ADCBuf_convert(handle, conversion, 1);
}

void ultrasond_createTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = UltrasondTaskStack;
    taskParams.stackSize = ULTRASOND_TASK_STACK_SIZE;
    taskParams.priority = ULTRASOND_TASK_PRIORITY;

    Task_construct(&UltrasondTask, ultrasond_taskFxn, &taskParams, NULL);
}

static void ultrasond_init(void)
{
    System_printf("ultrasond Init\n");
    System_flush();
}
/*
static void ultrasond_taskFxn(UArg a0, UArg a1)
{
    ultrasond_init();
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;
    int cpt=0;
    ADC_Params_init(&params);



    while(1){
        adc = ADC_open(Board_ADC0, &params);

            if (adc == NULL) {
                System_printf("Error initializing ADC channel 0\n");
                System_flush();
                while (1);
            }
        while(cpt<SAMPLE_NB){
            // Blocking mode conversion
            res = ADC_convert(adc, &adcValue0);
            if (res == ADC_STATUS_SUCCESS) {

                adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);

                System_printf("ADC channel 0 convert result: %d uV\n", adcValue0MicroVolt);
                System_flush();
            }
            else {
                System_printf("ADC channel 0 convert failed\n");
                System_flush();
            }
            cpt++;
        }
        cpt = 0;
    ADC_close(adc);
}
    return (NULL);
}
*/

static void ultrasond_taskFxn(UArg a0, UArg a1)
{
    ultrasond_init();
        ADCBuf_Handle adcBuf;
        ADCBuf_Params adcBufParams;
        ADCBuf_Conversion continuousConversion;

        /* Call driver init functions */
        ADCBuf_init();

    /*
     * The CC2650DK_7ID and CC1310DK_7XD measure an ambient light sensor in this example.
     * It is not powered by default to avoid high current consumption in other examples.
     * The code below turns on the power to the sensor.
     */
    #if defined(CC2650DK_7ID) || defined(CC1310DK_7XD)
        PIN_State pinState;

        PIN_Config AlsPinTable[] =
        {
            Board_ALS_PWR    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH    | PIN_PUSHPULL, /* Turn on ALS power */
            PIN_TERMINATE                                                            /* Terminate list */
        };

        /* Turn on the power to the ambient light sensor */
        PIN_open(&pinState, AlsPinTable);
    #endif


        /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
        ADCBuf_Params_init(&adcBufParams);
        adcBufParams.callbackFxn = adcBufCallback;
        adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
        adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
        adcBufParams.samplingFrequency = 200000;
        adcBuf = ADCBuf_open(Board_ADC0, &adcBufParams);


        /* Configure the conversion struct */
        continuousConversion.arg = NULL;
        continuousConversion.adcChannel = ADC_CHANNEl;
        continuousConversion.sampleBuffer = sampleBufferOne;
        continuousConversion.sampleBufferTwo = sampleBufferTwo;
        continuousConversion.samplesRequestedCount = ADCBUFFERSIZE;

        if (!adcBuf){
            /* AdcBuf did not open correctly. */
            System_printf("adcbuf open failed\n");
            System_flush();
            while(1);
        }

        /* Start converting. */
        int r = ADCBuf_convert(adcBuf, &continuousConversion, 1);
        if ( r!=ADCBuf_STATUS_SUCCESS) {
            /* Did not start conversion process correctly. */
            System_printf("convert failed return code : %d\n",r);
            System_flush();
            while(1);
        }

        /*
         * Go to sleep in the foreground thread forever. The data will be collected
         * and transfered in the background thread
         */
        while(1) {
            Task_sleep(1000);
        }
}








