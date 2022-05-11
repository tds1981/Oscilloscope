#include "ADC.h"

void InitTimer()
{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; 
		TIM1->SMCR &= ~TIM_SMCR_SMS;  // внутреннее тактирование, шина APB
		TIM1->PSC=9;
		TIM1->ARR=72;
		TIM1->CCR1=TIM1->ARR;
		TIM1->CR2 |= TIM_CR2_MMS_2;
		
		TIM1->CCMR1 |= TIM_CCMR1_OC1M;                // Ch1: PWM mode2
		TIM1->CCER |= TIM_CCER_CC1E;                  //<! Ch1 out enable
		TIM1->BDTR |= TIM_BDTR_MOE;                    //<! main out enable (!!!)
	
		TIM1->CR1 = TIM_CR1_CEN;
}

//----------------------------------------------------------

void InitADC()
{
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		ADC1->CR2 |= ADC_CR2_CAL; 
		while (ADC1->CR2&0x04 != 0){}
		
		ADC1->CR2 &= ~ADC_CR2_ADON;		// выкл ацп
	
		ADC1->CR1 &= ~ADC_CR1_SCAN; 			// запрет режима сканирования
		ADC1->CR2 &= ~ADC_CR2_CONT;				// запрет непрерывного режима
		//ADC1->CR2 |= ADC_CR2_CONT;	 		// разрешение непрерывного режима
		
		ADC1->CR2 |= ADC_CR2_EXTTRIG;  	// разрешение запуска ацп от внешнего события
		//ADC1->CR2 |= ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2; // SWSTART
		ADC1->CR2 &= ~(ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // TIM1_CC1	
		ADC1->CR2 |= ADC_CR2_ALIGN;
		ADC1->CR2 |= ADC_CR2_DMA;	
		
		ADC1->SQR1 =0; 
		ADC1->SQR3 =1; // канал А1
		
		ADC1->SMPR1 |= 0x08; //001: 7.5 cycles
			
		ADC1->CR2 |= ADC_CR2_ADON;
			
			
	//	ADC1->CR2 |= ADC_CR2_SWSTART;			
}	

#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)

#define DMA_BUFF_SIZE 2048
int16_t ADC_buff[DMA_BUFF_SIZE];

//-------------------------------------------------
void InitDMA()
{
		RCC->AHBENR |= RCC_AHBENR_DMA1EN;					//Разрешаем тактирование первого DMA модуля
		DMA1_Channel1->CPAR =  ADC1_DR_Address;		//Указываем адрес периферии - регистр результата преобразования АЦП для регулярных каналов
		DMA1_Channel1->CMAR = (uint32_t)ADC_buff;	//Задаем адрес памяти - базовый адрес массива в RAM
		DMA1_Channel1->CCR &= ~DMA_CCR_DIR; 		// Указываем направление передачи данных, из периферии в память
		DMA1_Channel1->CNDTR = DMA_BUFF_SIZE; 	// Количество пересылаемых значений
		
		DMA1_Channel1->CCR &= ~DMA_CCR_PINC; 		//Адрес периферии не инкрементируем после каждой пересылки
		DMA1_Channel1->CCR |= DMA_CCR_MINC; 		//Адрес памяти инкрементируем после каждой пересылки.
		DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; //Размерность данных периферии - 16 бит
		DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; //Размерность данных памяти - 16 бит
		DMA1_Channel1->CCR |= DMA_CCR_PL;		 //Приоритет - очень высокий 
		DMA1_Channel1->CCR |= DMA_CCR_CIRC; 		//Разрешаем работу DMA в циклическом режиме
		
		DMA1_Channel1->CCR |= DMA_CCR_HTIE; 	// Разрешение прерывания при достижении половины буфера
		DMA1_Channel1->CCR |= DMA_CCR_TCIE;
	
		NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	
		DMA1_Channel1->CCR |= DMA_CCR_EN; 			//Разрешаем работу 1-го канала DMA

}	

//----------------------------------------------------
//uint16_t CountAds=0;

uint8_t HalfFlag=1;

void DMA1_Channel1_IRQHandler(void)
{
		if ((DMA1->ISR&DMA_ISR_HTIF1)&&HalfFlag) 
		{	
		//	CountAds++;
		/*	for (uint16_t i=0; i<DMA_BUFF_SIZE/2; i++) 
			{
					ADC_buff[i]*=4;
					ADC_buff[i]=ADC_buff[i]-4095;
					uint8_t b=ADC_buff[i]>>8; 
					ADC_buff[i]=(ADC_buff[i]<<8)|b; 		
		//		ADC_buff[i]=CountAds;
			}		*/	
		CDC_Transmit_FS(ADC_buff, DMA_BUFF_SIZE);
		}
		
	  if (DMA1->ISR&DMA_ISR_TCIF1) // full bufer
		{	
		//	CountAds++;
		/*	for (uint16_t i=DMA_BUFF_SIZE/2; i<DMA_BUFF_SIZE; i++) 
			{
					ADC_buff[i]*=4;
					ADC_buff[i]=ADC_buff[i]-4095;
					uint8_t b=ADC_buff[i]>>8; 
					ADC_buff[i]=(ADC_buff[i]<<8)|b;
				
			//		ADC_buff[i]=CountAds;
			}		*/
			CDC_Transmit_FS(&ADC_buff[DMA_BUFF_SIZE/2], DMA_BUFF_SIZE);
}
		
		DMA1->IFCR |= 0xfffffff;//DMA_IFCR_CGIF1;
}


