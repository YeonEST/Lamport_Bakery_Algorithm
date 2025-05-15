// Lamport의 Bakery Algorithm을 구현
#include<iostream>
#include<thread>
#include<vector>
#define numb 4 // 테스트 할 스레드의 개수는 8개
using namespace std;

bool EnterFlag[numb + 1] ={ false }; // 임계구역에 들어가면 해당 ID에 해당하는 인덱스가 true
int turn[numb + 1] = { 0 }; // 각 스레드에 주어진 번호표의 숫자 값
int counter = 1; // 임계구역 작업 시 사용하기 위한 스레드들의 임계구역 액세스 횟수

void CriticalTask(int ID) {
    if (counter % 20 == 0) cout << endl; // 순서 출력이 너무 길어지면 보기 힘들기 때문에 20개 마다 끊음
    cout << ID << " "; // 임계구역에 진입 한 순서를 적는걸 임계구역 안 작업으로 설정
    counter++;
}

int Checking (int i, int ID) {
    // 1인 경우는 대기, 0이면 진입
    if (turn[i] < turn[ID]) return 1;
    else if (turn[i] > turn[ID]) return 0;
    else { // 표 번호(turn)가 같으면
        if (i < ID) return 1;
        else return 0;
    }
}

int FindMax(int a, int b){
    if (a > b) return a; // 번호표의 최대값 + 1을 스레드에 배분해야하기 때문에 최대값을 탐색하기위해 만듦
    else return b; // 더 큰 값을 리턴함
}

void LockFun(int ID){ // 임계구역 권한 파트
    EnterFlag[ID] = true; // 임계구역에 진입한 작업의 상태를 true로 셋
    int maxi = turn[0]; // 번호표 작업 시작
    for(int j = 0; j < numb; j++) maxi = FindMax(maxi, turn[j]); // 번호표 수에서 최대값을 찾음
    turn[ID] = 1 + maxi; // 번호표 푀대값에 1을 추가해 번호표 지급
    EnterFlag[ID] = false; // unlock으로

    for (int i = 0; i < numb; ++i){ // 번호표 비교 반복문
        while (EnterFlag[i]); // i번 스레드가 번호표가 있나 확인
        while ((turn[i] != 0) && Checking(i, ID)); // 자신의 번호표 보다 큰 값의 번호를 가진 스레드를 찾으면 넘김
    }
}

void UnlockFun(int ID) { // 임계구역 차지를 끝냄
    turn[ID] = 0; // 번호표를 0(번호표 없음)으로 돌림
}

void Task(int ID) {
    int temp; // 스레드의 작업용 변수
    for (temp = 0; temp < 100000000; ++temp) { // 스레드가 할 작업은 9999만까지 카운팅
        LockFun(ID); // 임계구역 진입
        /*temp값이 ID + 1로 나누어 떨어지면 크리티컬존 진입함(이유는 각 스레드의 진입 타이밍을 다르게 하기 위해서)*/
        if(temp % ((ID + 1) * 1000000)  == 0) CriticalTask(ID);
        UnlockFun(ID); // 비임계구역
    }
    cout << "Task[" << ID << "] : " << temp - 1 << "(Done)" << endl; // 카운팅이 끝나면 종료되었다 출력
}

int main(void) {
    vector<thread> tasks;
    for (int i = 0; i < numb; i++) {
        tasks.emplace_back(Task, i); // 스레드를 numb만큼 생성
    }
    for (int i = 0; i < numb; i++) {
        tasks[i].join(); // 모든 함수가 종료될 때 까지 대기
    }
    return 0;
}