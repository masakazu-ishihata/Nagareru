## 概要

[流れるループ][Nagareru] のソルバー・ジェネレータ


## 構成
- dataset: 流れるループの問題集
  - handcrafted: パズルクリエイターのブログから収集した問題
  - generated: 本プログラムで生成された問題
- source: 本プログラムのソースコード


## 準備

[TdZdd][] をダウンロードし，インクルードパスを通す．


## 使用方法

### コンパイル

```bash
$ cd source
$ make
g++ -O3 -Wall -std=c++11  -c main.cpp
g++ -O3 -Wall -std=c++11 -o nagareru main.o -lm
```

### 問題を解く
```bash
$ ./nagareru --file example.txt --dump
////////////////////////////////////////
// Solving
////////////////////////////////////////
Problem File : example.txt
# board nodes : 89
# board edges : 118
# zdd nodes (non-reduced) : 66
# zdd nodes (reduced)     : 66
# solutions : 1

$ dot -Tgif example_solution_1.dot -o example_solution_1.gif
$ open example_solution_1.gif
```
得られた `example_solution_1.gif` が `example.txt` の解を表しているか[確認][example]する．

### 問題を作る
```bash
$ ./nagareru --file random.txt --rand --width 10 --height 10
////////////////////////////////////////
// Random Generation
////////////////////////////////////////
Problem File : random.txt
CNF File     : random.cnf
Board Width  : 10
Board Height : 10
Init # Cells : 0
Seed         : 3316745319
# count call : 349
```
得られた `random.txt` を[ぱずぷれ][pzv]で開いて解いてみる．


## 入力ファイル

[ぱずぷれ][pzv] 形式で記述された流れるループの問題  
以下は `example.txt` の中身（[ぱずぷれ][pzv] で[開く][example]）  

```
pzprv3
nagare
10
10
. . . . . . . . . .
. U . r r . . . . .
. . . . . . u . . .
. . . . R . . . . .
. U u . . . U . N .
. D . . . l . . . .
. . . N . . . N u .
. D . D . r . . . .
. . . . . . . N . .
. . . . . r . . . .
```

ファイルの意味は上から順に以下の通り．
* フォーマット名
* パズル名
* 高さ
* 幅
* 以降は盤面を記述
  * .：空マス
  * 大文字アルファベット：黒マスとその方向
  * 小文字アルファベット：白マスとその方向

## 出力ファイル

`./nagareru --file example.txt --dump` を実行すると以下のファイルが出力される．

* `example_graph.lst`: 入力問題のグラフ表現の辺リスト
* `example_graph.dot`: 入力問題のグラフ表現の dot ファイル
* `example_solution_1.dot`: 入力問題の１つ目の解の dot ファイル（注意：解の数だけ出力）
* `example_zdd.dot`: `example.txt` 入力問題の解すべてを格納する ZDD の dot ファイル


## 参考文献・URL
* [流れるループ][Nagareru]
* [TdZdd][TdZdd]
* [ぱずぷれ][pzv]
* データ収集したパズルクリエーターブログ
  * [チビスケ丸のパズル置き場][b1]
  * [黄色いパズル][b2]
  * [パズルに関する備忘録][b3]
  * [トクナキラのパズル工房][b4]
  * [かしゆさかなのパズル公園][b5]
  * [オーキシンのパズル保管庫][b6]
  * [pzdc][b7]

[Nagareru]: https://www.nikoli.co.jp/ja/puzzles/nagareru_loop/ "Nagareru"
[TdZdd]: https://github.com/kunisura/TdZdd "TdZdd"
[pzv]: http://pzv.jp/ "PUZ-PRE"
[example]: http://pzv.jp/p.html?nagare/10/10/k6a44k1g9f61c6a5b7c3g5c51b7a7a4k5g4d "example.txt"

[b1]: http://tibisukemaru.blog.fc2.com/
[b2]: http://kiiroipazuru.blog.fc2.com/
[b3]: http://subarupuz.blog.fc2.com/
[b4]: http://puzzleblog542.blog.fc2.com/
[b5]: http://3141592653589797584.blog.fc2.com/
[b6]: http://auxinpuzzle.blog.fc2.com/
[b7]: http://pzdc.blog.fc2.com/
