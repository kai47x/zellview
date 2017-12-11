
MODULE SpokeLength3;

  IMPORT 
        RTS,
	java_net, 		(* java.net *)
	java_awt, 		(* java.awt *)
	java_lang,		(* java.lang *)
	java_applet,		(* java.applet *)
	java_awt_event,		(* java.awt.event *)
	java_awt_image;		(* java.awt.image *)


(* ==================================================================== *)

  TYPE	Message   = ARRAY 80 OF CHAR;

(* ==================================================================== *)

  TYPE
	WheelDims* = RECORD
                      isFront  : BOOLEAN;
                      hasAngle : BOOLEAN;
		      rimDiam  : REAL;
                      offset   : REAL;
		      nipProt  : REAL;
		      nipLeng  : REAL;
                      rimERD   : REAL;
		      spkNumND : INTEGER;
		      spkNumDS : INTEGER;
		      crsNumND : INTEGER;
		      crsNumDS : INTEGER;
                      spkAngDS : REAL;
                      spkAngND : REAL;
		      flgDiaDS : REAL;
		      flgDiaND : REAL;
		      flgOfsND : REAL;
		      flgOfsDS : REAL;
		    END;

(* ==================================================================== *)

  TYPE
	Canvas*     = POINTER TO RECORD
			(java_awt.Canvas)
			disp  : java_awt.Rectangle;
			imag  : java_awt.Image;     (* Current image   *)
			imgFC : java_awt.Image;     (* Front for cross *)
			imgFA : java_awt.Image;     (* Front for angle *)
			imgRC : java_awt.Image;     (* Rear for cross  *)
			imgRA : java_awt.Image;     (* Rear for angle  *)
			warn  : Message;
			rslt1 : Message;
			rslt2 : Message;
			tens1 : Message;
			tens2 : Message;
			dims  : WheelDims;
			theFont : java_awt.Font;
		      END;

(* ==================================================================== *)

  TYPE
       RadioPair*    = POINTER TO RECORD
			(java_awt.Container + java_awt_event.ItemListener)
                         parent : Controls;
                         yesBox : java_awt.Checkbox; 
                         noBox  : java_awt.Checkbox;
                       END;
                         
(* ==================================================================== *)

  TYPE
       LabelledText  = POINTER TO RECORD
			(java_awt.Container)
			 label  : java_awt.Label;
			 txtFld : java_awt.TextField;
                       END;

(* ==================================================================== *)

  TYPE
	Controls*    = POINTER TO RECORD
			(java_awt.Panel + java_awt_event.ActionListener) 
			canvas  : Canvas;
                        applet  : Applet;
                       (*
                        * First row ...
                        *   Compute-button, <blank>, <blank>, <blank>
                        *) 
                        button  : java_awt.Button;
                       (*
                        * Second row ...
                        *   Wheel type label, front/rear select,
                        *   Diam. type label, ERD/inside select
                        *) 
			wheelLab  : java_awt.Label;
                        frontRear : RadioPair;
			diamLab   : java_awt.Label;
                        insideERD : RadioPair;
                       (*
                        * Third row ...
                        *   Specify spoke label, cross/angle select
                        *   Offset rim label, yes/no select
                        *) 
			spokeLab   : java_awt.Label;
                        crossAngle : RadioPair;
			offsetLab  : java_awt.Label;
                        offstYesNo : RadioPair;
                       (*
                        * Fourth row ...
                        *   Rim diam, NippleLength, Protrusion, Rim offset
                        *) 
			rimLabT : LabelledText;
			rimDTxt : java_awt.TextField;
			nipLabT : LabelledText;
			nipLTxt : java_awt.TextField;
			proLabT : LabelledText;
			proLTxt : java_awt.TextField;
			offLabT : LabelledText;
			offLTxt : java_awt.TextField;
                       (*
                        * Fifth row ...
                        *   Flange offset, Flange PCD, Spoke#, SpokeInfo
                        *) 
			fOffLabND : LabelledText;
			fOffTxtND : java_awt.TextField;
			pcdLabND  : LabelledText;
			pcdTxtND  : java_awt.TextField;
			numLabND  : LabelledText;
			numTxtND  : java_awt.TextField;
			infoLabND : LabelledText;
			infoTxtND : java_awt.TextField;
                       (*
                        * Sixth row ...
                        *   Flange offset, Flange PCD, Spoke#, SpokeInfo
                        *) 
			fOffLabDS : LabelledText;
			fOffTxtDS : java_awt.TextField;
			pcdLabDS  : LabelledText;
			pcdTxtDS  : java_awt.TextField;
			numLabDS  : LabelledText;
			numTxtDS  : java_awt.TextField;
			infoLabDS : LabelledText;
			infoTxtDS : java_awt.TextField;
		      END;

