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
				printf("맨 위는 입력 불가능합니다.\n");
			}
			else {
				printf("x,y,z 좌표를 입력하세요: ");
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
			// 빈 인덱스 찾기
			emptyIndex = -1;
			for (int i = 0; i < length; i++) {
				if (point[i].ava == 0) {
					emptyIndex = i;
					break;
				}
			}

			// 빈 인덱스가 있는 경우
			if (emptyIndex != -1) {
				// 빈 인덱스부터 0까지의 데이터만 위로 이동시킨다
				for (int i = emptyIndex; i > 0; i--) {
					point[i] = point[i - 1];
				}

				// 0번 인덱스에 새 점을 삽입한다
				printf("x,y,z 좌표를 입력하세요: ");
				scanf("%d %d %d", &point[0].x, &point[0].y, &point[0].z);
				point[0].ava = 1;
			}
			else {
				// 빈 인덱스가 없는 경우, 기존의 코드대로 처리
				// 기존 데이터를 위로 이동시킨다
				for (int i = length - 1; i >= 0; i--) {
					if (i + 1 < 20) { // 배열의 범위를 넘지 않도록 체크
						point[i + 1] = point[i];
					}
				}

				// 0번 인덱스에 새 점을 삽입한다
				printf("x,y,z 좌표를 입력하세요: ");
				scanf("%d %d %d", &point[0].x, &point[0].y, &point[0].z);
				point[0].ava = 1;

				// 길이를 증가시킨다 (최대 길이 체크)
				if (length < 20) {
					length++;
				}
			}
			break;


		/*case 'e':
			printf("x,y,z 좌표를 입력하세요: ");
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
		case 'd':	// 다 지워졋을때 length0되도록 추가해야함
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
			printf("점의 개수: %d\n", length - numofNull);
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
				printf("점이 없습니다.");
				break;
			}

			printf("가장 멀리 있는 점: (%d, %d, %d)\n", point[farthestIndex].x, point[farthestIndex].y, point[farthestIndex].z);
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

					// 가장 가까운 점 찾기
					if (distance < minDistance) {
						minDistance = distance;
						closestIndex = i;
					}
				}
			}

			if (length == numofNull) {
				printf("점이 없습니다.");
				break;
			}

			printf("가장 가까운 점: (%d, %d, %d)\n", point[closestIndex].x, point[closestIndex].y, point[closestIndex].z);
			break;
		}
		case 'a':
		{
			numofNull = 0;

			// 동적 메모리 할당
			PointDistance* distances = (PointDistance*)malloc(length * sizeof(PointDistance));
			if (distances == NULL) {
				printf("메모리 할당 실패\n");
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
				printf("점이 없습니다.");
				free(distances); // 메모리 해제
				break;
			}

			// 거리 기준으로 정렬
			qsort(distances, length, sizeof(PointDistance), compare);

			// 정렬된 점 출력
			printf("원점과의 거리 오름차순으로 정렬된 점들:\n");
			for (int i = 0; i < length; i++) {
				if (point[distances[i].index].ava != 0) {
					printf("점 %d: (%d, %d, %d) 거리: %.2f\n",
						i,
						point[distances[i].index].x,
						point[distances[i].index].y,
						point[distances[i].index].z,
						distances[i].distance);
				}
			}

			// 메모리 해제
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

				// 내림차순 정렬
				for (int i = 0; i < length - 1; i++) {
					for (int j = 0; j < length - i - 1; j++) {
						// x, y, z 좌표 기준으로 내림차순 비교하여 정렬
						if (point[j].x < point[j + 1].x ||
							(point[j].x == point[j + 1].x && point[j].y < point[j + 1].y) ||
							(point[j].x == point[j + 1].x && point[j].y == point[j + 1].y && point[j].z < point[j + 1].z)) {
							// 점 교환
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