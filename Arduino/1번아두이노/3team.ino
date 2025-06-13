#include <Stepper.h>
#include <LiquidCrystal_I2C.h>

#define MR_speed 10 // 오른쪽 바퀴 속력 핀
#define ML_speed 11 // 왼쪽 바퀴 속력 핀
#define MR_direct 12 // 오른쪽 바퀴 방향 핀
#define ML_direct 13 // 왼쪽 바퀴 방향 핀
#define buzer 4 // 부저 핀 

#define Speed 250 // 모터 속력 (기본값)

Stepper myStepper(360, 5, 7, 6, 8);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int angle = 128; // 위치 
int second=0, minute = 3;   // 시, 분, 초
char ch = ':';


int setR_speed = 0;  // 오른쪽 바퀴 속력 설정 
int setL_speed = 0;  // 오른쪽 바퀴 속력 설정 
int break_light = 3;  // 브레이크등 핀
int backward_light = 2;  // 후진등 핀 

boolean is_stop = true;  // 정지상태 확인
boolean is_backward = false;  // 후진상태 확인
boolean is_bomb = false;  // 폭탄 작동상태 확인 

void UGV_commend(unsigned char commend) {
  switch(commend) { // 명령에 따라, 
  case 'w':
    forward();
    is_stop = false;
    is_backward = false;
    break;

   case 'a':
    left();
    is_stop = false;
    is_backward = false;
    break;
  
  case 'd':
    right();
    is_stop = false;
    is_backward = false;
    break;

  case 's':
    stop();
    is_stop = true;
    is_backward = false;
    break;

  case 'x':
    backward();
    is_stop = false;
    is_backward = true;
    break;
  
  case 'j':
    Stepper_left();
    break;

  case 'k':
    Stepper_right();
    break;

  case 'm':
    bomb_deploy();
    break;

  case 'i':
    attacking();
    break;
  }
}

// LOW : 전진,    HIGH : 후진 
void forward() {
  digitalWrite(MR_direct, LOW);
  digitalWrite(ML_direct, LOW);
  setR_speed = Speed;
  setL_speed = Speed; 
}

void backward() {
  digitalWrite(MR_direct, HIGH);
  digitalWrite(ML_direct, HIGH);
  setR_speed = Speed;
  setL_speed = Speed; 
}

void left() {
  digitalWrite(MR_direct, HIGH);
  digitalWrite(ML_direct, LOW);
  setR_speed = Speed;
  setL_speed = Speed; 
}

void right() {
  digitalWrite(MR_direct, LOW);
  digitalWrite(ML_direct, HIGH);
  setR_speed = Speed;
  setL_speed = Speed; 
}

void stop() {
  digitalWrite(MR_direct, LOW);
  digitalWrite(ML_direct, LOW);
  setR_speed = 0;
  setL_speed = 0; 
}

void UGV_motion() {
  analogWrite(MR_speed, setR_speed);
  analogWrite(ML_speed, setL_speed);
  if(is_stop) { // 정지 상태일 시 
    digitalWrite(break_light, HIGH);
  }

  else {
    digitalWrite(break_light, LOW);
  }
  
  if(is_backward) {
    digitalWrite(backward_light, HIGH);
  }

  else {
    digitalWrite(backward_light, LOW);
  }
}

void Stepper_left() {
  myStepper.step(-60);
}

void Stepper_right() {
  myStepper.step(60);
}

void bomb_deploy() {
  is_bomb = true; 
}

void attacking() {
    digitalWrite(buzer, HIGH);
    delay(100);
    digitalWrite(buzer, LOW);
}

void setup() {
  Serial.begin(9600); // 블루투스 통신 초기화 (0,1번핀 사용하기 때문)
  pinMode(MR_direct, OUTPUT); // 오른쪽 바퀴 모터 핀 (출력으로)
  pinMode(ML_direct, OUTPUT); // 왼쪽 바퀴 모터 핀 (출력으로)
  pinMode(break_light, OUTPUT);  // 브레이크등 핀 (출력)
  pinMode(backward_light, OUTPUT); // 후진등 핀 (출력)
  pinMode(buzer, OUTPUT); // 부저
  myStepper.setSpeed(24);

  lcd.init();       // LCD 설정
  lcd.clear();      // LCD를 모두 지운다.
  lcd.backlight();  // 백라이트를 켠다
  lcd.setCursor(0, 0); // 0번째줄, 0번째열로 커서 이동
  lcd.print("activate");
}

void loop() {

  unsigned char commend = 0;  // 명령어 저장 변수 
  
  if(Serial.available() && !is_bomb) {
    commend = Serial.read();  // 명령어 읽기 
    UGV_commend(commend);  // UGV에 명령어 입력
  }
  
  if(!is_bomb) {
    UGV_motion();
  }

  if(is_bomb) {
    if(second < 0){
      minute--;
      second = 59;
    } 
    lcd.clear();
    lcd.setCursor(0, 0);      // 0번째줄, 0번째열로 커서 이동
    lcd.print("Boom Timer!");
    lcd.setCursor(0, 1);      // 1번째줄, 0번째열로 커서 이동
    lcd.print("Time:: ");  
    lcd.print(minute);  // 분
    lcd.print(ch);
    lcd.print(second);  // 초
    second--;
    digitalWrite(buzer, HIGH);
    delay(100);
    digitalWrite(buzer, LOW);
    delay(900);

    if(minute == 0 &&second == 0){
      lcd.clear();
      lcd.print("explode!!!!");
      digitalWrite(buzer, HIGH);
      delay(100000000000);
    }
  }
}
