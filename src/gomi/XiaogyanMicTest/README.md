# XIAOGYANでインターネットラジオ
[XIAOGYAN](https://github.com/algyan/XIAOGYAN) (IoT ALGYAN 8周年記念ボード)で動作するインターネットラジオのサンプルコードです。

※ XIAO ESP32「S3」用です。「C3」ではうまく動作しません。

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/EtCeO2uyNWM/0.jpg)](https://www.youtube.com/watch?v=EtCeO2uyNWM)

- PlatformIO用のプロジェクトです。
- [XIAOGYAN Support Library](https://github.com/algyan/xiaogyan_arduino)のSpeakerクラスを少しいじってます。
- [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S)をXIAOGYAN用に改造しました。
- ↑ のライブラリにインターネットラジオ局のURLを渡してるだけです。(^^;)
- ときどきうまくつながらず(?)、音が出ないことがあります。
- シリアルポートにログが出ます。