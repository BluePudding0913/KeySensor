#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define DATABASE_URL "https://XXX"//firebase
#define LED_BUILTIN 16
#define ADC 35 // PINの登録では0と6~11番を登録するのはやめとけ
#define interval  30//ネットに接続できない間は30s毎に接続先を切り替える
#define r_delay 600//600sごとにセンサーの値を読みFireBaseに送信


const int timeout = 3;//再起動のスパン
int angle;//opened:3000,locked:2400,criterion:2700   pre:2475
bool x = 0;
int t = 0;
int u = 0;
uint64_t count;

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

void setup(){ //セットアップ後はLEDを点滅
  //Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ADC, INPUT);
  connect();
  fbrtdb_setup();
  for(t = 0; t < 8 ;++t)blink(100);
  while(true){
    l00p();
  }
}

void fbrtdb_setup(){
  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
  ssl.setInsecure();

  // Initialize the authentication handler.
  initializeApp(client, app, getAuth(noAuth));

  // Binding the authentication handler with your Database class object.
  app.getApp<RealtimeDatabase>(Database);

  // Set your database URL
  Database.url(DATABASE_URL);

  // In sync functions, we have to set the operating result for the client that works with the function.
  client.setAsyncResult(result);
}

void blink(int x){//指定した間隔でLEDを一回点滅する
  digitalWrite(LED_BUILTIN, HIGH);
  delay(x);
  digitalWrite(LED_BUILTIN, LOW);
  delay(x);
}

void connect() {//確実にネットにつなぐ関数,接続出来たらLEDを4秒点灯
  while(WiFi.status() != WL_CONNECTED){

    WiFi.disconnect(true);
    WiFi.begin("denken10", "PW1");
    lp();
    if(WiFi.status() == WL_CONNECTED)break;

    WiFi.disconnect(true);
    WiFi.begin("denken11", "PW2");
    lp();
    if(WiFi.status() == WL_CONNECTED)break;

    WiFi.disconnect(true);
    WiFi.begin("aterm-2de1d8-g", "PW3");
    lp();
    if(WiFi.status() == WL_CONNECTED)break;

    //WiFi.disconnect(true);
    //WiFi.begin("CHUO-U", WPA2_AUTH_PEAP, "MYID_IN_UNIV", "MYID_IN_UNIV", "");
    //lp();
    //if(WiFi.status() == WL_CONNECTED)break;
  }
  blink(4000);
}

void lp(){
  t = 0;
  while (!(WiFi.status() == WL_CONNECTED || t == interval)) {
    ++t;
    //Serial.print(".");
    blink(500);
  }
  delay(4000);
}

bool sendData() {
  angle = analogRead(ADC);
  if (Database.set<int>(client, "/bird", angle)){
    count = millis();
    delay(1000);
    Database.set<int>(client, "/count", count);
    t = temperatureRead();
    delay(1000);
    Database.set<int>(client, "/temperature", t);
    t = 0;
    delay(1000);
    t = Database.get<int>(client, "/sleep");
    blink(100);
    if(t > 0){
      sleep(t);
    }
    return true;
  }
  return false;
}

void sleep(int k){//スリーブするときは７回点滅
    delay(4000);
    Database.set<int>(client, "/sleep", -1);
    int i = 0;
    for(i=0;i<7;++i){
      blink(50);
      blink(50);
      delay(800);
    }
    WiFi.disconnect(true);
    count = 60ULL * 1000000;
    count = count*k;
    esp_sleep_enable_timer_wakeup(count);
    esp_deep_sleep_start();
}

void l00p(){
  x = sendData();
  u = 0;
  while (!x) {
    ++u;
    if(u==timeout)ESP.restart();
    connect();
    fbrtdb_setup();
    delay(5000);
    x = sendData();
  }
  delay(r_delay * 1000);
}

void loop(){}




