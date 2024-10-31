#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int matrix1[4][4];
int mat1of2;
int matrix2[4][4];
int mat2of2;

void setNumber() {
	mat1of2 = 0;
	mat2of2 = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (mat1of2 < 2) {
				matrix1[j][i] = rand() % 3;
				if (matrix1[j][i] == 2) {
					mat1of2++;
				}
			}
			else {
				matrix1[j][i] = rand() % 2;
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (mat2of2 < 2) {
				matrix2[j][i] = rand() % 3;
				if (matrix2[j][i] == 2) {
					mat2of2++;
				}
			}
			else {
				matrix2[j][i] = rand() % 2;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", matrix1[j][i]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", matrix2[j][i]);
		}
		printf("\n");
	}
}

void printmatrix(int holejjack) {
	if (holejjack == 0) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				printf("%d ", matrix1[j][i]);
			}
			printf("\n");
		}
		printf("\n");
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				printf("%d ", matrix2[j][i]);
			}
			printf("\n");
		}
	}
	else if (holejjack == 1) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (matrix1[j][i] % 2 != 0) {
					printf("%d ", matrix1[j][i]);
				}
				else {
					printf("  ");
				}
			}
			printf("\n");
		}
		printf("\n");
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (matrix2[j][i] % 2 != 0) {
					printf("%d ", matrix2[j][i]);
				}
				else {
					printf("  ");
				}
			}
			printf("\n");
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (matrix1[j][i] % 2 == 0) {
					printf("%d ", matrix1[j][i]);
				}
				else {
					printf("  ");
				}
			}
			printf("\n");
		}
		printf("\n");
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (matrix2[j][i] % 2 == 0) {
					printf("%d ", matrix2[j][i]);
				}
				else {
					printf("  ");
				}
			}
			printf("\n");
		}
	}
}

void mulmatrix() {
	int result[4][4] = { 0 };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[j][i] = 0;
			for (int k = 0; k < 4; k++) {
				result[j][i] += matrix1[k][j] * matrix2[i][k];
			}
		}
	}
	// 결과 행렬 출력
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", result[i][j]);
		}
		printf("\n");
	}
}

void plusmatrix() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", matrix1[j][i] + matrix2[j][i]);
		}
		printf("\n");
	}
}

void minusmatrix() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%2d ", matrix1[j][i] - matrix2[j][i]);
		}
		printf("\n");
	}
}

int determinant(int matrix[4][4], int n) {
	int det = 0;
	int submatrix[4][4];

	if (n == 2)
		return (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]);

	for (int x = 0; x < n; x++) {
		int subi = 0;
		for (int i = 1; i < n; i++) {
			int subj = 0;
			for (int j = 0; j < n; j++) {
				if (j == x)
					continue;
				submatrix[subi][subj] = matrix[i][j];
				subj++;
			}
			subi++;
		}
		det = det + (x % 2 == 0 ? 1 : -1) * matrix[0][x] * determinant(submatrix, n - 1);
	}
	return det;
}

void transpose(int matrix[4][4], int transposed[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			transposed[j][i] = matrix[i][j];
		}
	}
}

void mulNumber(int num) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", matrix1[j][i] * num);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", matrix2[j][i] * num);
		}
		printf("\n");
	}
}
int main() {
	srand((unsigned int)time(NULL));
	setNumber();

	char input;
	int holejjack = 2;
	int det1;
	int det2;
	int transposed1[4][4];
	int transposed2[4][4];

	while (1) {
		scanf("%c", &input);

		switch (input)
		{
		case 'm':
			mulmatrix();
			break;
		case 'a':
			plusmatrix();
			break;
		case 'd':
			minusmatrix();
			break;
		case 'r':
			det1 = determinant(matrix1, 4);
			det2 = determinant(matrix2, 4);
			printf("matrix1의 행렬식: %d\n", det1);
			printf("matrix2의 행렬식: %d\n", det2);
			break;
		case 't':
			transpose(matrix1, transposed1);
			transpose(matrix2, transposed2);
			int det1 = determinant(transposed1, 4);
			int det2 = determinant(transposed2, 4);

			printf("matrix1의 전치 행렬:\n");
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					printf("%d ", transposed1[j][i]);
				}
				printf("\n");
			}
			printf("matrix1 전치 행렬의 행렬식: %d\n\n", det1);

			printf("matrix2의 전치 행렬:\n");
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					printf("%d ", transposed2[j][i]);
				}
				printf("\n");
			}
			printf("matrix2 전치 행렬의 행렬식: %d\n", det2);
			break;
		case 'e':
			printmatrix(holejjack);
			holejjack = (holejjack + 1) % 2;
			break;
		case 's':
			setNumber();
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			mulNumber(input-'0');
			break;
		case 'q':
			return 0;
			break;
		}
	}
	return 0;
}