(* ==================================================================== *)

  TYPE
	Applet*     = POINTER TO RECORD
			(java_applet.Applet)
			controls : Controls;
		      END;

(* ==================================================================== *)

  CONST
        format      = "Invalid number format in ";


(* ==================================================================== *)
(* 			Some static helper procedures			*)
(* ==================================================================== *)

  PROCEDURE MkButton(IN s : ARRAY OF CHAR) : java_awt.Button;
  BEGIN
    RETURN java_awt.Button.Init(MKSTR(s));
  END MkButton;

  PROCEDURE MkLabel(IN s : ARRAY OF CHAR; j : INTEGER) : java_awt.Label;
  BEGIN
    RETURN java_awt.Label.Init(MKSTR(s$), j);
  END MkLabel;

  PROCEDURE MkTextField(IN s : ARRAY OF CHAR; j : INTEGER) : java_awt.TextField;
  BEGIN
    RETURN java_awt.TextField.Init(MKSTR(s), j);
  END MkTextField;

  PROCEDURE RoundToOneDigit(num : REAL; OUT res : Message);
    VAR str : java_lang.String;
	buf : java_lang.StringBuffer;
  BEGIN
    str := java_lang.String.valueOf(SHORT(ENTIER(num * 10.0 + 0.5)));
    buf := java_lang.StringBuffer.Init(str);
    buf := buf.insert(buf.length()-1, CHR(ORD(".")));
    buf := buf.append(CHR(0));
    buf.getChars(0, buf.length(), res, 0);
  END RoundToOneDigit;

  PROCEDURE RoundToInteger(num : REAL; OUT res : Message);
    VAR str : java_lang.String;
	buf : java_lang.StringBuffer;
  BEGIN
    str := java_lang.String.valueOf(SHORT(ENTIER(num + 0.5)));
    buf := java_lang.StringBuffer.Init(str);
    buf := buf.append(CHR(0));
    buf.getChars(0, buf.length(), res, 0);
  END RoundToInteger;

(* ==================================================================== *)

  PROCEDURE ClearMessages(c : Canvas);
  BEGIN
    c.warn[0]  := 0X;
    c.rslt1[0] := 0X;
    c.rslt2[0] := 0X;
    c.tens1[0] := 0X;
    c.tens2[0] := 0X;
  END ClearMessages;

