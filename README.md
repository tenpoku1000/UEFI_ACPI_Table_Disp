# UEFI_ACPI_Table_Disp

ACPI テーブルの取得結果の表示・ファイルへの保存を行う UEFI アプリケーションです。

[Acer Aspire E1-572 取得結果](https://raw.githubusercontent.com/tenpoku1000/UEFI_ACPI_Table_Disp/master/result1_Acer_Aspire_E1-572.txt)  
[ASUS H170-PRO 取得結果](https://raw.githubusercontent.com/tenpoku1000/UEFI_ACPI_Table_Disp/master/result2_ASUS_H170-PRO.txt)  
[ASRock FM2A68M-HD+ 取得結果](https://raw.githubusercontent.com/tenpoku1000/UEFI_ACPI_Table_Disp/master/result3_ASRock_FM2A68M-HD+.txt)  

## 開発環境

* Visual Studio Community 2015 + Windows SDK for Windows 10
https://developer.microsoft.com/ja-jp/windows/hardware/windows-driver-kit
* 64 ビット版 Windows 10
* 64 ビット UEFI 対応 PC
* USB メモリ

## 実行方法

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。  
  セキュアブート対応 PC ではセキュアブートを無効に設定します（無効化出来ない PC では動作不可）。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* USB メモリのルート・フォルダに情報取得結果ファイル result.txt が出力されます。
* USB メモリを PC から抜いて任意のキーを押すと、PC がリセットされて Windows が起動します。

PC の機種によっては、Boot 順序設定変更を恒久的な設定変更ではなく、  
次回起動時のみ有効の設定とした方が好ましい場合があります。恒久的な  
設定変更で、  PC リセット後の Windows 起動がスムーズに行えないか、  
起動しない場合があるためです。ご注意ください。  

## ビルド方法

* UEFI_ACPI_Table_Disp.sln をダブルクリックします。
* Visual Studio のセキュリティ警告を回避してプロジェクトを開きます。  
![warning](https://raw.githubusercontent.com/tenpoku1000/UEFI_ACPI_Table_Disp/master/images/MSVC.PNG)
* Visual Studio の起動後に F7 キーを押下します。efi フォルダ以下に UEFI アプリケーションが生成されます。

## 謝辞

本ソフトウェアは、以下の外部プロジェクトの成果物を利用しています。感謝いたします。

### BSD License
* gnu-efi http://sourceforge.net/projects/gnu-efi/
* UDK2015 http://www.tianocore.org/udk/udk2015/

## ライセンス

[MIT license](https://raw.githubusercontent.com/tenpoku1000/UEFI_ACPI_Table_Disp/master/LICENSE)

## 作者

市川 真一 <suigun1000@gmail.com>

## 参考資料

* UEFI - PhoenixWiki http://wiki.phoenix.com/wiki/index.php/UEFI
* Unified Extensible Firmware Interface Specification Version 2.5 April, 2015
http://www.uefi.org/sites/default/files/resources/UEFI%202_5.pdf
* Advanced Configuration and Power Interface Specification Version 6.0 April, 2015
http://www.uefi.org/sites/default/files/resources/ACPI_6.0.pdf
