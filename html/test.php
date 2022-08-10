<?php
 $conn = mysqli_connect("localhost","root","kcci");
 mysqli_set_charset($conn,"utf8");
 mysqli_select_db($conn,"openWeather");
 $result = mysqli_query($conn, "select date, time, SeoulTemp, SeoulHumi from SeoulData");
 $data = array(array('openWeather','temp','humi'));
 if($result){
     while($row = mysqli_fetch_array($result)){
         array_push($data, array($row['date']."\n".$row[1], intval($row[2]),intval($row[3])));
     }
 }
 $options = array(
     'title' => 'Temperature(C),Humidity(%)',
     'width' => 1000, 'height' => 500
 );
 ?>


<html>
  <head>
  <script src="//www.google.com/jsapi"></script>
  <script>
      let data = <?= json_encode($data) ?>;
      let options = <?= json_encode($options) ?>;
      google.load('visualization', '1.0', {'packages':['corechart']});
      google.setOnLoadCallback(function() {
          let chart1 = new google.visualization.LineChart(document.querySelector('#Seoulchart_div'));
         chart1.draw(google.visualization.arrayToDataTable(data), options);
      google.setOnLoadCallback(function() {
          let chart2 = new google.visualization.LineChart(document.querySelector('#Daejeonchart_div'));
         chart2.draw(google.visualization.arrayToDataTable(data), options);


      });
  </script>

  </head>
  <body>
    <table>
      <tr>
        <td><div id="Seoulchart_div"></div></td>
        <td><div id="Daejeonchart_div"></div></td>
      </tr>
    </table>
  </body>
</html>
