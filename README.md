# 概要
B-ARTはXbee ZigBeeモジュールみたいにUARTを無線で飛ばすモジュールです。
マイコンのプログラムを追加で書くことなく、UARTのTXとRXだけをつなげるだけでUARTを無線化できます。
無線部分はBLE(Bluetooth Low Energy)で実装をしています。

非常に小さいことが特徴で、**開発中の基板に挿して簡単に使える無線モジュール**をコンセプトに開発をしています。
(現状、秋月で買えるどの無線モジュールよりも小さいはず)

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_overview1.jpg)

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_overview2.jpg)

B-ARTにはType-PとType-Uの2種類があり、Type-Pはピンヘッダ、Type-UはUSBコネクタがついています。
Type-Pには4pinのピンヘッダが付いていて、3.3V-GND-TX-RX
の4本を接続するだけで使うことができます。
Type-UにはPCに直接挿せるUSBコネクタと、FTDIのUSB-UART変換IC(FT234XD)が載っています。
FT234XDのおかげでPCにさすとシリアルポートとして認識されるので、
普段シリアルポートを使うときと全く変わることなく使うことができます。

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_pc.jpg)

B-ARTの裏面にはType-P、Type-UともにDIPスイッチがあり、そこからIDを設定できます。
IDが同じ相手を選択して通信をするので、同じ環境にB-ARTが何組もあってもIDさえ違えば混線が起きないようになっています。

## 使用例
例えば小さい走行ロボットを無線を使ってデバッグする時にB-ARTは役立つでしょう。
走行をさせるときに有線だと線が邪魔になり、Xbeeなどはモジュールが大きすぎて載せられないという状況です。

下の写真は現在制作中のマイクロマウスの機体[MIZUHOv2](http://idken.net/posts/2017-04-01-mizuhov2/)に
B-ART TypePを挿しているところです。

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_mouse.jpg)

B-ARTは非常に小さく軽いので、マイクロマウスの機体に乗せても走行時にそれほど問題にはなりません。
基板に4pinのコネクタさえつけておけばほしい時にだけB-ARTを挿して使うことができます。

# 使い方

## DIPスイッチの設定

B-ARTのType-PとType-Uの基板の裏にはDIPスイッチが4bit分あり、
1bitはクライアント動作をするかサーバー動作をするか、3bitはIDを意味していています。

| DIPスイッチ | 意味                     |
|-------------|--------------------------|
| 1           | 0:client動作  1:server動作 |
| 2           | ID  bit2                   |
| 3           | ID  bit1                   |
| 4           | ID  bit0                   |

B-ARTは通信においてはType-PであるかType-Uであるかは関係なく、
同じIDを持つサーバーとクライアントが1対1で通信できるという仕組みになっています。
なので、同じIDを持っていさえすれば、

* TypeU <-> TypeU
* TypeP <-> TypeP
* TypeU <-> TypeP

の全ての組み合わせで通信が可能です。



## TypePのセットアップ
4pinのピンヘッダ(というよりスルーホール)があり、それぞれのpinは次のような意味を持っています。

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_pin.jpg)

B-ARTは最大30mAくらいの電流を必要とするので、それに耐えられるような電源につないでください。

## TypeUのセットアップ
パソコンのUSBに挿すだけです。

## 電源を入れる
電源を入れると
クライアント動作に設定されているB-ARTは赤色LEDが、
サーバー動作に設定されているB-ARTは緑色のLEDが光ります。


## サーバーとクライアントの接続
サーバー側とクライアント側に同じIDを持たせた状態で電源を入れると自動的に接続されます。
接続された瞬間にB-ARTに乗っている緑と赤の両方のLEDが一瞬点滅します。

一旦切断されても、接続可能な状態になると自動で接続されます。

## データを送受信する
UARTにデータを流すとサーバー側、クライアント側どちらからでももう片方に送信されます。
その際にLEDが点滅するので、LEDを見ることで正常に動作しているかが分かります。


# 基板

### TypeP

回路図

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_p_sch.png)


アートワーク

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_p_pcb.png)

### TypeU

USB周りの回路図

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_u_sch1.png)

マイコン周りの回路図

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_u_sch2.png)

アートワーク

![](https://raw.githubusercontent.com/idt12312/B-ART/doc/doc/bart_u_pcb.png)


# ファームウェア

## 開発環境

* [Nordic nRF5 SDK v12.1](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF5-SDK)
* Nordic SoftDevice S130
* [Eclipse Juno](https://eclipse.org/)
* [Eclipse CDT](https://eclipse.org/cdt/)
* [GNU ARM Eclipse Plugin](http://gnuarmeclipse.github.io/)
* [gcc-arm-none-eabi-4_9-2015q3](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update)

Eclipseのプロジェクトごとリポジトリにあげてあるので、
きっとEclipseにimportするだけで動かすことができます。

## 動作
起動時にDIPスイッチの状態を読み取って(main.c)、サーバー動作(main_server.c)をするか、
クライアント動作(main_client.c)をするかを決定しています。

BLEの操作はNordicのサンプルにある、NUSを参考にしています。
SDKにあるble_nusとble_nus_cをもとにble_bartsとble_barts_cを書いています。(名前を変えただけ)

### サーバー動作

GATTサーバーとして、次のようなサービスを立てます。

| AttributeType  | Name        | Permissions | UUID                                 |
|----------------|-------------|-------------|--------------------------------------|
| Service        | BART Service | Read       | 20yy0001-DA04-4C71-ADBE-6CA269E7E983 |
| Characteristic | TX          | Write       | 20yy0002-DA04-4C71-ADBE-6CA269E7E983 |
| Characteristic | RX          | Notify      | 20yy0003-DA04-4C71-ADBE-6CA269E7E983 |

TXにwriteするとUARTからデータが出力され、
UARTにデータが入るとRXをsubscribeしているデバイスにNotifyします。

UUIDのyyはID(3bit)を16進数に変換したものが入ります。


### クライアント動作
自分と同じIDをもつサーバーを探します。
このとき、サーバーのもつServiceのUUID(上の表のyy)をもとに同じIDのものを検索します。

同じIDのサーバーが見つかったらConnectし、RXのCharactericics(notify)をsubscribeします。

後はUARTからデータが来たらサーバーのRXにwriteをし、サーバーからnotifyが来たらUARTにデータを流します。