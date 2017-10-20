<%
use uo;
use os;
%>

<html>
<head>
<title>Log of Long Running Scripts</title>
</head>

<body>
<% 
  include header; 
  PrintHeader( "Long-running Scripts" );
%>

<div align=center>
<table border=1 cellspacing=0 cellpadding=5>
  <tr bgcolor=#99CCFF>
    <td>Script Name</td>
    <td>Cycles</td>
	<!--<td>Sleep Cycles</td>-->
    <td>Cycles since Sleep</td>
    <td>Prog Counter</td>
	<td>Call Depth</td>
	<td>Globals</td>
  </tr>

<% for i := 1 to 100 %>
<%
  foreach script in (polcore().running_scripts)
    if (script.consec_cycles >= 20)
%>
  <tr>
    <td><%=script.name%></td>
	<td><%=script.instr_cycles%></td>
	<!--<td><%=script.sleep_cycles%></td>-->
    <td><%=script.consec_cycles%></td>
	<td><%=script.PC%></td>
	<td><%=script.call_depth%></td>
	<td><%=script.num_globals%></td>
  </tr>
<%
    endif
  endforeach
sleepms (1);
endfor
%>


</table>
</div>

</BODY>
</HTML>
