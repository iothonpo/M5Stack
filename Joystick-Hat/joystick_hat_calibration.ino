/* Joystick Hatのキャリブレーション
 * ゼロ点補正、もしくはゼロ点＋最大値補正の２パターンあり。
 *  https://iothonpo.com/m5stickc-joystick-hat/
 */
#include <M5StickC.h>

// I2C通信の設定
#define JOYSTICK_ADDRESS 0x38 // 　通信するスレーブアドレス
#define JOYSTICK_REGISTER_RAW 0x01 // 生データのレジスタ
#define JOYSTICK_REGISTER_DATA 0x02 // 演算したデータのレジスタ
#define JOYSTICK_REGISTER_CAL 0x03 // キャリブレーション用のレジスタ
#define SDA 0  // SDAのピンアサイン（GPIO0）
#define SCL 26 // SCLのピンアサイン（GPIO26）
#define FREQUENCY 100000 // 通信速度の設定。100Kbps(Standard mode)

// ジョイスティックの情報を格納する構造体
struct joystickData {
	int16_t rawX; // x座標の生データ
	int16_t rawY; // y座標の生データ
	int8_t axisX; // 演算後のx座標
	int8_t axisY; // 演算後のy座標
	boolean btnStatus; // センターボタンの押下検知
	boolean connectionStatus; // 通信状態
};
struct joystickData data; // 実体(entity)の宣言

// ゼロ点のみ補正するモード
void centerCalibration() {
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_CAL);
	Wire.write(0x01); // ゼロ点補正モードを指定
	Wire.endTransmission();
	delay(1000); // 現在時点をゼロ点とする
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_CAL);
	Wire.write(0x00); // 補正モードを終了する
	Wire.endTransmission();
}
// ゼロ点とx軸、y軸の最大値を補正するモード
void fullCalibration() {
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_CAL);
	Wire.write(0x02); // ゼロ点補正モードを指定
	Wire.endTransmission();
	// ジョイスティックをぐるぐる回し、x軸、y軸の最大値を補正する
	M5.Lcd.setCursor(0, 0); 
	M5.Lcd.print("Rotate Joystick");
	delay(20000);
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_CAL);
	Wire.write(0x03); // 補正モードを終了する
	Wire.endTransmission();
}

// I2C通信でデータ取得
void readData() {
	// 生データの取得
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_RAW);
	if(Wire.endTransmission() != 1){
		data.connectionStatus = false;
	}
	Wire.requestFrom(JOYSTICK_ADDRESS, 4);
	if (Wire.available()) {
		data.rawX = Wire.read(); // x座標生データの下位バイト
		data.rawX |= Wire.read() << 8; // x座標生データの上位バイト
		data.rawY = Wire.read();
		data.rawY |= Wire.read() << 8;
	}

	// 演算したデータの取得
	Wire.beginTransmission(JOYSTICK_ADDRESS);
	Wire.write(JOYSTICK_REGISTER_DATA);
	if(Wire.endTransmission() != 1){
		data.connectionStatus = false;
	}
	Wire.requestFrom(JOYSTICK_ADDRESS, 3);
	if (Wire.available()) {
		data.axisX = Wire.read();
		data.axisY = Wire.read();
		data.btnStatus = Wire.read();
		data.connectionStatus = true;
	}
}

// M5stickCの初期化
void setup() {
	M5.begin();
	M5.Lcd.setRotation(0);
	M5.Lcd.fillRect(0, 0, 80, 160, BLACK);
	Wire.begin(SDA, SCL, FREQUENCY);
	// キャリブレーションする場合にコメントアウトする
	//centerCalibration(); // ゼロ点補正する場合
	fullCalibration(); // ゼロ点,最小値,最大値を補正する場合
	M5.Lcd.fillRect(0, 0, 80, 160, BLACK);
}

// ジョイスティックの状態を表示
void loop() {
	readData();
	M5.Lcd.setCursor(0, 0); 
	M5.Lcd.printf("Raw X: %4d\n", data.rawX);
	M5.Lcd.printf("Raw Y: %4d\n", data.rawY); 
	M5.Lcd.printf("X: %4d\n", data.axisX);
	M5.Lcd.printf("Y: %4d\n", data.axisY);
	M5.Lcd.printf("B: %s\n", data.btnStatus ? "Released" : " Pressed"); // スペースで古い表示を上書きする
	M5.Lcd.printf("Status:\n%s\n", data.connectionStatus ? " Connected" : "Disconnected");
}
