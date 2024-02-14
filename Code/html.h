const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Tidenanzeiger</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>

<h1 style="text-align:center">Tidenanzeiger</h1>

<hr />
<table align="center" border="1" cellpadding="1" cellspacing="1" style="width:500px">
	<caption><strong>Status</strong></caption>
	<tbody>
		<tr>
			<td>Version&nbsp;:</td>
			<td>%VER%</td>
		</tr>
		<tr>
			<td>Wasserh&ouml;he:</td>
			<td>%VALUE% cm</td>
		</tr>
		<tr>
			<td>Ebbe/Flut:</td>
			<td>%TREND%</td>
		</tr>
		<tr>
			<td>Letzte Aktualisierung:</td>
			<td>%TIMESTAMP%</td>
		</tr>
    <tr>
			<td>RSSI:</td>
			<td>%RSSI% dB</td>
		</tr>
	</tbody>
</table>
<hr />
<table align="center" border="1" cellpadding="1" cellspacing="1" style="height:28px; width:495px">
	<caption><strong>Update</strong></caption>
	<tbody>
		<tr>
			<td style="text-align:center">
			<p><strong><a href="http://192.168.0.15/update"><span style="color:#ffffff"><span style="background-color:#e74c3c">Update starten</span></span></a></strong></p>
			</td>
		</tr>
	</tbody>
</table>
</body></html>)rawliteral";
