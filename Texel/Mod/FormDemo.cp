(*
GpcpCompiler.SaveCopyAsText
GpcpCompiler.Compile
GpcpCompiler.Run
*)
MODULE FormDemo;

	

	IMPORT
		Swing := javax_swing,
		Awt := java_awt,
		Event := java_awt_event,
		Lang := java_lang,
		Console, 
		CPmain ;

	CONST
		PanelX = 20; PanelY = 20;
		PanelWidth = 140; PanelHeight = 160;
		PanelXOffset = 150;

	TYPE
		SwingConst = Swing.SwingConstants;
		System = Lang.System;

		BackgroundPanel* = POINTER TO RECORD(Swing.JPanel + Event.ItemListener)
			frame: Frame;
			(*			label: Swing.JLabel; *)
			btnGruen, btnBlau, btnRot, btnGelb, btnGrau: Swing.JRadioButton;
		END;

		ForegroundPanel* = POINTER TO RECORD(Swing.JPanel + Event.ItemListener);
			frame: Frame;
			label: Swing.JLabel;
			btnBlau, btnRot, btnGelb, btnRosa, btnWeiss: Swing.JRadioButton;
		END;

		FontPanel* = POINTER TO RECORD(Swing.JPanel + Event.KeyListener + Event.ItemListener + Event.FocusListener)
			frame: Frame;
			label: Swing.JLabel;
			chkFett, chkKursiv: Swing.JCheckBox;
			tfGroesse: Swing.JTextField;
		END;

		Frame* = POINTER TO RECORD(Swing.JFrame + Event.ActionListener)
			bgPanel: BackgroundPanel;
			fgPanel: ForegroundPanel;
			fontPanel: FontPanel;
			btnBeenden: Swing.JButton;
			font: Awt.Font;
		END;

	VAR comp: Awt.Component; b: BOOLEAN; 	(* Dummies für WinApi und Awt *)

		(* ****************   Prozeduren für Hintergrundfarben   *********************************  *)

	PROCEDURE (f: Frame) SetBackColor, NEW;
		VAR i: INTEGER;
			color: Awt.Color;
	BEGIN
		color := Awt.Color.LIGHT_GRAY;

		FOR i := 0 TO f.fgPanel.getComponentCount() - 1 DO
			f.fgPanel.getComponent(i).setEnabled(TRUE)
		END;

		IF f.bgPanel.btnGruen.isSelected() THEN
			color := Awt.Color.GREEN
		ELSIF f.bgPanel.btnBlau.isSelected() THEN
			color := Awt.Color.BLUE;
			f.fgPanel.btnBlau.setEnabled(FALSE)
		ELSIF f.bgPanel.btnRot.isSelected() THEN
			color := Awt.Color.RED;
			f.fgPanel.btnRot.setEnabled(FALSE)
		ELSIF f.bgPanel.btnGelb.isSelected() THEN
			color := Awt.Color.YELLOW;
			f.fgPanel.btnGelb.setEnabled(FALSE);
			f.fgPanel.btnWeiss.setEnabled(FALSE)
		ELSE
			f.fgPanel.btnRosa.setEnabled(FALSE)
		END;
		f.getContentPane().setBackground(color);
	END SetBackColor;

	PROCEDURE (panel: BackgroundPanel) itemStateChanged* (e: Event.ItemEvent), NEW;
	BEGIN
		IF e.getStateChange() = Event.ItemEvent.SELECTED THEN
			panel.frame.SetBackColor();
		END
	END itemStateChanged;

	PROCEDURE (panel: BackgroundPanel) Add (IN grp: Swing.ButtonGroup; VAR but: Swing.JRadioButton; str: ARRAY OF CHAR), NEW;
	BEGIN
		but := Swing.JRadioButton.Init(MKSTR(str));
		but.setOpaque(FALSE);
		but.addItemListener(panel);
		grp.add(but);
		comp := panel.add(but)
	END Add;

	PROCEDURE (f: Frame)NewBackgroundPanel (): BackgroundPanel, NEW;
		VAR layout: Awt.LayoutManager;
			panel: BackgroundPanel;
			btnGrp: Swing.ButtonGroup;
			lbl: Swing.JLabel;
	BEGIN
		NEW(panel);
		panel.frame := f;

		layout := Awt.GridLayout.Init(6, 1)(Awt.LayoutManager);
		panel.setLayout(layout);
		panel.setBounds(PanelX, PanelY, PanelWidth, PanelHeight);
		panel.setBorder(Swing.BorderFactory.createLineBorder(Awt.Color.BLACK));
		panel.setOpaque(FALSE);
		(* Label für Hintergrundfarbe *)
		lbl := Swing.JLabel.Init("Hintergrund");
		lbl.setHorizontalAlignment(SwingConst.CENTER);
		comp := panel.add(lbl);

		(* Buttongruppe Hintergrundfarbe *)
		btnGrp := Swing.ButtonGroup.Init();

		panel.Add(btnGrp, panel.btnGruen, "Grün");
		panel.Add(btnGrp, panel.btnBlau, "Blau");
		panel.Add(btnGrp, panel.btnRot, "Rot");
		panel.Add(btnGrp, panel.btnGelb, "Gelb");
		panel.Add(btnGrp, panel.btnGrau, "Grau");

		comp := f.add(panel);
		RETURN panel
	END NewBackgroundPanel;

	(* ****************   Prozeduren für Vordergrundfarben   *********************************  *)

	PROCEDURE (f: Frame) SetForeColor, NEW;
		VAR i: INTEGER;
			color: Awt.Color;
	BEGIN

		color := Awt.Color.LIGHT_GRAY;

		(* erstmal alle Hintergrundfarben aktivieren *)
		FOR i := 0 TO f.bgPanel.getComponentCount() - 1 DO
			f.bgPanel.getComponent(i).setEnabled(TRUE)
		END;

		IF f.fgPanel.btnBlau.isSelected() THEN
			color := Awt.Color.BLUE;
			f.bgPanel.btnBlau.setEnabled(FALSE)
		ELSIF f.fgPanel.btnRot.isSelected() THEN
			color := Awt.Color.RED;
			f.bgPanel.btnRot.setEnabled(FALSE)
		ELSIF f.fgPanel.btnGelb.isSelected() THEN
			color := Awt.Color.YELLOW;
		ELSIF f.fgPanel.btnRosa.isSelected() THEN
			color := Awt.Color.PINK;
			f.bgPanel.btnGrau.setEnabled(FALSE)
		ELSIF f.fgPanel.btnWeiss.isSelected() THEN
			color := Awt.Color.WHITE;
		END;

		IF (color = Awt.Color.YELLOW) OR (color = Awt.Color.WHITE) THEN
			f.bgPanel.btnGelb.setEnabled(FALSE)
		END;

		(* Hintergrundpanel *)
		f.bgPanel.setBorder(Swing.BorderFactory.createLineBorder(color));
		FOR i := 0 TO f.bgPanel.getComponentCount() - 1 DO
			f.bgPanel.getComponent(i).setForeground(color)
		END;

		(* Vordergrundpanel *)
		f.fgPanel.setBorder(Swing.BorderFactory.createLineBorder(color));
		FOR i := 0 TO f.fgPanel.getComponentCount() - 1 DO
			f.fgPanel.getComponent(i).setForeground(color)
		END;

		(* Fontpanel *)
		f.fontPanel.setBorder(Swing.BorderFactory.createLineBorder(color));
		FOR i := 0 TO f.fontPanel.getComponentCount() - 1 DO
			comp := f.fontPanel.getComponent(i);
			IF ~(comp IS Swing.JTextField) THEN
				comp.setForeground(color)
			END
		END
	END SetForeColor;

	PROCEDURE (panel: ForegroundPanel) itemStateChanged* (e: Event.ItemEvent), NEW;
	BEGIN
		IF e.getStateChange() = Event.ItemEvent.SELECTED THEN
			panel.frame.SetForeColor();
		END
	END itemStateChanged;

	PROCEDURE (panel: ForegroundPanel) Add (IN grp: Swing.ButtonGroup; VAR but: Swing.JRadioButton; str: ARRAY OF CHAR), NEW;
	BEGIN
		but := Swing.JRadioButton.Init(MKSTR(str));
		but.setOpaque(FALSE);
		but.addItemListener(panel);
		grp.add(but);
		comp := panel.add(but)
	END Add;

	PROCEDURE (f: Frame) NewForegroundPanel (): ForegroundPanel, NEW;
		VAR layout: Awt.LayoutManager;
			panel: ForegroundPanel;
			btnGrp: Swing.ButtonGroup;
			lbl: Swing.JLabel;
	BEGIN
		NEW(panel);
		panel.frame := f;

		layout := Awt.GridLayout.Init(6, 1)(Awt.LayoutManager);
		panel.setLayout(layout);
		panel.setBounds(PanelX + PanelXOffset, PanelY, PanelWidth, PanelHeight);
		panel.setBorder(Swing.BorderFactory.createLineBorder(Awt.Color.BLACK));
		panel.setOpaque(FALSE);
		(* Label für Hintergrundfarbe *)
		lbl := Swing.JLabel.Init("Vordergrund");
		lbl.setHorizontalAlignment(SwingConst.CENTER);
		comp := panel.add(lbl);
		panel.label := lbl;

		(* Buttongruppe Hintergrundfarbe *)
		btnGrp := Swing.ButtonGroup.Init();

		panel.Add(btnGrp, panel.btnBlau, "Blau");
		panel.Add(btnGrp, panel.btnRot, "Rot");
		panel.Add(btnGrp, panel.btnGelb, "Gelb");
		panel.Add(btnGrp, panel.btnRosa, "Rosa");
		panel.Add(btnGrp, panel.btnWeiss, "Weiss");

		comp := f.add(panel);
		RETURN panel
	END NewForegroundPanel;

	(* ****************   Prozeduren für Fontproperties   *********************************  *)

	PROCEDURE (f: Frame) SetFontSize (), NEW;
		VAR fontSize, newSize: INTEGER;
	BEGIN
	END SetFontSize;

	PROCEDURE (panel: FontPanel) keyPressed* (e: Event.KeyEvent), NEW;
	BEGIN
		(*
		IF e.getStateChange() = Event.ItemEvent.SELECTED THEN
		panel.frame.SetBackColor();
		END
		*)
	END keyPressed;

	PROCEDURE (panel: FontPanel) keyReleased* (e: Event.KeyEvent), NEW;
	BEGIN
	END keyReleased;

	PROCEDURE (panel: FontPanel) keyTyped* (e: Event.KeyEvent), NEW;
	BEGIN
	END keyTyped;

	PROCEDURE (panel: FontPanel) itemStateChanged* (e: Event.ItemEvent), NEW;
	BEGIN
	END itemStateChanged;

	PROCEDURE (panel: FontPanel) focusGained* (e: Event.FocusEvent), NEW;
	BEGIN
		(*
		IF e.getSource() IS Swing.JTextField THEN
		e.getSource()(Swing.JTextField).seletAll()
		END
		*)
	END focusGained;


	PROCEDURE (panel: FontPanel) focusLost* (e: Event.FocusEvent), NEW;
	BEGIN
		panel.frame.SetFontSize();
	END focusLost;

	PROCEDURE (f: Frame) NewFontPanel (): FontPanel, NEW;
		VAR layout: Awt.LayoutManager;
			panel: FontPanel;
			lbl: Swing.JLabel;
			check: Swing.JCheckBox;
			field: Swing. JTextField;
	BEGIN
		NEW(panel);
		panel.frame := f;

		(*		layout := Awt.GridLayout.Init(6, 1)(Awt.LayoutManager); *)
		panel.setLayout(NIL);
		panel.setBounds(PanelX + 2 * PanelXOffset, PanelY, PanelWidth, PanelHeight);
		panel.setBorder(Swing.BorderFactory.createLineBorder(Awt.Color.BLACK));
		panel.setOpaque(FALSE);

		(* Label für Fontpanel *)
		lbl := Swing.JLabel.Init("Schriftart");
		lbl.setHorizontalAlignment(SwingConst.CENTER);
		lbl.setBounds(0, 4, panel.getWidth(), 25);
		comp := panel.add(lbl);
		panel.label := lbl;

		(* ********************** Checkbox Fett *************************** *)
		check := Swing.JCheckBox.Init("Fett");
		check.setOpaque(FALSE);
		check.addItemListener(panel);
		check.setBounds(10, 30, 100, 25);
		comp := panel.add(check);

		(* ********************** Checkbox Kursiv *************************** *)
		check := Swing.JCheckBox.Init("Kursiv");
		check.setOpaque(FALSE);
		check.addItemListener(panel);
		check.setBounds(10, 55, 100, 25);
		comp := panel.add(check);

		(* ********************** Schriftgrösse  *************************** *)
		lbl := Swing.JLabel.Init("Grösse");
		lbl .setBounds(10, 80, 100, 25);
		comp := panel.add(lbl);

		field := Swing.JTextField.Init();
		field.setBounds(10, 110, 100, 25);
		field.setHorizontalAlignment(SwingConst.CENTER);
		field.addKeyListener(panel);
		field.addFocusListener(panel);
		comp := panel.add(field);
		panel.tfGroesse := field;

		comp := f.add(panel);
		RETURN panel
	END NewFontPanel;

	PROCEDURE (f: Frame) actionPerformed* (e: Event.ActionEvent), NEW;
	BEGIN
		System.exit(0)
	END actionPerformed;

	PROCEDURE NewFrame (): Frame;
		VAR f: Frame;
			jFrame: Swing.JFrame;
			btn: Swing.JButton;
	BEGIN
		NEW(f);
		f.setTitle("CheckBox und RadioButton");
		f.setSize(485, 280);
		f.setResizable(FALSE);
		f.setDefaultCloseOperation(Swing.JFrame.EXIT_ON_CLOSE);
		f.setLayout(NIL);

		f.bgPanel := f.NewBackgroundPanel();
		f.fgPanel := f.NewForegroundPanel();
		f.fontPanel := f.NewFontPanel();

		btn := Swing.JButton.Init("Beenden");
		btn.setBounds(170, 200, 140, 30);
		btn.setToolTipText("Programm beenden");
		btn.setMnemonic('e');
		btn.addActionListener(f);
		comp := f.add(btn);
		f.btnBeenden := btn;

		RETURN f
	END NewFrame;

	PROCEDURE Run;
		VAR frame: Frame;
	BEGIN
		frame := NewFrame();
		frame.setVisible(TRUE)
	END Run;

BEGIN
	Console.WriteString("Hallo CP World"); Console.WriteLn;
	Run;
END FormDemo.
