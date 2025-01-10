#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void DisplayMenu() {
  printf("Select an option\n");
  printf("[1] Sleep in 30 minutes\n");
  printf("[2] Sleep in 45 minutes\n");
  printf("[3] Sleep in 1h minutes\n");
  printf("[4] Sleep at custom time\n");
  printf("Enter your choice: ");
}

int CalculateSecondsUntil(int targetHour, int targetMinute) {
  time_t currentTime = time(NULL);
  struct tm *localTime = localtime(&currentTime);

  int currentHour = localTime->tm_hour;
  int currentMinute = localTime->tm_min;
  int currentSecond = localTime->tm_sec;

  int targetSeconds = targetHour * 3600 + targetMinute * 60;
  int currentSeconds = currentHour * 3600 + currentMinute * 60 + currentSecond;

  int secondsUntil = targetSeconds - currentSeconds;

  if (secondsUntil < 0) {
    // Target time is in the next day
    secondsUntil += 24 * 3600;
  }

  return secondsUntil;
}

int main() {
  int choice;
  int duration = 0;

  while (1) {
    DisplayMenu();
    if (scanf("%d", &choice) != 1) {
      fprintf(stderr, "Invalid input. Exiting...\n");
      exit(EXIT_FAILURE);
    }

    switch (choice) {
    case 1: {
      duration = 30 * 60;
      break;
    }
    case 2: {
      duration = 45 * 60;
      break;
    }
    case 3: {
      duration = 60 * 60;
      break;
    }
    case 4: {
      int targetHour, targetMinute;
      printf("Enter target time (HH:MM): ");
      if (scanf("%d:%d", &targetHour, &targetMinute) != 2 || targetHour < 0 ||
          targetHour >= 24 || targetMinute < 0 || targetMinute >= 60) {
        fprintf(stderr, "Invalid time format. Exiting...\n");
        exit(EXIT_FAILURE);
      }

      duration = CalculateSecondsUntil(targetHour, targetMinute);
      break;
    }
    default:
      fprintf(stderr, "Invalid choice. Please try again.\n\n");
      exit(EXIT_FAILURE);
      break;
    }

    pid_t pid = fork();
    if (pid == 0) {
      // Child process handles sleep and suspend
      sleep(duration);
      printf("Putting the system to sleep now.\n");
      execlp("systemctl", "systemctl", "suspend", (char *)NULL);
      fprintf(stderr, "Failed to execute suspend command\n");
      exit(EXIT_FAILURE);
    } else if (pid > 0) {
      // Parent process continues and exits
      printf("Waiting for %d seconds before scheduling system sleep...\n",
             duration);
    } else {
      fprintf(stderr, "Failed to fork process.\n");
      exit(EXIT_FAILURE);
    }
    break;
  }

  return EXIT_SUCCESS;
}
