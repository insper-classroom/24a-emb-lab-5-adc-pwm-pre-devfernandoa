#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"

#define MOVING_AVERAGE_SIZE 5
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p){
    int data = 0;
    int moving_average_data[MOVING_AVERAGE_SIZE] = {0};
    int sum = 0;
    int count = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            // Remover o dado mais antigo do total
            sum -= moving_average_data[count % MOVING_AVERAGE_SIZE];
            // Adicionar o novo dado ao array
            moving_average_data[count % MOVING_AVERAGE_SIZE] = data;
            // Adicionar o novo dado ao total
            sum += data;
            // Calcular a média móvel
            int moving_average = sum / MOVING_AVERAGE_SIZE;
            // Imprimir o dado filtrado na UART
            printf("Dado filtrado: %d\n", moving_average);
            // Incrementar o contador
            count++;
            // Deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
