#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate_random_amount_of_hours() { return (rand() % 16) + 1; }

// Gets the price of the parking slot according to the number of hours it was
// used.
float price_by_time(int hours) { return hours * 10.0; }

// Generates a random floating point number in [0-1]
// float randf() { return (float)rand() / (float)(RAND_MAX); }

int main() {
  int available_slots = 15;

  const int MAX_SLOTS = 15;
  const int VEHICLES_DURING_DAY = 20;
  const uint SEED = 696969;
  srand(SEED);

  int total_hours_stayed = 0;
  float earnings = 0;
  printf("========== PARKING SIMULATION ==========\n");
  printf("Using the following parameters:\n* MAX SLOTS: %d\n* SEED: %d\n* "
         "VEHICLES DURING DAY: %d\n",
         MAX_SLOTS, SEED, VEHICLES_DURING_DAY);

#pragma omp parallel sections shared(available_slots)
  {
#pragma omp section
    {
      for (int id_vehicle = 0; id_vehicle < VEHICLES_DURING_DAY; id_vehicle++) {
        while (available_slots <= 0) {
          struct timespec time = {.tv_nsec = 0};
          nanosleep(&time, NULL);
        }

        printf("Letting in car with ID: %d\n", id_vehicle);

#pragma omp atomic
        available_slots--;
      }
    }

#pragma omp section
    {
      for (int id_vehicle = 0; id_vehicle < VEHICLES_DURING_DAY; id_vehicle++) {
        while (available_slots >= MAX_SLOTS) {
          struct timespec time = {.tv_nsec = 0};
          nanosleep(&time, NULL);
        }

        int hours_stayed = generate_random_amount_of_hours();
        printf("Letting go of car with ID: %d, it stayed %d hours\n",
               id_vehicle, hours_stayed);
        float price_to_pay = price_by_time(hours_stayed);
#pragma omp atomic
        earnings += price_to_pay;
#pragma omp atomic
        total_hours_stayed += hours_stayed;
#pragma omp atomic
        available_slots++;
      }
    }
  }

  printf("=== DONE ===\n"
         "* HOURS WORKED: %d\n"
         "* EARNINGS: %f\n"
         "* TOTAL VEHICLES: %d\n",
         total_hours_stayed, earnings, VEHICLES_DURING_DAY);
}
