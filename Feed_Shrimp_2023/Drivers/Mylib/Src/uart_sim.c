#include "uart_sim.h"

uint8_t check10Times=0;
uint8_t count10Times=0;

int8_t Sim_SendCommand(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3,char* command,char* response)
{
	uint8_t answer = 0;
	Transmit_Data_Uart3(*rx_uart3->huart,command);
	Display_Uart1(*rx_uart1->huart,command);
	while(Check_Rx_Complete(rx_uart3)==0)
	{}
		
	if(strstr(rx_uart3->sim_rx,response) != NULL) 
	{
		answer = 1;
	}
	else 
	{
		answer = 0;
	}
	Display_Uart1(*rx_uart1->huart,rx_uart3->sim_rx);
	Delete_Buffer(rx_uart3);
	return answer;
}

int8_t Compare_Uart1_RX_Uart3_TX(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3,char* response)
{
	uint8_t answer = 0;
	if(rx_uart1->sim_rx[1]!=NULL)
	{
		while(Check_Rx_Complete(rx_uart1)==0)
		{}
			
		Display_Uart1(*rx_uart1->huart,rx_uart1->sim_rx);	
		HAL_UART_Transmit(rx_uart3->huart, (uint8_t*)rx_uart1->sim_rx, (uint8_t)strlen(rx_uart1->sim_rx), 1000);

		while(Check_Rx_Complete(rx_uart3)==0)
		{}
			
		Display_Uart1(*rx_uart1->huart,rx_uart3->sim_rx);
		if(strstr(rx_uart3->sim_rx,response) != NULL) 
		{
			answer = 1;
		}
		else 
		{
			answer = 0;
		}
		Delete_Buffer(rx_uart1);
		Delete_Buffer(rx_uart3);
	}
	return answer;
}

int8_t Uart1_To_Uart3(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3)
{
	uint8_t answer = 0;
	if(Check_Rx_Complete(rx_uart1)==1)
	{
		Display_Uart1(*rx_uart1->huart,rx_uart1->sim_rx);	
		HAL_UART_Transmit(rx_uart3->huart, (uint8_t*)rx_uart1->sim_rx, (uint8_t)strlen(rx_uart1->sim_rx), 1000);
		Delete_Buffer(rx_uart1);
	}
  if(Check_Rx_Complete(rx_uart3)==1)
	{
		Display_Uart1(*rx_uart1->huart,rx_uart3->sim_rx);
		Delete_Buffer(rx_uart3);
		answer=1;
	}
	return answer;
}

void Config_Uart_Sim(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3)
{
	while(Sim_SendCommand(rx_uart1,rx_uart3,"AT","OK")==0)
	{
		HAL_Delay(2000);
	}
	
	while(check10Times==0)
	{
	Check_Status_Config_Sim(rx_uart1,rx_uart3,"AT+CPIN?","OK");
	Check_Status_Config_Sim(rx_uart1,rx_uart3,"AT+CSQ","OK");
	Check_Status_Config_Sim(rx_uart1,rx_uart3,"AT+CGREG?","OK");
		
	if(check10Times==1)
	{
		Transmit_Data_Uart3(*rx_uart3->huart,"AT+CFUN=4");
		HAL_Delay(5000);
		Transmit_Data_Uart3(*rx_uart3->huart,"AT+CFUN=1");
		check10Times=0;
	}
	else
	{
	break;
	}
	}
}


void Check_Status_Config_Sim(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3, char* command,char* response)
{
		if(check10Times==0)
		{
		while(Sim_SendCommand(rx_uart1, rx_uart3, command, response)==0 )
		{
			HAL_Delay(500);
			count10Times++;
			if(count10Times>10)
			{
				count10Times=0;
				check10Times=1;
				break;
			}
		}
		count10Times=0;
		Delete_Buffer(rx_uart3);
		}	
}

void Display_Uart1(UART_HandleTypeDef huart,void* data)																								
{
	HAL_UART_Transmit(&huart,(uint8_t *)data,(uint16_t)strlen(data),1000);
	HAL_UART_Transmit(&huart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
}

void Delete_Buffer(UART_BUFFER *rx_uart)
{
	rx_uart->countBuffer=0;
	rx_uart->buffer=0x00;
	int len = strlen(rx_uart->sim_rx);
	for(int i = 0; i < len; i++)
	{
		rx_uart->sim_rx[i] = 0;
	}
}

void Transmit_Data_Uart3(UART_HandleTypeDef huart, char* command)
{
	HAL_UART_Transmit(&huart, (uint8_t *)command, (uint16_t)strlen(command), 1000);
	HAL_UART_Transmit(&huart,(uint8_t *)"\r",(uint16_t)strlen("\r"),1000);
}

void Setup_On_Off_Sim(GPIO_TypeDef* GPIO1, uint16_t GPIO_Pin_On_Off_Sim, 
                      GPIO_TypeDef* GPIO2, uint16_t GPIO_Pin_PWKEY,  
                      GPIO_TypeDef* GPIO3, uint16_t GPIO_Pin_RESET, uint32_t timeOut)
{
	if(timeOut<=2)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_On_Off_Sim,GPIO_PIN_RESET);
	}
	
	if(timeOut>2 && timeOut<=3)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_RESET,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_On_Off_Sim,GPIO_PIN_SET);
	}
	
	if(timeOut>3 && timeOut <=6)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_SET);
	}
	if(timeOut>6 && timeOut <=21)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_RESET);
	}
}

int8_t Check_Rx_Complete(UART_BUFFER *rx_uart)
{
	uint16_t answer=0;
	if(rx_uart->buffer==0x0A)
	{
		HAL_Delay(1);
		if(rx_uart->buffer==0x0A)
		{
			answer=1;
		}
	}
	return answer;
}

void Config_SMS_Receive(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3)
{
	while(Sim_SendCommand(rx_uart1, rx_uart3, "AT+CMGF=1","OK")==0)
	{
		HAL_Delay(1000);
	}
	
	while(Sim_SendCommand(rx_uart1, rx_uart3, "AT+CNMI=1,2","OK")==0)
	{
		HAL_Delay(1000);
	}
}

int8_t Wait_SMS_Receive(UART_BUFFER *rx_uart1, UART_BUFFER *rx_uart3,char* response)
{
	uint8_t answer = 0;
	if(Check_Rx_Complete(rx_uart3)==1)
	{
		if(strstr(rx_uart3->sim_rx,response) != NULL) 
		{
			answer = 1;
			
		}
		else 
		{
			answer = 0;
		}
		Display_Uart1(*rx_uart1->huart,rx_uart3->sim_rx);
		//Delete_Buffer(rx_uart3);
	}
	return answer;
}



