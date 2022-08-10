<!DOCTYPE html>
<html>
<head>
	<meta charset  = "UTF-8">
	<meta http-equiv="refresh" content = "5">
	<style type = "text/css">
		.spec{
			text-align:center;
		}
		.con{
			text-align:left;
		}
	</style>
</head>
<body>
	<h1 align = "center">My Database</h1>
	<div class = "spec">
		# <b>The sensor value description</b><br>
		# 1 ~ 99 humi <br>
		# 1 ~ 99 temp
	</div>

	<table border = '1' style = "width = 30%" align = "center">
	<tr align = "center">
		<th>ID</th>
		<th>DATE</th>
		<th>TIME</th>
		<th>Temperature</th>
		<th>Humidity</th>
	</tr>
	<?php
		$conn = mysqli_connect("localhost","root","kcci");
		mysqli_select_db($conn,"openWeather");
		$result = mysqli_query($conn, "select * from SeoulData");

		while($row = mysqli_fetch_array($result)){
			echo "<tr align = center>";
			echo '<td>'.$row['id'].'</td>';
			echo '<td>'.$row['date'].'</td>';
			echo '<td>'.$row['time'].'</td>';
			echo '<td>'.$row['SeoulTemp'].'</td>';
			echo '<td>'.$row['SeoulHumi'].'</td>';
			echo "</tr>";
			mysqli_close($conn);
		}
	?>
	</table>
</body>
</html>
