/*状态事件*/

var currentColor = 'black'; // 默认为黑色先行

function status1(){
    currentColor = 'black';
    document.getElementById('qizi').src = "black.png";
}
function status2(){
    currentColor = 'white';
    document.getElementById('qizi').src = "white.png";
}

/* 控件事件 */
function closeFunction() {
if (confirm("是否退出游戏？")) {
   window.close();
} else {
   history.back();
}
}

/* 判断输赢 */
var cnt = function() {
   return currentColor;
};


var tds = document.getElementsByTagName('td');
var iswin = false; // 有没有分出胜负

// 负责下棋，即改变单元格的背景
var xia = function() {
   if (iswin) {
       alert('游戏结束!');
       return;
   }
   if (this.style.background.indexOf('.png') >= 0) {
       alert('不能重复放置棋子！');
       return;
   }

   var color = cnt();
   this.style.background = 'url(' + color + '.png)'; // 先显示棋子

   // 使用回调来处理胜利消息
   judge.call(this, color, function(winMessage) {
       setTimeout(function() { alert(winMessage); }, 10);
   });

   if (!iswin) {
       // 如果游戏没有结束，则交换颜色并更新当前棋手的图片
       currentColor = (currentColor === 'black') ? 'white' : 'black';
       document.getElementById('qizi').src = currentColor + ".png";
   }
}


// 判断胜负的函数
var judge = function(color, callback) {
   var curr = {
       x: this.cellIndex,
       y: this.parentElement.rowIndex,
       color: color
   };
   var line = ['', '', '', '']; // 横，竖，左斜，右斜四个方向
   var winMessage = ''; // 胜利消息

   // 遍历所有单元格以建立四条线的状态
   for (var i = 0, tmp = {}; i < tds.length; i++) {
       tmp = {
           x: tds[i].cellIndex,
           y: tds[i].parentElement.rowIndex,
           color: '0'
       };

       if (tds[i].style.background.indexOf('black') >= 0) {
           tmp.color = 'b';
       } else if (tds[i].style.background.indexOf('white') >= 0) {
           tmp.color = 'w';
       }

       if (curr.y == tmp.y) { // 同一行
           line[0] += tmp.color;
       }
       if (curr.x == tmp.x) { // 同一列
           line[1] += tmp.color;
       }
       if ((curr.x + curr.y) == (tmp.x + tmp.y)) { // 左斜线
           line[2] += tmp.color;
       }
       if ((curr.x - tmp.x) == (curr.y - tmp.y)) { // 右斜线
           line[3] += tmp.color;
       }
   }

   // 检查是否有五子连线
   for (var i = 0; i < 4; i++) {
       if (line[i].indexOf('bbbbb') >= 0) {
           winMessage = '黑方胜了！';
           iswin = true;
           break;
       }
       if (line[i].indexOf('wwwww') >= 0) {
           winMessage = '白方胜了！';
           iswin = true;
           break;
       }
   }

   // 如果有胜利条件，调用回调函数显示胜利消息
   if (winMessage !== '') {
       callback(winMessage);
   }
};

window.onload = function() {
   // 设置初始棋手图片为黑棋
   document.getElementById('qizi').src = "black.png";

   document.getElementsByTagName('table')[0].onclick = function(ev) {
       xia.call(ev.target); // 使用 ev.target 替换 ev.srcElement
   };
}

