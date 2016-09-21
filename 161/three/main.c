#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char last[30];
	char first[35];
	float salary;
	int hired_yr;
} employee;

int main(int argc, char** argv){
	employee database[20];

	// Initialization	
	int i = 0;
	while(i < 20){
		database[i].last[0] = 0;
		database[i].first[0] = 0;
		database[i].salary = 0;
		database[i++].hired_yr = 0;
	}

/*
	// Obligatory debug prints
	i = 0;
	while(i < 20){
		printf("%d\nEmployee Name: %s %s\nSalary: %f\nYear Hired: %d\n", i, database[i].first, database[i].last, database[i].salary, database[i].hired_yr);
		i++;
	}
*/

	// Prompt
	printf("NoSQL BrianDB Welcomes you!\n");
	printf("Enter an option:\n");

	// Loop!
	int exit = 0;
	while(!exit){

		printf("[D]isplay record\n");
		printf("[M]odify record\n");
		printf("[S]ave records to 'company.db'\n");
		printf("[L]oad records from 'company.db'\n");
		printf("[E]xit\n");

		char in = 0;
		scanf(" %c", &in);
		getchar();

		while(in != 'D' && in != 'M' && in != 'S' && in != 'L' && in != 'E'){
			printf("'%c' is an invalid input!\n", in);
			scanf(" %c", &in);
			getchar();
		}

		if(in == 'D'){
			int n = -1;
			printf("You have chosen to display an entry.\n");
			printf("Please enter the entry number you wish to display: ");
			scanf(" %d", &n); getchar();
		
			while(n < 1 || n > 20){
				printf("The valid number range is 1 to 20, inclusive\n");
				printf("Please enter the entry number you wish to display: ");
				scanf(" %d", &n); getchar();
			}

			// Print data
			printf("-----------------------------------------------------\n");
			printf("Entry number %d\n", n);
			printf("Full Name: %s %s\n", database[n-1].first, database[n-1].last);
			printf("Salary: $%.2f\n", database[n-1].salary);
			printf("Year Hired: %d\n", database[n-1].hired_yr);
			printf("-----------------------------------------------------\n");
		}else if(in == 'M'){
			int n = -1;
			printf("You have chosen to modify an entry.\n");
			printf("Please enter the entry number you wish to modify: ");
			scanf(" %d", &n); getchar();
		
			while(n < 1 || n > 20){
				printf("The valid number range is 1 to 20, inclusive\n");
				printf("Please enter the entry number you wish to modify: ");
				scanf(" %d", &n); getchar();
			}

			// Write data
			printf("Enter first name: ");
			scanf("%34s", database[n-1].first); getchar();
			
			printf("Enter last name: ");
			scanf("%29s", database[n-1].last); getchar();
			
			printf("Enter salary: ");
			scanf("%f", &database[n-1].salary); getchar();

			printf("Enter year hired: ");
			scanf("%d", &database[n-1].hired_yr); getchar();

			// Print data
			printf("-----------------------------------------------------\n");
			printf("Entry number %d\n", n);
			printf("Full Name: %s %s\n", database[n-1].first, database[n-1].last);
			printf("Salary: $%.2f\n", database[n-1].salary);
			printf("Year Hired: %d\n", database[n-1].hired_yr);
			printf("-----------------------------------------------------\n");
		}else if(in == 'S'){
			// Open/create file and save as binary data (as it is stored in memory)
			FILE *f;
			f = fopen("company.db", "wb+");
			int w = fwrite(database, sizeof(employee), 20, f);
			printf("%d bytes written\n");
			fclose(f);
			printf("Database saved to file 'company.db' successfully!\n");
		}else if(in == 'L'){
			// Open file and load as binary data
			FILE *f;
			f = fopen("company.db", "r");
			if(f == NULL)
				printf("File does not exist!\n");
			int r = fread(database, sizeof(employee), 20, f);
			printf("%d bytes read\n");
			fclose(f);
			printf("Database loaded from file 'company.db' successfully!\n");
		}else{
			exit = 1;
		}
	}
	// "User-Friendly" Program :)
	printf("Thank you for using BrianDB!\n");
}
