/*
    Shift only from 1 - 25
    option : 1. Encrypt
             2. Decrypt

    How to use :
    mpiexec -n <jumlah prosesor> <nama aplikasi>.exe "<text>" <shift> <option>

    Example:
    mpiexec -n 4 Caesar_Cipher.exe "Aaron Effendi" 3 2

    Referensi:
    1. https://github.com/nimish-agrawal/Encryption-Decryption-using-Parallel-Computing/blob/master/MPI/decrypt.cpp
    2. https://github.com/nimish-agrawal/Encryption-Decryption-using-Parallel-Computing/blob/master/MPI/encrypt.cpp
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>

#define id rank
#define TAG 0

char encrypt(char text, int k)
{
	if(text > 96 && text < 123){
        text = text + k;
        if(text > 122){
            text = text - 26;
        }
    }
    else if(text > 64 && text < 91){
        text = text + k;
        if(text > 90){
            text = text - 26;
        }
    }
    return text;
}

char decrypt(char text, int k)
{
	if(text > 96 && text < 123){
        text = text - k;
        if(text < 97){
            text = text + 26;
        }
    }
    else if(text > 64 && text < 91){
        text = text - k;
        if(text < 65){
            text = text + 26;
        }
    }
    return text;
}

int main(int argc, char *argv[])
{
	char text[255], a, b, hasil[256], decode[256];
	int n, shift, option, rank, size;
	int starting_point, stopping_point, starting_point1, stopping_point1;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	strcpy(text, argv[1]); // for input the text in CMD
	shift = atoi(argv[2]); // for input the shift in CMD
	option = atoi(argv[3]); // for input the option for encrypt or decrypt in CMD

    n = strlen(text);
    fflush(stdin);
    if(argc > 4){
        if (rank == 0) printf("Please, the right amount of data!\n" );
		MPI_Finalize(); exit(1);
    }
    if (shift < 1 || shift >25) {
        if(rank == 0){
            printf("Please, input shift between 1 - 25!");
        }
		MPI_Finalize(); exit(1);
    }

    if(rank == 0){
        printf("--------- Welcome to Caesar Cipher --------- \n\n");
        printf("The text is: %s\n", text);
    }
    if(size == 1){
        if(rank == 0){
            if(option == 1){
                printf("Encrypting...!\n");
                for(int i = 0; i < n; i++){
                    b = text[i];
                    a = encrypt(b, shift);
                    hasil[i] = a;
                }
                hasil[n] = '\0';
                printf("Encrypted text: %s \n", hasil);
                fflush(stdin);
            }
            else if(option == 2){
                printf("Decrypting...!\n");
                for(int i = 0; i < n; i++){
                    b = text[i];
                    a = decrypt(b, shift);
                    hasil[i] = a;
                }
                hasil[n] = '\0';
                printf("Decrypted text: %s \n", hasil);
                fflush(stdin);
            }
        }
    }
    else {
        if (rank == 0)
        {
            for (int i = 0; i < n; i++)
            {
                MPI_Send(&text[i], 1, MPI_CHAR, (i % (size-1)) + 1, TAG, MPI_COMM_WORLD);
            }
        }
        else
        {
            if (id <= n%(size-1))
            {
                for (int i = 0; i < (n / (size-1) + 1); i++)
                {
                    MPI_Recv(&b, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if(option == 1) {
                        a = encrypt(b, shift);
                    }
                    else if(option == 2){
                        a = decrypt(b, shift);
                    }
                    MPI_Send(&a, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
                }
            }
            else
            {
                for (int i = 0; i < n / (size-1); i++)
                {
                    MPI_Recv(&b, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if(option == 1) {
                        a = encrypt(b, shift);
                    }
                    else if(option == 2){
                        a = decrypt(b, shift);
                    }
                    MPI_Send(&a, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
                }
            }
        }
        if (rank == 0){
            for (int i = 0; i < n; i++)
            {
                MPI_Recv(&a, 1, MPI_CHAR, (i % (size-1)) + 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                hasil[i] = a;
            }
            hasil[n] = '\0';
        }
        if (rank == 0)
        {
            if(option == 1){
                printf("Encrypting...!\n");
                printf("Encrypted text: %s \n", hasil);
                fflush(stdin);
            }else if(option == 2){
                printf("Decrypting...!\n");
                printf("Decrypted text: %s \n", hasil);
                fflush(stdin);
            }
        }
    }
    if (rank == 0)
    {
        printf("\n--------------------------------------------\n");
        fflush(stdin);
    }
    MPI_Finalize();

	return 0;
}
