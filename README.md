# AviUtl プラグイン - イージング選択

* version 2.0.0 by 蛇色 - 2022/03/19 画像を選択できるように修正
* version 1.1.0 by 蛇色 - 2022/03/18 負数も使えるように修正
* version 1.0.0 by 蛇色 - 2022/03/18 初版

イージング番号を GUI で選択します。

## 導入方法

以下のファイルとフォルダを AviUtl の Plugins フォルダに入れてください。
* SelectEasing.auf
* SelectEasing (フォルダ)

## 使用方法

1. イージング番号を入力するダイアログを出します。
2. その横にイージング画像が表示されるので、その中からイージング番号を選びます。

## 設定方法

SelectEasing.ini をテキストエディタで編集してから AviUtl を起動します。

```ini
[viewer]
imageVersion=2 ; 画像のバージョンを指定します。
negative=0 ; 1 の場合は負数になります。例えばイージング番号 41 を選択すると数値は -41 になります。
left=1 ; 1 の場合はダイアログの左側に画像を表示します。0 の場合は右側に表示します。
alpha=255 ; 画像のアルファ値を指定します。
scale=100 ; 画像の表示倍率を指定します。
selectedColorR=0xff ; 選択状態の色を指定します。
selectedColorG=0x00
selectedColorB=0xff
selectedColorA=0x66
hotColorR=0xff ; ホット状態の色を指定します。
hotColorG=0xff
hotColorB=0x00
hotColorA=0x66
```

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r14 https://www.nicovideo.jp/watch/sm40087155
