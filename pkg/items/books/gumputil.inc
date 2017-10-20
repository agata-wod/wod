/////////////////////////////////////////////
//
//  Shinigami's GumpUtil v1.01 (2000/02/26)
//    (some nice Functions for Gumps)
//
//  Author:    Shinigami
//  eMail:     Shinigami@gmx.net
//  Languages: English/German
//
/////////////////////////////////////////////

/////////////////
// Installation
/////////////////
//
// Move it to /pol/scripts/include and use it in your script:
//   Include "include/gumputil";

//////////////
// Functions
//////////////
//
// GumpTextWidth(gumptext)           // Returns the Width of GumpText
// GumpTextWidthFast(gumptext)       // Returns the Width of GumpText
// GumpTextLeft(gumptext, x, width)  // Returns the X-Value (Left-Alignment)
// GumpTextRight(gumptext, x, width) // Returns the X-Value (Right-Alignment)
// GumpTextMid(gumptext, x, width)   // Returns the X-Value (Mid-Alignment)
//
// Parameter:
//   gumptext - a String
//   x        - X-Value from the Textbox
//   width    - Width from the Textbox

//////////////////////
// Original Location
//////////////////////
//
// www.onelist.com
//
//   OneList/pol-scriptforum/Includes/gumputil.zip

///////////////////////////////////////////////////////////////////////////////

//////////////
// Constants
//////////////

var length:=struct;
       length.+chars:={
         "",                // Dummy (for Speed...)
         "|",               //  2
         "`',.;:",          //  3
         "!",               //  4
         "il1[]",           //  5
         "bcejotîˆ(){}=",   //  6
         "adfpqrszIÑ‰~",    //  7
         "ghnuvyÅ¸ <>-",    //  8
         "xJ578$?ﬂ+",       //  9
         "kCE2369^/*",      // 10
         "mwABDFOPéƒô÷40%", // 11
         "GLNQSZ#",         // 12
         "HRT&_",           // 13
         "Kö‹UY",           // 14
         "X",               // 15
         "V",               // 16
         "M",               // 18
         "W"               // 21
       };
       length.+charsf:=   // Space as Filler is not allowed
         "|||||||||||||||"+ //  2
         "`',.;::::::::::"+ //  3
         "!!!!!!!!!!!!!!!"+ //  4
         "il1[]]]]]]]]]]]"+ //  5
         "bcejotîˆ(){}==="+ //  6
         "adfpqrszIÑ‰~~~~"+ //  7
         "ghnuvyÅ¸ <>----"+ //  8
         "xJ578$?ﬂ+++++++"+ //  9
         "kCE2369^/******"+ // 10
         "mwABDFOPéƒô÷40%"+ // 11
         "GLNQSZ#########"+ // 12
         "HRT&___________"+ // 13
         "Kö‹UYYYYYYYYYYY"+ // 14
         "XXXXXXXXXXXXXXX"+ // 15
         "VVVVVVVVVVVVVVV"+ // 16
         "MMMMMMMMMMMMMMM"+ // 18
         "WWWWWWWWWWWWWWW"; // 21
       length.+width := array {
          2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
          12, 13, 14, 15, 16, 18, 21
       };

//////////////
// Functions
//////////////

// Returns the Width of GumpText

Function GumpTextWidth(gumptext)
  var start_i:=len(length.chars);
  var points:=0;
  var j;

  For (j:=len(gumptext); j>0; j:=j-1)
    var i:=start_i;
    While ((i>0) And (!length.chars[i][CStr(gumptext[j])]))
      i:=i-1;
    EndWhile;
    If (i>0) // siehe Speed-Dummy
      points:=points+length.width[i-1];
    Else
      SysLog("GumpTextWidth: Unknown Character = '"+gumptext[j]+"'");
    EndIf
  EndFor

  Return (points);
EndFunction

// Returns the Width of GumpText

Function GumpTextWidthFast(gumptext)
  var points:=0;
  var j;

  For (j:=len(gumptext); j>0; j:=j-1)
    var i:=Find(length.charsf, gumptext[j], 1)-1;
    If (i>=0)
      points:=points+length.width[CInt(i/15)+1];
    Else
      SysLog("GumpTextWidthFast: Unknown Character = '"+gumptext[j]+"'");
    EndIf
  EndFor

  Return (points);
EndFunction

// Returns the X-Value (Left-Alignment)

Function GumpTextLeft(gumptext, x, width)
  Return (x);
EndFunction

// Returns the X-Value (Right-Alignment)

Function GumpTextRight(gumptext, x, width)
  Return (x+(width-GumpTextWidthFast(gumptext)));
EndFunction

// Returns the X-Value (Mid-Alignment)

Function GumpTextMid(gumptext, x, width)
  Return (x+CInt((width-GumpTextWidthFast(gumptext))/2));
EndFunction
