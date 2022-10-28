/************************************************************************
* 5 semestre - Eng. da Computao - Insper
*
* 2021 - Exemplo com HC05 com RTOS
*
*/

#include <asf.h>
#include "conf_board.h"
#include <string.h>

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// LEDs
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão
/*
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)
*/

//botao 1 do controle:
// Config do BUT1
#define BUT1_PIO PIOA
#define BUT1_ID ID_PIOA
#define BUT1_IDX 24
#define BUT1_IDX_MASK (1u << BUT1_IDX)

//botao 2 do controle:
// Config do BUT2
#define BUT2_PIO PIOC
#define BUT2_ID ID_PIOC
#define BUT2_IDX 19
#define BUT2_IDX_MASK (1u << BUT2_IDX)

//botao 3 do controle:
// Config do BUT3
#define BUT3_PIO PIOD
#define BUT3_ID ID_PIOD
#define BUT3_IDX 11
#define BUT3_IDX_MASK (1u << BUT3_IDX)

//botao 4 do controle:
// Config do BUT4
#define BUT4_PIO PIOA
#define BUT4_ID ID_PIOA
#define BUT4_IDX 6
#define BUT4_IDX_MASK (1u << BUT4_IDX)

//botao para handhshake
#define BUTH_PIO PIOA
#define BUTH_ID ID_PIOA
#define BUTH_IDX 2
#define BUTH_IDX_MASK (1u << BUTH_IDX)




//rumble
#define LED_PIO         PIOD  //periferico que controla o LED
#define LED_PIO_ID      ID_PIOD    //ID do periferico PIOC
#define LED_PIO_IDX     26	  //ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX) //Mascara para CONTROLARMOS o LED

//analogico

#define AFEC_Y AFEC0
#define AFEC_Y_ID ID_AFEC0
#define AFEC_Y_CHANNEL 0 // Canal do pino PD30


// usart (bluetooth ou serial)
// Descomente para enviar dados
// pela serial debug
//#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define USART_COM USART1
#define USART_COM_ID ID_USART1
#else
#define USART_COM USART0
#define USART_COM_ID ID_USART0
#endif

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/
//tasks 
#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_ADC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

//queue for the buttons
QueueHandle_t xQueueBut;
/** Queue for msg log send data */
QueueHandle_t xQueueADC;;


//timer para o afec
TimerHandle_t xTimer;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

struct ButStruct{
	char id;
	char status;
	char eop;
};

typedef struct {
	uint value;
} adcData;

/************************************************************************/
/* RTOS application HOOK                                                */
/************************************************************************/

/* Called if stack overflow during execution */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	* identify which task has overflowed its stack.
	*/
	for (;;) {
	}
}

/* This function is called by FreeRTOS idle task */
extern void vApplicationIdleHook(void) {
	pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/* This function is called by FreeRTOS each tick */
extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */

void but1_callback(void){
	struct ButStruct b1;
	b1.id = '1';
	b1.eop = 'X'; 
	
	if(pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK) == 0) {
		//rise edge
		b1.status = '1';//id 1 para bottao 1 apertado
		xQueueSendFromISR(xQueueBut, &b1, 0);
	}
	else{
		//fall edge 
		b1.status = '0'; //id 0 para bottao 1 solto
		xQueueSendFromISR(xQueueBut, &b1, 0);  	    
	}
}

void but2_callback(void){
	struct ButStruct b2;
	b2.id = '2';
	b2.eop = 'X'; 
	
	if(pio_get(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK) == 0) {
		//rise edge
		b2.status = '1'; //id 1 para bottao 1 apertado
		xQueueSendFromISR(xQueueBut, &b2, 0);
	}
	else{
		//fall edge
		b2.status = '0'; //id 0 para bottao 1 solto
		xQueueSendFromISR(xQueueBut, &b2, 0);
	}
}

void but3_callback(void){
	struct ButStruct b3;
	b3.id = '3';
	b3.eop = 'X'; 
	
	if(pio_get(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK) == 0) {
		//rise edge
		b3.status = '1'; //id 1 para bottao 1 apertado
		xQueueSendFromISR(xQueueBut, &b3, 0);
	}
	else{
		//fall edge
		b3.status = '0'; //id 0 para bottao 1 solto
		xQueueSendFromISR(xQueueBut, &b3, 0);
	}
}

void but4_callback(void){
	struct ButStruct b4;
	b4.id = '4';
	b4.eop = 'X';
	
	if(pio_get(BUT4_PIO, PIO_INPUT, BUT4_IDX_MASK) == 0) {
		//rise edge
		b4.status = '1';//id 1 para bottao 1 apertado
		xQueueSendFromISR(xQueueBut, &b4, 0);
	}
	else{
		//fall edge
		b4.status = '0'; //id 0 para bottao 1 solto
		xQueueSendFromISR(xQueueBut, &b4, 0);
	}
}


