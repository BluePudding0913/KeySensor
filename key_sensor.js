//GASのコードです
//この関数はAppScriptのトリガーを用いて１分ごとに実行されています
//関数の内容:FirebaseのRealtime databaseに保存されているbirdという値を読み取って条件を満たせばdiscordにウェブフックでメッセージを送信する関数です
//"bird"には部室のドアノブのつまみに取り付けられた加速度センサーの値が入っています。
function readRoomKey() {
  var databaseUrl_1 = "XXX.json"; // FirebaseのURLを指定
  var databaseUrl_2 = "XXX.json"; // FirebaseのURLを指定
  var databaseUrl_3 = "XXX.json";
  var databaseUrl_4 = "XXX.json";
  var webhookUrl = "XXX"; // DiscordのウェブフックURLを指定
  var webhookUrl_denken = "XXX";
  var token = 'XXX'; //LINE Notifyで発行したアクセストークン
  try {
    var options = {
      muteHttpExceptions: true
    };
    
    var response = UrlFetchApp.fetch(databaseUrl_1, options);
    var response_2nd = UrlFetchApp.fetch(databaseUrl_2, options);
    var response_3rd = UrlFetchApp.fetch(databaseUrl_3,options);
    var response_4th = UrlFetchApp.fetch(databaseUrl_4,options);
    if (response.getResponseCode() == 200) {
      var birdValue = parseInt(response.getContentText(), 10); // birdの値を整数に変換
      var countValue = parseInt(response_2nd.getContentText(), 10); // countの値を整数に変換
      var tmpValue = parseInt(response_4th.getContentText(), 10); // temperatureの値を整数に変換
      var fishValue = response_3rd.getContentText();
      var is_locked = false;
      is_locked = fishValue === "true";
      Logger.log("Bird value: " + birdValue + " time:" +countValue + " temperature:" +tmpValue);

      // birdの値が2700より大きいか否かで分岐
      if (birdValue > 2700) {
        sendToDiscord("opn:" + birdValue + " time:" +countValue + " temp:"+tmpValue, webhookUrl);
        if(is_locked){
          sendToDiscord("opened",webhookUrl_denken);
          var payload = JSON.stringify(false); // 値をJSON形式に変換
          var op = {
            method: 'put',
            contentType: 'application/json',
            payload: payload,
             muteHttpExceptions: true
          };
          UrlFetchApp.fetch(databaseUrl_3,op);
        }
      } else {
        sendToDiscord("lk:" + birdValue + " time:" +countValue + " temp:"+tmpValue, webhookUrl);
        if(!is_locked){
          sendToDiscord("locked",webhookUrl_denken);
          var payload = JSON.stringify(true);
          var op = {
            method: 'put',
            contentType: 'application/json',
            payload: payload,
            muteHttpExceptions: true
          };
          UrlFetchApp.fetch(databaseUrl_3,op);
        }
        
        //sendLineNotification("lk(v<2700):" + birdValue + " time:" +countValue, token);
      }

    } else {
      Logger.log("Error fetching data: " + response.getResponseCode());
      Logger.log("Response: " + response.getContentText());
    }
  } catch (e) {
    Logger.log("Exception: " + e.message);
  }
}

// Discordにメッセージを送信する関数//
function sendToDiscord(message, webhookUrl) {
  var payload = JSON.stringify({ content: message });
  
  var options = {
    method: 'post',
    contentType: 'application/json',
    payload: payload
  };
  var response = UrlFetchApp.fetch(webhookUrl, options);
  Logger.log("Discord_ResponseCode:"+response.getResponseCode());
}

// LINEにメッセージを送信する関数//
function sendLineNotification(message,tkn) {
  

  var options = {
    'method': 'post',
    'headers': {
      'Authorization': 'Bearer ' + tkn
    },
    'payload': {
      'message': message
    }
  };
  var response = UrlFetchApp.fetch('https://notify-api.line.me/api/notify', options);
  Logger.log("LINE_response.getContent:"+response.getContentText());
}
