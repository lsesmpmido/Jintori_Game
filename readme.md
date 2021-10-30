# Jintori_Game

<h3 style="color:#ff0000">ジャンル：「パズルゲーム」</h3>

このゲームはドラッグで図形を描画することで、領土を獲得するゲームです。

画面の65％以上の自分の領土で埋めることができたらゲームクリアで、残り時間が0になった場合や、図形を描画中に敵と接触した場合はゲームオーバーになります。

# 仕様

<p align="center">
<img src="https://user-images.githubusercontent.com/54675248/139556659-8d6d6d36-dd0e-410a-86f3-70683cbb0a84.png" width="320px" >
</p>

* カラフルな■は敵を表しており、ドラッグ時に触れるとゲームオーバーになります。
* 画面下のスコアの「Time」、「area」、「rect_num」はそれぞれ、「残り時間」、「画面に占める自分の領土の割合」、「描画した図形の個数」を表しています。
* ゲームのクリア条件はareaの値が65％を超えることであり、Timeが0または図形を描画中に敵から攻撃を受けるとゲームオーバーとなります。

# 操作方法
 
<h3 style="color:#ff0000">図形の作成</h3>

* 画面内の任意の場所でマウスの右ボタンをクリックする．
* そのまま，作成したい図形をドラッグして描き，任意の場所で右ボタンを離すと図形が作成される
<p align="center">
<img src="https://user-images.githubusercontent.com/54675248/139557655-9ace658d-6ad9-45f5-b9ff-33358ff064ff.png" width="320px" >
</p>

図形を重ねて描く、または画面外まで描こうとすると、背景色が黄色になり、図形を作成することができません。
<p align="center">
<img src="https://user-images.githubusercontent.com/54675248/139557494-cdd4925f-536b-4939-81b3-227fd04a7034.png" width="320px" >
</p>

<h3 style="color:#ff0000">ゲームのリセット</h3>

* キーボードの「F1」キーを押すとゲームをリセットします。

このゲームはWindows用です。
