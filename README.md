# juce-jp-handson-202406

2024年6月に実施したJUCE 8 ハンズオン＆ミートアップに向けて作成した、オーディオプラグインのGUIをWebViewで作成するサンプルコードです。  
このプロジェクトは、C++とJUCE 8を使用したプロジェクトです。Windows、macOS、Linuxの環境をサポートしています。

[イベントページ(外部サイト)](https://juce.connpass.com/event/319164/)

## サンプルプロジェクト：ReactInstrument

![ReactInstrument](.image/plugin-demo.gif)

### プラグインフォーマット

- Windows
    - VST3
    - CLAP
    - Standalone
- macOS
    - AudioUnit
    - VST3
    - CLAP
    - Standalone
- Linux
    - VST3
    - CLAP
    - Standalone

## 前提条件

このプロジェクトでは、以下の3つの依存ツールを使用しています。

- クロスプラットフォームなタスクランナーとして[Deno](https://deno.com/)を使用しています。
- WebViewのビルド環境に[Node.js](https://nodejs.org/)と[npm](https://www.npmjs.com/)を使用しています。
- C++ビルドツールに[CMake](https://cmake.org/)を使用しています。

上記のうち、Denoの用意は必須ではありませんが、プロジェクトのビルド環境の準備、ビルドの実行、インストール処理を短い命令かつクロスプラットフォームで実行することができます。

C++ビルド環境については、JUCEをビルドできる環境があれば十分です。可能であれば、JUCEのDemoRunnerがビルドできることを確認しておくと安心です。  
WebViewビルド環境については、Node.jsとnpmを使用しています。事前にビルド済みのアーカイブファイル `WebViewBundle.zip` を同梱しているので、プログラムを使用することが目的であればWebViewをビルドする作業は必須ではありません。

- C++ビルド環境
    - Visual Studio: 2022 (Windows)
    - Xcode: 14.2以降 (macOS)
    - GCC: 11以降 (Linux)
    - Clang: 14以降 (Linux)
    - Ninja-build: 1.11以降 (Linux)
    - CMake: 3.25以降
    - Git: 2.3以降

- WebViewビルド環境
    - Node.js: 20以降
    - npm: 10以降

## ビルド手順

### Gitリポジトリをクローンする

Gitが使用できる環境にて、以下のコマンドを実行してください。  
このリポジトリには依存するサブモジュールが含まれているため、git cloneプロセスを実行する際に --recursiveオプションを追加してください。  

```sh
git clone --recursive https://github.com/COx2/juce-jp-handson-202406.git
```

Gitサブモジュールをクローンまたは更新するには、次のコマンドを実行します。

```sh
cd juce-jp-handson-202406
git submodule update --init --recursive
```

### Denoタスク一覧

Denoタスクランナーを使用することで、プロジェクトのビルド環境の準備、ビルドの実行、インストール処理を短い命令かつクロスプラットフォームで実行することができます。
Denoタスクランナーを実行するには、Denoランタイム環境が必要です。Denoのインストール方法は公式ウェブサイトをご覧ください。https://deno.com/

利用可能なDenoタスクは以下の通りです:

```
deno task
Available tasks:
- get:webview2
    deno run --allow-all ./Tools/deno/nuget-dl.ts && deno run --allow-all ./Tasks/deno/get-webview2.ts
- get:samplepack
    deno run --allow-all ./Tasks/deno/get-samplepack.ts
- build:all
    deno task build:webviewbundle && deno task build:plugin
- build:webviewbundle
    deno run --allow-all ./Tasks/deno/build-webview-bundle.ts
- build:plugin
    deno run --allow-all ./Tasks/deno/build-plugin.ts
- install:plugin
    deno run --allow-all ./Tasks/deno/install-plugin.ts
- launch:standalone
    deno run --allow-all ./Tasks/deno/launch-standalone.ts
- build:plugin:debug
    deno run --allow-all ./Tasks/deno/build-plugin-debug.ts
- launch:standalone:debug
    deno run --allow-all ./Tasks/deno/launch-standalone-debug.ts
```

### ビルド前の準備

C++ビルドを実行する前に、以下の準備作業を実施してください。

#### Windows: DenoタスクでWebView2を取得する

WebView2ライブラリをMicrosoft社のリポジトリから取得します。

```sh
deno task get:webview2
```

#### DenoタスクでWebViewをビルドしてアーカイブファイルを作成する

```sh
deno task build:webviewbundle
```

### CMakeビルドの実行

#### A. Denoタスクの場合

```sh
deno task build:plugin
```

#### B. プラットフォームネイティブシェルスクリプトの場合

各プラットフォーム用のスクリプトは、次のコマンドを実行してください。

Windows:
```sh
.\Scripts\build_windows_msvc2022.bat
```

macOS:
```sh
./Scripts/build_macos_xcode.sh
```

Linux:
```sh
./Scripts/build_linux_ninja.sh
```

### CMakeインストールの実行

#### A. Denoタスクの場合

Windows: 管理者権限で開いたターミナルからDenoタスクを実行してください。

```sh
deno task install:plugin
```

macOS, Linux: ターミナルからsudo権限でDenoタスクを実行してください。

```sh
sudo deno task install:plugin
```

#### B. プラットフォームネイティブシェルスクリプトの場合

各プラットフォーム用のスクリプトは、次のコマンドを実行してください。

Windows: 管理者権限で開いたターミナルからスクリプトを実行してください。

```sh
.\Scripts\install_windows_msvc2022_vst3.bat
.\Scripts\install_windows_msvc2022_clap.bat
```

macOS: ターミナルからsudo権限でスクリプトを実行してください。
```sh
sudo ./Scripts/install_macos_xcode_au.sh
sudo ./Scripts/install_macos_xcode_vst3.sh
sudo ./Scripts/install_macos_xcode_clap.sh
```

Linux:ターミナルからsudo権限でスクリプトを実行してください。
```sh
sudo ./Scripts/install_linux_ninja_vst3.sh
sudo ./Scripts/install_linux_ninja_clap.sh
```

### スタンドアローン版の起動

プラットフォーム毎のスタンドアローン版の成果物は以下の場所に置かれています。  
スタンドアローン版の起動は、各プラットフォームの実行手順で立ち上げることが出来ます。  

* Windows: `./builds/vs2022/ReactInstrument/ReactInstrument_artefacts/Release/Standalone/ReactInstrument.exe`
* macOS: `./builds/xcode/ReactInstrument/ReactInstrument_artefacts/Release/Standalone/ReactInstrument.app`
* Linux: `./builds/ninja-single-release/ReactInstrument/ReactInstrument_artefacts/Release/Standalone/ReactInstrument`


#### Denoタスクでスタンドアローン版を起動する

```sh
deno task launch:standalone
```

#### Debug版のビルド

```sh
deno task build:plugin:debug
```

#### Debug版の起動

```sh
deno task launch:standalone:debug
```

## Attention Regarding Licenses

If you are using this project to create and distribute software, please ensure compliance with the licenses of the following software libraries:

+ [JUCE](https://github.com/juce-framework/JUCE)  
+ [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions)  
