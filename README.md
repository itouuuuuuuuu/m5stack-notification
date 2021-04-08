# M5Stack Notification Sample

## 初期設定
FirebaseとWi-Fiの情報を設定するために、 下記コマンドで `secret.h` ファイルを作成します。

```
$ cp ./src/secret.h.sample ./src/secret.h
```

作成された `secret.h` に、FirebaseとWi-Fi情報を記述して保存します。

```
#define FIREBASE_HOST "リアルタイムDBのホスト名（例：hoge-default-rtdb.firebaseio.com）"
#define FIREBASE_AUTH "RDBシークレットキー"
#define WIFI_SSID "Wi-FiのSSID"
#define WIFI_PASSWORD "Wi-Fiのパスワード"
```