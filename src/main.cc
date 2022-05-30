/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"
#include "hx_drv_tflm.h"
#include "stdio.h"

//#include "synopsys_wei_gpio.h"


char string_buf[100] = "test\n";

#define accel_scale 1000

typedef struct
{
	uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;
volatile void delay_ms(unsigned int ms)
{
  volatile unsigned int delay_i = 0;
  volatile unsigned int delay_j = 0;
  volatile const unsigned int delay_cnt = 20000;

  for(delay_i = 0; delay_i < ms; delay_i ++)
    for(delay_j = 0; delay_j < delay_cnt; delay_j ++);
} 
volatile void delay_us(unsigned int us)
{
  uint32_t start = 0;
  uint32_t end = 0;
  uint32_t diff = 0;
  hx_drv_tick_get(&start);	

  do{
	hx_drv_tick_get(&end);	
	if(end < start){
		diff = 0XFFFFFFFF - start + end;
	}else{
		diff = (end - start);
	}
  }while( diff < (us * 400) );
} 

void GPIO_UART_TX(uint8_t *data, uint32_t len , hx_drv_gpio_config_t *Tx) //gpio0 為板子的Tx
{
	Tx -> gpio_data = 0;
    hx_drv_gpio_set(Tx);
	for ( int i = 0; i < len; i++ )
    {
        uint8_t c = data[i];
        
        // pull down as start bit
		Tx -> gpio_data = 0;
        hx_drv_gpio_set(Tx);
        delay_us(26);  // baudrate默認 38400 (26)
        for ( int j = 0; j < 8; j++ )
        {
            if ( c & 0x01 )
            {
				Tx->gpio_data = 1;
                hx_drv_gpio_set(Tx);  //GPIO_PIN_SET = 1
            }
            else
            {
				Tx->gpio_data = 0;
                hx_drv_gpio_set(Tx);
            }
            delay_us(26);
            c >>= 1;
        }
        // pull high as stop bit
		Tx->gpio_data = 1;
        hx_drv_gpio_set(Tx);
        delay_us(26);
    }
}
void GPIO_UART_RX(uint8_t *data, uint16_t *len, hx_drv_gpio_config_t * Rx) //gpio1 為板子的Rx
{
	sprintf(string_buf,"receive\n");
	hx_drv_uart_print(string_buf);
	int count = 0; 
	// while(1)
    // {
    //     uint8_t b[8] = { 0 };
    //     uint32_t c = 0;
    //     // wait for start bit
	// 	do{
	// 		hx_drv_gpio_get(Rx);
	// 	}while ( Rx->gpio_data == 1 );  // wait while gpio read high
    //     delay_us(26/2);
	// 	//delay_us(26);
    //     for ( int j = 0; j < 8; j++ )
    //     {
    //         delay_us(26);
	// 		hx_drv_gpio_get(Rx);
    //         b[j] = ( Rx->gpio_data) ? 1 : 0;
    //     }
        
    //     for ( int j = 0; j < 8; j++ )
    //     {
    //         c |= (b[j] << j);
    //     }
    //     data[count & 0x3fff] = c;

	// 	count++;
	// 	if(c == 0x0d){   // 0x0d'\r'
				
	// 		count |= 0x8000;
	// 	}else if((c == 0x0a) && (count & 0x8000) ){ //0x0a '\n'
	// 		count |= 0x4000;
	// 	}
	// 	if( count & 0x4000){
	// 		*len = (count & 0x3fff) - 2;
	// 		data[*len] = '\0';
	// 		break;
	// 	}
    //     // wait for stop bit
	// 	do{
	// 		hx_drv_gpio_get(Rx);
	// 	}
    //     while ( Rx->gpio_data == 0 );  //wait until gpio read high as stop bit
	// 	break;
    // }
	
	
}

static uint16_t sample_data = 301;
static uint16_t sample_data_nim_of_bytes = 17;

int main(int argc, char* argv[])
{

	float x_data[3][300];
	int signal_pass;
	uint8_t label;
	int32_t int_buf;
	accel_type accel_x, accel_y, accel_z;
	uint32_t msec_cnt = 0;
	uint32_t sec_cnt = 0;
	uint16_t counter = 0;
	uint16_t flag = 0;
	int32_t flagx,flagy,flagz;
	uint16_t str_length;
	hx_drv_gpio_config_t hal_gpio_ZERO;
	hx_drv_gpio_config_t hal_gpio_ONE;
	hx_drv_uart_initial(UART_BR_115200);

	//It will initial accelerometer with sampling rate 119 Hz, bandwidth 50 Hz, scale selection 4g at continuous mode.
	//Accelerometer operates in FIFO mode. 
	//FIFO size is 32

	hal_gpio_ZERO.gpio_pin = HX_DRV_PGPIO_0;  // 決定pin腳要用數字
    hal_gpio_ZERO.gpio_direction = HX_DRV_GPIO_OUTPUT;  //HX_DRV_GPIO_OUTPUT(3)
    hal_gpio_ZERO.gpio_data = 1;
	
	if(hx_drv_gpio_initial(&hal_gpio_ZERO) != HX_DRV_LIB_PASS)
    	hx_drv_uart_print("Accel 0 fail");
  	else
    	hx_drv_uart_print("Accel 0 success");

	hal_gpio_ONE.gpio_pin = HX_DRV_PGPIO_1;  // 決定pin腳要用數字
    hal_gpio_ONE.gpio_direction = HX_DRV_GPIO_INPUT;  //HX_DRV_GPIO_OUTPUT(3)
    hal_gpio_ONE.gpio_data = 0;

	
	if(hx_drv_gpio_initial(&hal_gpio_ONE) != HX_DRV_LIB_PASS)
    	hx_drv_uart_print("Accel 1 fail");
  	else
    	hx_drv_uart_print("Accel 1 success");


	if (hx_drv_accelerometer_initial() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Accelerometer Initialize Fail\n");
	else
		hx_drv_uart_print("Accelerometer Initialize Success\n");
	setup();
  
	uint8_t a[sample_data-1][sample_data_nim_of_bytes] ;   
	uint8_t t[3] = {'1','2','3'};
	uint8_t in[3];
	uint16_t count = 3;
	uint16_t one = 1;
	uint8_t temp_a[sample_data_nim_of_bytes];
	uint8_t send_bit;
	delay_ms(10);
	if (flag == 0)
	{
		GPIO_UART_RX(in,&count,&hal_gpio_ONE);
		flag +=1;
	}


	uint8_t key_data;
	uint8_t breakFlag = ' ';
	uint8_t user_number = 0;
	while (1) 
	{	
		hx_drv_uart_print("Enter User: \n");
		while(1){
			hx_drv_uart_getchar(&user_number);
			delay_ms(1000);
			if(user_number != 0) break;
		}
		delay_ms(1000);
		hx_drv_uart_print("Enter activity 1 - 6: \n");

		while(1){
			hx_drv_uart_getchar(&key_data);
			if(key_data == '1' || key_data == '2' || key_data == '3' || key_data == '4' || key_data == '5' || key_data == '6') {
				
				while(1){ // 125 x 500
				//while(msec_cnt < 75000){ // 125 x 500
					hx_drv_uart_getchar(&key_data);
					float x, y, z;
					hx_drv_accelerometer_receive(&x, &y, &z);

					x_data[0][counter] = x;
					x_data[1][counter] = y;
					x_data[2][counter] = z;

					int_buf = x_data[0][counter] * accel_scale; //scale value
					if(int_buf < 0)
					{
						int_buf = int_buf * -1;
						accel_x.symbol = '-';
						flagx = -1;
					}
					else 
					{
						accel_x.symbol = '+';
						flagx = 1;
					}
					accel_x.int_part = int_buf / accel_scale;
					accel_x.frac_part = int_buf % accel_scale;


					int_buf = x_data[1][counter] * accel_scale; //scale value
					if(int_buf < 0)
					{
						int_buf = int_buf * -1;
						accel_y.symbol = '-';
						flagy = -1;
					}
					else 
					{
						accel_y.symbol = '+';
						flagy = 1;
					}
					accel_y.int_part = int_buf / accel_scale;
					accel_y.frac_part = int_buf % accel_scale;


					int_buf = x_data[2][counter] * accel_scale; //scale value
					if(int_buf < 0)
					{
						int_buf = int_buf * -1;
						accel_z.symbol = '-';
						flagz = -1;
					}
					else 
					{
						accel_z.symbol = '+';
						flagz = 1;
					}
					accel_z.int_part = int_buf / accel_scale;
					accel_z.frac_part = int_buf % accel_scale;
					switch(key_data) {
						case '1':
							sprintf(string_buf, "%d,Walking,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
						case '2':
							sprintf(string_buf, "%d,Jogging,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
						case '3':
							sprintf(string_buf, "%d,Upstairs,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
						case '4':
							sprintf(string_buf, "%d,Downstairs,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
						case '5':
							sprintf(string_buf, "%d,Sitting,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
						case '6':
							sprintf(string_buf, "%d,Standing,%d000000,%c%d.%3d,%c%d.%3d,%c%d.%3d;\n", 
								user_number, msec_cnt,
								accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
								accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
								accel_z.symbol, accel_z.int_part, accel_z.frac_part);
							break;
					}
					delay_ms(125);
					hx_drv_uart_print(string_buf);
					msec_cnt = msec_cnt + 125;
					if(key_data == '9') break;
				} 
				msec_cnt = 0;
				key_data = 0;
				user_number = 0;
				break;
			}
		}
	}


}