(* ==================================================================== *)

  PROCEDURE (IN d : WheelDims)ValidateFrontDims(OUT msg : Message),NEW;
   (* ---------------------- *)
    PROCEDURE MkMsg(IN lh, rh : ARRAY OF CHAR; OUT msg : Message);
    BEGIN
      msg := lh + rh;
    END MkMsg;
   (* ---------------------- *)
  BEGIN (* Check for bad format input *)
    IF d.rimDiam < 0.0 THEN
      MkMsg(format, "rim diameter", msg);
    ELSIF d.flgDiaND < 0.0 THEN
      MkMsg(format, "flange diameter", msg);
    ELSIF d.spkNumND = -1 THEN
      MkMsg(format, "spoke number", msg);
    ELSIF ~d.hasAngle & (d.crsNumND = -1) THEN
      MkMsg(format, "spoke cross-number", msg);
    ELSIF d.flgOfsND = -1 THEN
      MkMsg(format, "flange offset length", msg);
    ELSIF d.nipProt = -1 THEN
      MkMsg(format, "nipple protrusion", msg);
    ELSIF d.nipLeng = -1 THEN
      MkMsg(format, "nipple overall length", msg);
    ELSIF d.hasAngle & (d.spkAngND < 0.0) THEN
      MkMsg(format, "angle specification", msg);

    ELSIF d.nipProt > d.nipLeng THEN
      msg := "Nipple cannot protrude more than overall length";
    ELSIF d.spkNumND MOD 2 # 0 THEN
      msg := "Number of spokes must be even";
    ELSIF (d.spkNumND MOD 4 # 0) & (d.crsNumND # 0) THEN
      msg := "Spoke-num must divide by four unless spoking is radial";
    ELSIF (d.rimDiam > 620) OR (d.rimDiam < 450) THEN
      msg := "Unusual rim diameter: are you sure?";
    ELSIF (d.flgDiaND > 70) OR (d.flgDiaND < 25) THEN
      msg := "Unusual flange diameter: are you sure?";
    ELSIF (d.flgOfsND > 40) OR (d.flgOfsND < 30) THEN
      msg := "Unusual flange offset: are you sure?";
    ELSIF ~d.hasAngle & (d.crsNumND > d.spkNumND DIV 8) THEN
      msg := "Crossings too high: worse than tangential";
    ELSIF d.hasAngle & (d.spkAngND > 85.0) THEN
      msg := "Spoke angle too high: close to tangential";
    ELSIF (d.nipLeng = 0.0) & (d.nipProt = 0.0) THEN
      msg := "";	(* text fields must be disabled *)
    ELSIF (d.nipLeng > 16) OR (d.nipLeng < 8) THEN
      msg := "Unusual nipple length: are you sure?";
    ELSIF d.nipProt < 5 THEN
      msg := "Very short nipple protrusion: are you sure?";
    ELSE
      msg := "";
    END;
  END ValidateFrontDims;

(* ==================================================================== *)

  PROCEDURE (IN d : WheelDims)ValidateRearDims(OUT msg : Message),NEW;
   (* ---------------------- *)
    PROCEDURE MkMsg(IN lh, rh : ARRAY OF CHAR; OUT msg : Message);
    BEGIN
      msg := lh + rh;
    END MkMsg;
   (* ---------------------- *)
  BEGIN (* Check for bad format input *)
    IF d.rimDiam < 0.0 THEN
      MkMsg(format, "rim diameter", msg);
    ELSIF d.flgDiaND < 0.0 THEN
      MkMsg(format, "ND flange diameter", msg);
    ELSIF d.flgDiaDS < 0.0 THEN
      MkMsg(format, "DS flange diameter", msg);
    ELSIF d.spkNumND = -1 THEN
      MkMsg(format, "ND flange diameter", msg);
    ELSIF d.spkNumDS = -1 THEN
      MkMsg(format, "DS flange diameter", msg);
    ELSIF ~d.hasAngle & (d.crsNumND = -1) THEN
      MkMsg(format, "ND cross-number", msg);
    ELSIF ~d.hasAngle & (d.crsNumDS = -1) THEN
      MkMsg(format, "DS cross-number", msg);
    ELSIF d.flgOfsND = -1 THEN
      MkMsg(format, "ND flange-offset length", msg);
    ELSIF d.flgOfsDS = -1 THEN
      MkMsg(format, "DS flange-offset length", msg);
    ELSIF d.nipProt = -1 THEN
      MkMsg(format, "nipple protrusion length", msg);
    ELSIF d.nipLeng = -1 THEN
      MkMsg(format, "nipple overall length", msg);
    ELSIF d.hasAngle & (d.spkAngND < 0.0) THEN
      MkMsg(format, "ND angle specification", msg);
    ELSIF d.hasAngle & (d.spkAngDS < 0.0) THEN
      MkMsg(format, "DS angle specification", msg);
    ELSIF d.offset < 0.0 THEN
      MkMsg(format, "rim offset distance", msg);
    ELSIF d.offset > 8.0 THEN
      msg := "Offset value seems too high";

    ELSIF d.nipProt > d.nipLeng THEN
      msg := "Nipple cannot protrude more than overall length";
    ELSIF (ODD(d.spkNumND)) & (d.crsNumND # 0) THEN
      msg := "ND Spoke-num must be even unless spoking is radial";

    ELSIF d.hasAngle & (d.spkAngDS <= 40.0) THEN
      msg := "Drive-side spokes too close to radial";
    ELSIF ~d.hasAngle & (d.crsNumDS < 2) THEN
      msg := "Drive-side spokes too close to radial";
    ELSIF (d.rimDiam > 620) OR (d.rimDiam < 450) THEN
      msg := "Unusual rim diameter: are you sure?";

    ELSIF (d.flgDiaND > 70) OR (d.flgDiaND < 25) THEN
      msg := "Unusual ND flange diameter: are you sure?";
    ELSIF (d.flgDiaDS > 70) OR (d.flgDiaDS < 25) THEN
      msg := "Unusual DS flange diameter: are you sure?";

    ELSIF (d.flgOfsND > 40) OR (d.flgOfsND < 30) THEN
      msg := "Unusual ND flange offset: are you sure?";
    ELSIF (d.flgOfsDS > 40) OR (d.flgOfsDS < 17) THEN
      msg := "Unusual DS flange offset: are you sure?";

    ELSIF ~d.hasAngle & (d.crsNumND > d.spkNumND DIV 4) THEN
      msg := "ND Crossings too high: worse than tangential";
    ELSIF d.hasAngle & (d.spkAngND > 85.0) THEN
      msg := "ND Spoke angle too high: close to tangential";

    ELSIF ~d.hasAngle & (d.crsNumDS > d.spkNumDS DIV 4) THEN
      msg := "DS Crossings too high: worse than tangential";
    ELSIF d.hasAngle & (d.spkAngDS > 85.0) THEN
      msg := "DS Spoke angle too high: close to tangential";

    ELSIF (d.nipLeng = 0.0) & (d.nipProt = 0.0) THEN
      msg := "";	(* text fields must be disabled *)
    ELSIF (d.nipLeng > 16) OR (d.nipLeng < 8) THEN
      msg := "Unusual nipple length: are you sure?";
    ELSIF d.nipProt < 5 THEN
      msg := "Very short nipple protrusion: are you sure?";
    ELSE
      msg := "";
    END;
  END ValidateRearDims;

(* ==================================================================== *)

  PROCEDURE (c : Canvas)AssignImage(    app : Applet;
                                    VAR img : java_awt.Image;
                                    IN  nam : ARRAY OF CHAR), NEW;
  BEGIN
   (*
    *  Demand load the image, and assign to c.imag
    *)
    IF img = NIL THEN img := app.getImage(app.getCodeBase(), MKSTR(nam)) END;
    c.imag := img;
  END AssignImage;

(* ==================================================================== *)

  PROCEDURE (c : Canvas)paint*(g : java_awt.Graphics);
    VAR j : BOOLEAN;
	m : Message;
        w : INTEGER;
        h : INTEGER;
        o : java_awt_image.ImageObserver;
  BEGIN
    c.disp := c.getBounds();
    g.setColor(java_awt.Color.white);
    g.fillRect(c.disp.x, c.disp.y, c.disp.width, c.disp.height);
    o := c(java_awt_image.ImageObserver);

    h := c.imag.getHeight(o);
    w := c.imag.getWidth(o);
    j := g.drawImage(c.imag, (c.disp.width - w) DIV 2, 0, o);
    g.setFont(c.theFont);
    IF c.warn[0] # 0X THEN
      g.setColor(java_awt.Color.red);
      m := "WARNING: " + c.warn;
      g.drawString(MKSTR(m), 20, 310);
    END;
    IF c.rslt1[0] # 0X THEN
      g.setColor(java_awt.Color.black);
      IF c.rslt2[0] = 0X THEN
        m := "SPOKE LENGTH: " + c.rslt1 +
                           "mm, TENSION: " + c.tens1 + "kg";
        g.drawString(MKSTR(m), 50, 360);
      ELSE
        m := "NON DRIVE SPOKE LENGTH: " + c.rslt1 + 
                           "mm, TENSION: " + c.tens1 + "kg";
        g.drawString(MKSTR(m), 50, 360);
        m := "DRIVE SIDE SPOKE LENGTH: " + c.rslt2 + 
                           "mm, TENSION: " + c.tens2 + "kg";
        g.drawString(MKSTR(m), 50, 376);
      END;
    END;
  END paint;

(* ==================================================================== *)

  PROCEDURE (c : Canvas)compute(),NEW;
    VAR efctv, 
        thetaND, thetaDS, 
        xDiffND, xDiffDS,
        yDiffND, yDiffDS,
        sLengND, sLengDS : REAL;
        sTensND, sTensDS : REAL;
  BEGIN
    ClearMessages(c);
    IF c.dims.isFront THEN 
      c.dims.ValidateFrontDims(c.warn);
    ELSE
      c.dims.ValidateRearDims(c.warn);
    END;
    efctv := c.dims.rimDiam + 2.0 * (c.dims.nipLeng - c.dims.nipProt);

    IF c.dims.offset > 0.0 THEN
      c.dims.flgOfsND := c.dims.flgOfsND - c.dims.offset;    
      c.dims.flgOfsDS := c.dims.flgOfsDS + c.dims.offset;    
    END;

    IF c.dims.hasAngle THEN
      thetaND := 2.0 * java_lang.Math.PI * c.dims.spkAngND / 360.0; 
    ELSIF c.dims.isFront THEN
      thetaND := 4.0 * java_lang.Math.PI * c.dims.crsNumND / c.dims.spkNumND;
    ELSE
      thetaND := 2.0 * java_lang.Math.PI * c.dims.crsNumND / c.dims.spkNumND;
    END;
    xDiffND := (efctv - c.dims.flgDiaND * java_lang.Math.cos(thetaND)) / 2.0;
    yDiffND := c.dims.flgDiaND * java_lang.Math.sin(thetaND) / 2.0;
    sLengND := java_lang.Math.sqrt(c.dims.flgOfsND * c.dims.flgOfsND +
				xDiffND * xDiffND + yDiffND * yDiffND);
    sLengND := sLengND - 1.0; (* adjust for spoke diameter = 2mm *)
    RoundToOneDigit(sLengND, c.rslt1);
   (*
    *  formula for spoke tension for front wheels ...
    *     tND = 200.0 * sLengND / spkNumND * flgOfsND;
    *  formula for spoke tension for rear wheels ...
    *     tND = 100.0 * sLengND / spkNumND * flgOfsND;
    *     tDS = 100.0 * sLengDS / spkNumDS * flgOfsDS;
    *  Adapted from Charles Coin's formula ... (with corrections)
    *)
    IF c.dims.isFront THEN
      sTensND := (200.0 * sLengND) / (c.dims.spkNumND * c.dims.flgOfsND);
      RoundToInteger(sTensND, c.tens1);
    ELSE (* rear wheel *)
      IF c.dims.hasAngle THEN
        thetaDS := 2.0 * java_lang.Math.PI * c.dims.spkAngDS / 360.0; 
      ELSE
        thetaDS := 2.0 * java_lang.Math.PI * c.dims.crsNumDS / c.dims.spkNumDS;
      END;

      xDiffDS := (efctv - c.dims.flgDiaDS * java_lang.Math.cos(thetaDS)) / 2.0;
      yDiffDS := c.dims.flgDiaDS * java_lang.Math.sin(thetaDS) / 2.0;
      sLengDS := java_lang.Math.sqrt(c.dims.flgOfsDS * c.dims.flgOfsDS +
				xDiffDS * xDiffDS + yDiffDS * yDiffDS);
      sLengDS := sLengDS - 1.0; (* adjust for spoke diameter = 2mm *)
      RoundToOneDigit(sLengDS, c.rslt2);

      sTensND := (100.0 * sLengND) / (c.dims.spkNumND * c.dims.flgOfsND);
      RoundToInteger(sTensND, c.tens1);
      sTensDS := (100.0 * sLengDS) / (c.dims.spkNumDS * c.dims.flgOfsDS);
      RoundToInteger(sTensDS, c.tens2);
    END;
    c.repaint();
  END compute;

(* ==================================================================== *)

  PROCEDURE MkRadioPair(IN l,r : ARRAY OF CHAR;
                        parent : Controls;
                        leftOn : BOOLEAN) : RadioPair;
    VAR pair : RadioPair;
  BEGIN
    NEW(pair);
    pair.setLayout(java_awt.GridLayout.Init(1,2)(java_awt.LayoutManager));
    pair.parent := parent;
    pair.yesBox := java_awt.Checkbox.Init(MKSTR(l), leftOn);
    pair.noBox  := java_awt.Checkbox.Init(MKSTR(r), ~leftOn);
    pair.add(pair.yesBox, NIL);
    pair.add(pair.noBox, NIL);
    pair.yesBox.addItemListener(pair(java_awt_event.ItemListener));
    pair.noBox.addItemListener(pair(java_awt_event.ItemListener));
    RETURN pair;
  END MkRadioPair;

(* ==================================================================== *)

  PROCEDURE MkLabelledText(IN lab : ARRAY OF CHAR;
                           txt    : java_awt.TextField) : LabelledText;
    VAR text : LabelledText;
  BEGIN
    NEW(text);
    text.setLayout(java_awt.GridLayout.Init(1,2)(java_awt.LayoutManager));
    text.label := MkLabel(MKSTR(lab), java_awt.Label.RIGHT);
    text.txtFld := txt;
    text.add(text.label, NIL);
    text.add(text.txtFld, NIL);
    RETURN text;
  END MkLabelledText;

(* ==================================================================== *)

  PROCEDURE (p : Controls)initialize(canvas : Canvas),NEW;
  BEGIN
    p.canvas := canvas;
    p.setLayout(java_awt.GridLayout.Init(6,4)(java_awt.LayoutManager));

   (* ====================== ROW ONE   ====================== *)

    p.button := MkButton("COMPUTE-LENGTH");
    p.button.setBackground(java_awt.Color.green);
    p.add(p.button, NIL);
    p.add(MkLabel("(All measurements in mm)", java_awt.Label.CENTER), NIL);
    p.add(MkLabel("('ND' means Non-drive side)", java_awt.Label.CENTER), NIL);
    p.add(MkLabel("('DS' means Drive-side)", java_awt.Label.CENTER), NIL);

   (* ====================== ROW TWO   ====================== *)

    p.wheelLab := MkLabel("Wheel type ", java_awt.Label.RIGHT);
    p.frontRear := MkRadioPair("front", "rear", p, TRUE);

    p.diamLab  := MkLabel("Rim measurement ", java_awt.Label.RIGHT);
    p.insideERD := MkRadioPair("inside", "ERD", p, FALSE);

    p.add(p.wheelLab, NIL);
    p.add(p.frontRear, NIL);

    p.add(p.diamLab, NIL);
    p.add(p.insideERD, NIL);

   (* ====================== ROW THREE ====================== *)

    p.spokeLab := MkLabel("Specify spoke ", java_awt.Label.RIGHT);
    p.crossAngle := MkRadioPair("cross", "angle", p, TRUE);

    p.offsetLab  := MkLabel("Offset Rim? ", java_awt.Label.RIGHT);
    p.offstYesNo := MkRadioPair("yes", "no", p, FALSE);

    p.offsetLab.setEnabled(FALSE);
    p.offstYesNo.setEnabled(FALSE);

    p.add(p.spokeLab, NIL);
    p.add(p.crossAngle, NIL);
    p.add(p.offsetLab, NIL);
    p.add(p.offstYesNo, NIL);

   (* ====================== ROW FOUR  ====================== *)

    p.rimDTxt := MkTextField("600",6);
    p.rimLabT := MkLabelledText("Rim diam. ", p.rimDTxt);

    p.nipLTxt := MkTextField("12",6);
    p.nipLabT := MkLabelledText("Nipple len. ", p.nipLTxt);

    p.proLTxt := MkTextField("8",6);
    p.proLabT := MkLabelledText("Nipple pro. ", p.proLTxt);

    p.offLTxt := MkTextField("0",6);
    p.offLabT := MkLabelledText("Offset ", p.offLTxt);

    p.nipLabT.setEnabled(FALSE);
    p.proLabT.setEnabled(FALSE);
    p.offLabT.setEnabled(FALSE);

    p.add(p.rimLabT, NIL);
    p.add(p.nipLabT, NIL);
    p.add(p.proLabT, NIL);
    p.add(p.offLabT, NIL);

   (* ====================== ROW FIVE  ====================== *)

    p.fOffTxtND := MkTextField("35",6);
    p.fOffLabND := MkLabelledText("Flange offset ", p.fOffTxtND);

    p.pcdTxtND := MkTextField("38",6);
    p.pcdLabND := MkLabelledText("Flange PCD ", p.pcdTxtND);

    p.numTxtND := MkTextField("32",6);
    p.numLabND := MkLabelledText("Total spokes ", p.numTxtND);

    p.infoTxtND := MkTextField("3",6);
    p.infoLabND := MkLabelledText("Crossings ", p.infoTxtND);

    p.add(p.fOffLabND, NIL);
    p.add(p.pcdLabND, NIL);
    p.add(p.numLabND, NIL);
    p.add(p.infoLabND, NIL);

   (* ====================== ROW SIX   ====================== *)

    p.fOffTxtDS := MkTextField("17",6);
    p.fOffLabDS := MkLabelledText("DS Flange ofst ", p.fOffTxtDS);

    p.pcdTxtDS := MkTextField("38",6);
    p.pcdLabDS := MkLabelledText("DS Flange PCD ", p.pcdTxtDS);

    p.numTxtDS := MkTextField("16",6);
    p.numLabDS := MkLabelledText("DS Spoke num. ", p.numTxtDS);

    p.infoTxtDS := MkTextField("3",6);
    p.infoLabDS := MkLabelledText("DS Crossings ", p.infoTxtDS);

    p.fOffLabDS.setEnabled(FALSE);
    p.pcdLabDS.setEnabled(FALSE);
    p.numLabDS.setEnabled(FALSE);
    p.infoLabDS.setEnabled(FALSE);

    p.add(p.fOffLabDS, NIL);
    p.add(p.pcdLabDS, NIL);
    p.add(p.numLabDS, NIL);
    p.add(p.infoLabDS, NIL);

    ClearMessages(p.canvas);
   (* ======================================================= *)
    p.button.addActionListener(p(java_awt_event.ActionListener));
   (* ======================================================= *)
  END initialize;

(* ==================================================================== *)

  PROCEDURE (p : Controls)actionPerformed*(e : java_awt_event.ActionEvent),NEW;
    VAR isOk : BOOLEAN;
        frnt : BOOLEAN;
        ngle : BOOLEAN;
        ofst : BOOLEAN;
  BEGIN
   (*
    *  The compute button has been pushed.
    *  Read in the text fields, convert to numerical form, as required.
    *  Catch format errors, and finally call "compute()".
    *)
    frnt := p.frontRear.yesBox.getState();
    ngle := p.crossAngle.noBox.getState();
    ofst := p.offstYesNo.yesBox.getState();
    p.canvas.dims.isFront := frnt;
    p.canvas.dims.hasAngle := ngle;

    RTS.StrToReal(p.rimDTxt.getText(), p.canvas.dims.rimDiam, isOk);
    IF ~isOk THEN p.canvas.dims.rimDiam := -1.0 END;

    RTS.StrToReal(p.pcdTxtND.getText(), p.canvas.dims.flgDiaND, isOk);
    IF ~isOk THEN p.canvas.dims.flgDiaND := -1.0 END;
    RTS.StrToReal(p.fOffTxtND.getText(), p.canvas.dims.flgOfsND, isOk);
    IF ~isOk THEN p.canvas.dims.flgOfsND := -1.0 END;
    RTS.StrToInt(p.numTxtND.getText(), p.canvas.dims.spkNumND, isOk);
    IF ~isOk THEN p.canvas.dims.spkNumND := -1 END;

    IF ofst THEN
      RTS.StrToReal(p.offLTxt.getText(), p.canvas.dims.offset, isOk);
      IF ~isOk THEN p.canvas.dims.offset := -1.0 END;
    ELSE
      p.canvas.dims.offset := 0.0;
    END;

    IF ~frnt THEN
      RTS.StrToReal(p.pcdTxtDS.getText(), p.canvas.dims.flgDiaDS, isOk);
      IF ~isOk THEN p.canvas.dims.flgDiaDS := -1.0 END;
      RTS.StrToReal(p.fOffTxtDS.getText(), p.canvas.dims.flgOfsDS, isOk);
      IF ~isOk THEN p.canvas.dims.flgOfsDS := -1.0 END;
      RTS.StrToInt(p.numTxtDS.getText(), p.canvas.dims.spkNumDS, isOk);
      IF ~isOk THEN p.canvas.dims.spkNumDS := -1 END;
    END;

    IF p.nipLTxt.isEnabled() THEN
      RTS.StrToReal(p.proLTxt.getText(), p.canvas.dims.nipProt, isOk);
      IF ~isOk THEN p.canvas.dims.nipProt := -1.0 END;
      RTS.StrToReal(p.nipLTxt.getText(), p.canvas.dims.nipLeng, isOk);
      IF ~isOk THEN p.canvas.dims.nipLeng := -1.0 END;
    ELSE
      p.canvas.dims.nipProt := 0.0;
      p.canvas.dims.nipLeng := 0.0;
    END;

    IF ngle THEN
      RTS.StrToReal(p.infoTxtND.getText(), p.canvas.dims.spkAngND, isOk);
      IF ~isOk THEN p.canvas.dims.spkAngND := -1.0 END;
      IF ~frnt THEN
        RTS.StrToReal(p.infoTxtDS.getText(), p.canvas.dims.spkAngDS, isOk);
        IF ~isOk THEN p.canvas.dims.spkAngDS := -1.0 END;
      END;
    ELSE
      RTS.StrToInt(p.infoTxtND.getText(), p.canvas.dims.crsNumND, isOk);
      IF ~isOk THEN p.canvas.dims.crsNumND := -1 END;
      IF ~frnt THEN
        RTS.StrToInt(p.infoTxtDS.getText(), p.canvas.dims.crsNumDS, isOk);
        IF ~isOk THEN p.canvas.dims.crsNumDS := -1 END;
      END;
    END;

    p.canvas.compute(); 
  END actionPerformed;

(* ==================================================================== *)
  PROCEDURE^ (p : LabelledText)newText(IN s : ARRAY OF CHAR),NEW;
(* ==================================================================== *)

  PROCEDURE (p : Controls)Notify*(s : java_lang.String),NEW; 
    VAR nOn : BOOLEAN;
  BEGIN
   (*
    *  A radio button item state has changed, and the 
    *  button-handler has called "Notify()" with its label
    *  as argument. 
    *  We match the label name, then take the appropriate
    *  action to update the state of the applet.  In some
    *  cases we must call repaint to display a new image.
    *)
    ClearMessages(p.canvas);
    IF    s.equals(MKSTR("front")) THEN
      p.offsetLab.setEnabled(FALSE);
      p.offstYesNo.setEnabled(FALSE);

      p.fOffLabDS.setEnabled(FALSE);
      p.pcdLabDS.setEnabled(FALSE);
      p.numLabDS.setEnabled(FALSE);
      p.infoLabDS.setEnabled(FALSE);

      p.fOffLabND.newText("Flange offset ");
      p.pcdLabND.newText("Flange PCD ");
      p.numLabND.newText("Total spokes ");
      p.numLabND.txtFld.setText("32");

      IF p.crossAngle.yesBox.getState() THEN
        p.infoLabND.newText("Crossings ");
        p.infoLabND.txtFld.setText("3");
        p.canvas.AssignImage(p.applet, p.canvas.imgFC, "spokefront.gif");
      ELSE
        p.infoLabND.newText("Angle (degrees) ");
        p.infoLabND.txtFld.setText("67.5");
        p.canvas.AssignImage(p.applet, p.canvas.imgFA, "spokeanglefront.gif");
      END;

      p.offLabT.setEnabled(FALSE);

    ELSIF s.equals(MKSTR("rear")) THEN
      p.offsetLab.setEnabled(TRUE);
      p.offstYesNo.setEnabled(TRUE);

      p.fOffLabDS.setEnabled(TRUE);
      p.pcdLabDS.setEnabled(TRUE);
      p.numLabDS.setEnabled(TRUE);
      p.infoLabDS.setEnabled(TRUE);

      p.fOffLabND.newText("ND Flange ofst ");
      p.pcdLabND.newText("ND Flange PCD ");
      p.numLabND.newText("ND spoke num ");
      p.numLabND.txtFld.setText("16");

      IF p.crossAngle.yesBox.getState() THEN
        p.infoLabND.newText("ND Crossings ");
        p.infoLabND.txtFld.setText("3");
        p.canvas.AssignImage(p.applet, p.canvas.imgRC, "spokerear.gif");
      ELSE
        p.infoLabND.newText("Angle (degrees) ");
        p.infoLabND.txtFld.setText("67.5");
        p.canvas.AssignImage(p.applet, p.canvas.imgRA, "spokeanglerear.gif");
      END;

      p.offLabT.setEnabled(p.offstYesNo.yesBox.getState());

    ELSIF s.equals(MKSTR("inside")) THEN
      p.nipLabT.setEnabled(TRUE);
      p.proLabT.setEnabled(TRUE);

    ELSIF s.equals(MKSTR("ERD")) THEN
      p.nipLabT.setEnabled(FALSE);
      p.proLabT.setEnabled(FALSE);

    ELSIF s.equals(MKSTR("cross")) THEN
      IF p.frontRear.yesBox.getState() THEN
        p.infoLabND.newText("Crossings ");
        p.canvas.AssignImage(p.applet, p.canvas.imgFC, "spokefront.gif");
      ELSE
        p.infoLabND.newText("ND Crossings ");
        p.canvas.AssignImage(p.applet, p.canvas.imgRC, "spokerear.gif");
      END;
      p.infoLabDS.newText("DS Crossings ");
      p.infoLabND.txtFld.setText("3");
      p.infoLabDS.txtFld.setText("3");

    ELSIF s.equals(MKSTR("angle")) THEN
      p.infoLabND.newText("Angle (degrees) ");
      p.infoLabDS.newText("Angle (degrees) ");
      p.infoLabND.txtFld.setText("67.5");
      p.infoLabDS.txtFld.setText("67.5");

      IF p.frontRear.yesBox.getState() THEN
        p.canvas.AssignImage(p.applet, p.canvas.imgFA, "spokeanglefront.gif");
      ELSE
        p.canvas.AssignImage(p.applet, p.canvas.imgRA, "spokeanglerear.gif");
      END;

    ELSIF s.equals(MKSTR("yes")) THEN
      p.offLabT.setEnabled(TRUE);
      p.offLabT.txtFld.setText("4");

    ELSIF s.equals(MKSTR("no")) THEN
      p.offLabT.setEnabled(FALSE);
      p.offLabT.txtFld.setText("0");

    ELSE RETURN;
    END;

    p.validate();
    p.canvas.repaint(); 
  END Notify;

(* ==================================================================== *)

  PROCEDURE (p : RadioPair)setEnabled*(v : BOOLEAN);
  BEGIN 
    p.noBox.setEnabled(v);
    p.yesBox.setEnabled(v);
  END setEnabled;

(* ==================================================================== *)

  PROCEDURE (p : LabelledText)setEnabled*(v : BOOLEAN);
  BEGIN 
    p.label.setEnabled(v);
    p.txtFld.setEnabled(v);
  END setEnabled;

(* ==================================================================== *)

  PROCEDURE (p : LabelledText)newText(IN s : ARRAY OF CHAR),NEW;
  BEGIN 
    p.label.setText(MKSTR(s));
  END newText;

(* ==================================================================== *)

  PROCEDURE (p : RadioPair)itemStateChanged*(e : java_awt_event.ItemEvent),NEW; 
    VAR str : java_lang.String;
  BEGIN
    str := e.getItem()(java_lang.String);
    IF str.equals(p.yesBox.getLabel()) THEN
       p.yesBox.setState(TRUE);
       p.noBox.setState(FALSE);
    ELSE (* must be no box *)
       p.yesBox.setState(FALSE);
       p.noBox.setState(TRUE);
    END;
    p.parent.Notify(str);
  END itemStateChanged;

(* ==================================================================== *)

  PROCEDURE (thisApp : Applet)init*();	(* overrides JApplet.init() *)
    VAR tmpCan : Canvas;
  BEGIN
    thisApp.setLayout(java_awt.BorderLayout.Init()(java_awt.LayoutManager));
    NEW(tmpCan);
    NEW(thisApp.controls);

    thisApp.controls.canvas := tmpCan;
    thisApp.controls.initialize(tmpCan);
    thisApp.controls.applet := thisApp;

    thisApp.controls.canvas.theFont := 
	java_awt.Font.Init("TimesRoman", java_awt.Font.PLAIN, (*20*) 16);
    tmpCan.AssignImage(thisApp, tmpCan.imgFC, "spokefront.gif");

    thisApp.add(tmpCan, "Center");
    thisApp.add(thisApp.controls, "South");
  END init;

  PROCEDURE (thisApp : Applet)start*();	(* overrides JApplet.start() *)
  BEGIN
    thisApp.controls.setEnabled(TRUE);
  END start;

  PROCEDURE (thisApp : Applet)stop*();	(* overrides JApplet.stop() *)
  BEGIN
    thisApp.controls.setEnabled(FALSE);
  END stop;

(* ==================================================================== *)
END SpokeLength3.
(* ==================================================================== *)