void buth_callback(void){
	struct ButStruct bh;
	bh.id = '5';
	bh.eop = 'X';
	
	if(pio_get(BUTH_PIO, PIO_INPUT, BUTH_IDX_MASK) == 0) {
		//rise edge
		bh.status = '1';//id 1 para bottao 1 apertado
		xQueueSendFromISR(xQueueBut, &bh, 0);
		printf("botao foi apertado");
	}
	else{
		//fall edge
		bh.status = '0'; //id 0 para bottao 1 solto
		xQueueSendFromISR(xQueueBut, &bh, 0);
	}
}

static void AFEC_pot_callback(void) {
	adcData adc;
	adc.value = afec_channel_get_value(AFEC_Y, AFEC_Y_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}



/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void io_init(void) {

	// Ativa PIOs
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
	//pmc_enable_periph_clk(BUT_PIO_ID);

	//botao 1
	// Configura Pinos
	pmc_enable_periph_clk(BUT1_ID);
	
	//pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	//debounce filter
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 60);
	pio_handler_set(BUT1_PIO,
	BUT1_ID,
	BUT1_IDX_MASK,
	PIO_IT_EDGE,
	but1_callback);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	NVIC_EnableIRQ(BUT1_ID);
	NVIC_SetPriority(BUT1_ID, 4); // Prioridade 4

	//botao 2
	pmc_enable_periph_clk(BUT2_ID);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 60);
	pio_handler_set(BUT2_PIO,
	BUT2_ID,
	BUT2_IDX_MASK,
	PIO_IT_EDGE,
	but2_callback);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);
	NVIC_EnableIRQ(BUT2_ID);
	NVIC_SetPriority(BUT2_ID, 4); // Prioridade 4

	//botao 3
	pmc_enable_periph_clk(BUT3_ID);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 60);
	pio_handler_set(BUT3_PIO,
	BUT3_ID,
	BUT3_IDX_MASK,
	PIO_IT_EDGE,
	but3_callback);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);
	NVIC_EnableIRQ(BUT3_ID);
	NVIC_SetPriority(BUT3_ID, 4); // Prioridade 4

	//bota4
	pmc_enable_periph_clk(BUT4_ID);
	pio_configure(BUT4_PIO, PIO_INPUT, BUT4_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT4_PIO, BUT4_IDX_MASK, 60);
	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT4_PIO,
	BUT4_ID,
	BUT4_IDX_MASK,
	PIO_IT_EDGE,
	but4_callback);
	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT4_PIO, BUT4_IDX_MASK);
	//status da interrupcao	
	pio_get_interrupt_status(BUT4_PIO);
	NVIC_EnableIRQ(BUT4_ID);
	NVIC_SetPriority(BUT4_ID, 4); // Prioridade 4
	
	//botah
	pmc_enable_periph_clk(BUTH_ID);
	pio_configure(BUTH_PIO, PIO_INPUT, BUTH_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUTH_PIO, BUTH_IDX_MASK, 60);
	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUTH_PIO,
	BUTH_ID,
	BUTH_IDX_MASK,
	PIO_IT_EDGE,
	buth_callback);
	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUTH_PIO, BUTH_IDX_MASK);
	//status da interrupcao
	pio_get_interrupt_status(BUTH_PIO);
	NVIC_EnableIRQ(BUTH_ID);
	NVIC_SetPriority(BUTH_ID, 4); // Prioridade 4
	
	//rumble
	//Ativa o PIO que o LED esta conectado
	pmc_enable_periph_clk(LED_PIO_ID);
	
	//Inicializacao PC8 como saidoa
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
		#endif
		.paritytype = CONF_UART_PARITY,
		#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
		#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	#if defined(__GNUC__)
	setbuf(stdout, NULL);
	#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	* emits one character at a time.
	*/
	#endif
}

uint32_t usart_puts(uint8_t *pstring) {
	uint32_t i ;

	while(*(pstring + i))
	if(uart_is_tx_empty(USART_COM))
	usart_serial_putchar(USART_COM, *(pstring+i++));
}

