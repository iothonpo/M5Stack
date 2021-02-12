/* Joystick Hatの動作確認用プログラム
 * ジョイスティックの位置情報を表示する
 * https://iothonpo.com/m5stickc-joystick-hat/
 */
#include <M5StickC.h>
// M5StickC.h内でWire.hをincludeしてるので下記行は不要。
// #include <M5StickC.h>

// I2C通信の設定
#define JOYSTICK_ADDRESS 0x38 // 　通信するスレーブアドレス
#define JOYSTICK_REGISTER 0x02 // アクセスするレジスタ
#define SDA 0  //SDAのピンアサイン（GPIO0）
#define SCL 26 //SCLのピンアサイン（GPIO26）
#define FREQUENCY 100000 // 通信速度の設定。100Kbps(Standard mode)

// ジョイスティックの位置情報を格納する構造体
struct joystickData {
	int8_t x; // x座標
	int8_t y; // y座標
	boolean btnStatus; // センターボタンの押下検知
	boolean connectionStatus; // 通信状態
};
struct joystickData data; // 実体(entity)の宣言

// I2C通信でデータ取得
void readData() {
	Wire.beginTransmission(JOYSTICK_ADDRESS); // 通信するスレーブアドレス
	Wire.write(JOYSTICK_REGISTER); // readするレジスタアドレスをキューに追加
	if(Wire.endTransmission() != 1){	 // キュー送信とエラー処理
		data.connectionStatus = false; //通信失敗
	}
	Wire.requestFrom(JOYSTICK_ADDRESS, 3); //Joystickが返す3byteを取得
	if (Wire.available()) {
		data.x = Wire.read(); // 1byte目はx座標
		data.y = Wire.read(); // 2byte目はy座標
		data.btnStatus = Wire.read(); // 3byte目はボタン状態
		data.connectionStatus = true; // 通信成功
	}
}

// M5stickCの初期化
void setup() {
	M5.begin();
	M5.Lcd.setRotation(0);
	M5.Lcd.fillRect(0, 0, 80, 160, BLACK);
	Wire.begin(SDA, SCL, FREQUENCY);
}

// ジョイスティックの状態を表示
void loop() {
	readData();
	M5.Lcd.setCursor(0, 0);  
	M5.Lcd.printf("X: %4d\n", data.x); // 符号付4桁を右詰めで表示
	M5.Lcd.printf("Y: %4d\n", data.y);
	M5.Lcd.printf("B: %s\n", data.btnStatus ? "Released" : " Pressed"); // スペースで古い表示を上書きする
	M5.Lcd.printf("Status:\n%s\n", data.connectionStatus ? "   Connected" : "Disconnected");
}
