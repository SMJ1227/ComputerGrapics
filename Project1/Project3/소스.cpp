#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

typedef struct Point {
	int ava;
	int x;
	int y;
	int z;
};

struct PointDistance {
	double distance;
	int index;
};

int compare(const void* a, const void* b) {
	return (*(PointDistance*)a).distance > (*(PointDistance*)b).distance ? 1 : -1;
}

int main() {
	Point point[20];
	Point copyA[20];
	Point copyS[20];
	int length = 0;
	int emptyIndex = -1;

	for (int i = 0; i < 20; i++) {
		point[i].ava = 0;
	}

	for (int i = 19; i >= 0; i--) {
		if (point[i].ava == 0) {
			printf("%2d: NULL\n", i);
		}
	}

	bool nonull = true;
	char input;
	int numofNull = 0;
	int movenum = 0;
	bool usedA = false;
	bool usedS = false;

	while (1) {
		printf("-------------------------------------\n");
		for (int i = 19; i >= 0; i--) {
			if (point[i].ava == 0) {
				printf("%2d: NULL\n", i);
			}
			else {
				printf("%2d: %d, %d, %d)\n", i, point[i].x, point[i].y, point[i].z);
			}
		}
		scanf(" %c", &input);
		switch (input)
		{
		case '+':
		case '=':
			if (length > 19) {
				printf("�� ���� �Է� �Ұ����մϴ�.\n");
			}
			else {
				printf("x,y,z ��ǥ�� �Է��ϼ���: ");
				scanf("%d %d %d", &point[length].x, &point[length].y, &point[length].z);
				point[length].ava = 1;
				length++;
			}
			break;
		case '-':
		case '_':
			if (length == 0) break;
			point[length - 1].ava = 0;
			length--;
			break;
		case 'e':
			// �� �ε��� ã��
			emptyIndex = -1;
			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) {
					emptyIndex = i;
					break;
				}
			}

			// �� �ε����� �ִ� ���
			if (emptyIndex != -1) {
				// �� �ε������� 0������ �����͸� ���� �̵���Ų��
				for (int i = emptyIndex; i > 0; i--) {
					point[i] = point[i - 1];
				}

				// 0�� �ε����� �� ���� �����Ѵ�
				printf("x,y,z ��ǥ�� �Է��ϼ���: ");
				scanf("%d %d %d", &point[0].x, &point[0].y, &point[0].z);
				point[0].ava = 1;
			}
			else {
				// �� �ε����� ���� ���, ������ �ڵ��� ó��
				// ���� �����͸� ���� �̵���Ų��
				for (int i = length - 1; i >= 0; i--) {
					if (i + 1 < 20) { // �迭�� ������ ���� �ʵ��� üũ
						point[i + 1] = point[i];
					}
				}

				// 0�� �ε����� �� ���� �����Ѵ�
				printf("x,y,z ��ǥ�� �Է��ϼ���: ");
				scanf("%d %d %d", &point[0].x, &point[0].y, &point[0].z);
				point[0].ava = 1;

				// ���̸� ������Ų�� (�ִ� ���� üũ)
				if (length < 20) {
					length++;
				}
			}
			break;


		/*case 'e':
			printf("x,y,z ��ǥ�� �Է��ϼ���: ");
			for (int i = length; i > 0; i--) {
				if (point[0].ava == 0) { 
					nonull = false; 
					break; 
				}
				else if (point[i - 1].ava == 0) {
					for (int j = i - 1; j > 0; j--) {
						point[j].x = point[j - 1].x;
						point[j].y = point[j - 1].y;
						point[j].z = point[j - 1].z;
						point[j].ava = point[j - 1].ava;
					}
					nonull = false;
					break;
				}
				else {
					point[i].x = point[i - 1].x;
					point[i].y = point[i - 1].y;
					point[i].z = point[i - 1].z;
				} 
			}
			scanf("%d %d %d", &point[0].x, &point[0].y, &point[0].z);
			if (nonull == true) {
				point[length].ava = 1;
				if(length<19)length++;
			}
			else {
				point[0].ava = 1;
				nonull = true;
			}
			break;*/
		case 'd':	// �� ���������� length0�ǵ��� �߰��ؾ���
			if (length == 0) break;
			for (int i = 0; i < length; i++) {
				if (point[i].ava == 1) {
					point[i].ava = 0;
					break;
				}
			}
			break;
		case 'l':
			numofNull = 0;
			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) { numofNull++; }
			}
			printf("���� ����: %d\n", length - numofNull);
			break;
		case 'c':
			for (int i = 0; i < length; i++) {
				point[i].ava = 0;
			}
			length = 0;
			break;
		case 'm':
		{
			numofNull = 0;
			int farthestIndex = 0;
			double maxDistance = 0;

			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) {
					numofNull++;
				}
				else {
					double distance = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);
					if (distance > maxDistance) {
						maxDistance = distance;
						farthestIndex = i;
					}
				}
			}

			if (length == numofNull) {
				printf("���� �����ϴ�.");
				break;
			}

			printf("���� �ָ� �ִ� ��: (%d, %d, %d)\n", point[farthestIndex].x, point[farthestIndex].y, point[farthestIndex].z);
			break;
		}
		case 'n':
		{
			numofNull = 0;
			int closestIndex = 0;
			double minDistance = DBL_MAX;

			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) {
					numofNull++;
				}
				else {
					double distance = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);

					// ���� ����� �� ã��
					if (distance < minDistance) {
						minDistance = distance;
						closestIndex = i;
					}
				}
			}

			if (length == numofNull) {
				printf("���� �����ϴ�.");
				break;
			}

			printf("���� ����� ��: (%d, %d, %d)\n", point[closestIndex].x, point[closestIndex].y, point[closestIndex].z);
			break;
		}
		case 'a':
		{
			numofNull = 0;

			// ���� �޸� �Ҵ�
			PointDistance* distances = (PointDistance*)malloc(length * sizeof(PointDistance));
			if (distances == NULL) {
				printf("�޸� �Ҵ� ����\n");
				break;
			}

			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) {
					numofNull++;
				}
				else {
					distances[i].distance = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);
					distances[i].index = i;
				}
			}

			if (length == numofNull) {
				printf("���� �����ϴ�.");
				free(distances); // �޸� ����
				break;
			}

			// �Ÿ� �������� ����
			qsort(distances, length, sizeof(PointDistance), compare);

			// ���ĵ� �� ���
			printf("�������� �Ÿ� ������������ ���ĵ� ����:\n");
			for (int i = 0; i < length; i++) {
				if (point[distances[i].index].ava != 0) {
					printf("�� %d: (%d, %d, %d) �Ÿ�: %.2f\n",
						i,
						point[distances[i].index].x,
						point[distances[i].index].y,
						point[distances[i].index].z,
						distances[i].distance);
				}
			}

			// �޸� ����
			free(distances);
			break;
		}
		case 's':
			if (usedS == false) {
				for (int i = 0; i < 20; i++) {
					copyS[i] = point[i];
				}
				usedA = false;
				usedS = true;
				movenum = 0;
				for (int i = 0; i < length; i++) {
					if (point[i].ava == 0 && point[i + 1].ava == 1) {
						point[i].x = point[i + 1].x;
						point[i].y = point[i + 1].y;
						point[i].z = point[i + 1].z;
						point[i].ava = 1;
						point[i + 1].ava = 0;
						movenum++;
					}
				}
				length = length - movenum;

				// �������� ����
				for (int i = 0; i < length - 1; i++) {
					for (int j = 0; j < length - i - 1; j++) {
						// x, y, z ��ǥ �������� �������� ���Ͽ� ����
						if (point[j].x < point[j + 1].x ||
							(point[j].x == point[j + 1].x && point[j].y < point[j + 1].y) ||
							(point[j].x == point[j + 1].x && point[j].y == point[j + 1].y && point[j].z < point[j + 1].z)) {
							// �� ��ȯ
							Point temp = point[j];
							point[j] = point[j + 1];
							point[j + 1] = temp;
						}
					}
				}
			}
			else if (usedS == true) {
				for (int i = 0; i < 20; i++) {
					point[i] = copyS[i];
				}
				usedS = false;
			}
			break;
		case 'q':
			return 0;
		}
	}
	return 0;
}