void usart_put_string(Usart *usart, char str[]) {
	usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
	uint timecounter = timeout_ms;
	uint32_t rx;
	uint32_t counter = 0;

	while( (timecounter > 0) && (counter < bufferlen - 1)) {
		if(usart_read(usart, &rx) == 0) {
			buffer[counter++] = rx;
		}
		else{
			timecounter--;
			vTaskDelay(1);
		}
	}
	buffer[counter] = 0x00;
	return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen,
char buffer_tx[], int timeout) {
	usart_put_string(usart, buffer_tx);
	usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void config_usart0(void) {
	sysclk_enable_peripheral_clock(ID_USART0);
	usart_serial_options_t config;
	config.baudrate = 9600;
	config.charlength = US_MR_CHRL_8_BIT;
	config.paritytype = US_MR_PAR_NO;
	config.stopbits = false;
	usart_serial_init(USART0, &config);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);

	// RX - PB0  TX - PB1
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_init(void) {
	char buffer_rx[128];
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEagoravai", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN0000", 100);
}

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback) {
	/*************************************
   * Ativa e configura AFEC
   *************************************/
  /* Ativa AFEC - 0 */
  afec_enable(afec);

  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /*
  * Calibracao:
  * Because the internal ADC offset is 0x200, it should cancel it and shift
  down to 0.
  */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);

  /***  Configura sensor de temperatura ***/
  struct afec_temp_sensor_config afec_temp_sensor_cfg;

  afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
  afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);

  /* configura IRQ */
  afec_set_callback(afec, afec_channel, callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

void task_bluetooth(void) {
	printf("Task Bluetooth started \n");
	
	printf("Inicializando HC05 \n");
	config_usart0();
	hc05_init();

	// configura LEDs e Botões
	io_init();

	struct ButStruct data;
	int handhsake = 0;
	// Task não deve retornar.
	while(1) {
		
		//handhsake
		if(xQueueReceiveFromISR( xQueueBut, &data, ( TickType_t ) 1) ){
		
			if(data.id == '5'){
			
				while(!usart_is_tx_ready(USART_COM)) {
					vTaskDelay(10 / portTICK_PERIOD_MS);
				}
			
				//mandando o status
				usart_write(USART_COM, data.id);
				printf("iniciando HandShake, com botao: %d", data.id);
			}
			
			while(!handhsake){
				
				while(!usart_is_rx_ready(USART_COM)) {
					vTaskDelay(10 / portTICK_PERIOD_MS);
				}
				
				char *c;
				usart_read(USART_COM, &c);
				if (c == 'A'){
					handhsake = 1;
				}
			}
		}
		
		if( (xQueueReceiveFromISR( xQueueBut, &data, ( TickType_t ) 1)) && handhsake ){
			// envia status botão
			
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			pio_set(LED_PIO, LED_PIO_IDX_MASK);
			//mandando o ID
			usart_write(USART_COM, data.id);
			printf("\n id: %c", data.id);
			
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			
			//mandando o status
			usart_write(USART_COM, data.status);
			printf("\n status: %c", data.status);
						
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			
			// envia fim de pacote
			
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, data.eop);
			

			// dorme por 500 ms
			vTaskDelay(5 / portTICK_PERIOD_MS);
			pio_clear(LED_PIO, LED_PIO_IDX_MASK);
		}
	}
}

void vTimerCallback(TimerHandle_t xTimer) {
	/* Selecina canal e inicializa conversão */
	afec_channel_enable(AFEC_Y, AFEC_Y_CHANNEL);
	afec_start_software_conversion(AFEC_Y);
}

static void task_adc(void *pvParameters) {

  // configura ADC e TC para controlar a leitura
  config_AFEC_pot(AFEC_Y, AFEC_Y_ID, AFEC_Y_CHANNEL, AFEC_pot_callback);

  xTimer = xTimerCreate(/* Just a text name, not used by the RTOS
                        kernel. */
                        "Timer",
                        /* The timer period in ticks, must be
                        greater than 0. */
                        100,
                        /* The timers will auto-reload themselves
                        when they expire. */
                        pdTRUE,
                        /* The ID is used to store a count of the
                        number of times the timer has expired, which
                        is initialised to 0. */
                        (void *)0,
                        /* Timer callback */
                        vTimerCallback);
  xTimerStart(xTimer, 0);
  
  while(1) {
	  
	  // variável para recever dados da fila
	  adcData adc;

	  if (xQueueReceiveFromISR(xQueueADC, &(adc), 1000)) {
		  //printf("ADC: %d \n", adc);
		  
		  if(adc.value < 1500){
			  		  
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			  		  
			//mandando 
			usart_write(USART_COM, 'D'); //D == down
			
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
						
			//mandando o status
			usart_write(USART_COM, 'D'); //D == down 
			  		  
			// envia fim de pacote
			  		  
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, 'X');
			  
		  }
		  
		  		  
		if(adc.value > 3000){
			  		  
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			  		  
			//mandando 
			usart_write(USART_COM,'U'); //D == down
			
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
						
			//mandando o status
			usart_write(USART_COM, 'U'); //D == down
			  		  
			// envia fim de pacote
			  		  
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, 'X');
			  		  
		}
		  
		// dorme por 5 ms
		vTaskDelay(5 / portTICK_PERIOD_MS);
	  }
  }
}
/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	configure_console();
	
	struct ButStruct jorge;
	
	// Create the queue for the buttons
	xQueueBut = xQueueCreate(32, sizeof(jorge) );
	
	// verifica se fila foi criada corretamente
	if (xQueueBut == NULL){
		printf("falha em criar a fila \n");
	}
	
	xQueueADC = xQueueCreate(100, sizeof(adcData));
	if (xQueueADC == NULL){
		printf("falha em criar a queue xQueueADC \n");
	}
	/* Create task to make led blink */
	xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL,	TASK_BLUETOOTH_STACK_PRIORITY, NULL);
	//task adc
	xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, NULL);
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}