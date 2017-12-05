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
		CPmain, Console;

	TYPE
		SwingConst = Swing.SwingConstants;
		
		BackgroundPanel* = POINTER TO RECORD(Swing.JPanel + Event.ItemListener)
			frame: Frame;
			btnGruen, btnBlau, btnRot, btnGelb, btnGrau: Swing.JRadioButton;
		END;
		
		Frame* = POINTER TO RECORD(Swing.JFrame)
			bgPanel: BackgroundPanel;
		END;

	VAR
		frame: Frame;
		comp: Awt.Component;
		b: BOOLEAN;

	PROCEDURE (f: Frame) SetBackColor, NEW;
	VAR color: Awt.Color;
	BEGIN
		color := Awt.Color.LIGHT_GRAY;
		IF f.bgPanel.btnGruen.isSelected() THEN color := Awt.Color.GREEN
		ELSIF f.bgPanel.btnBlau.isSelected() THEN color := Awt.Color.BLUE
		ELSIF f.bgPanel.btnRot.isSelected() THEN color := Awt.Color.RED
		ELSIF f.bgPanel.btnGelb.isSelected() THEN color := Awt.Color.YELLOW
		ELSIF f.bgPanel.btnGrau.isSelected() THEN color := Awt.Color.GRAY
		END;
		f.getContentPane().setBackground(color);
	END SetBackColor;

	PROCEDURE (panel: BackgroundPanel) itemStateChanged* (e: Event.ItemEvent), NEW;
	BEGIN
		IF e.getStateChange() = Event.ItemEvent.SELECTED THEN
			panel.frame.SetBackColor();
			Awt.Toolkit.getDefaultToolkit().beep(); 
		END
	END itemStateChanged;

	PROCEDURE (panel: BackgroundPanel) Add(IN grp:Swing.ButtonGroup; VAR but: Swing.JRadioButton; str: ARRAY OF CHAR), NEW;
	BEGIN
		but := Swing.JRadioButton.Init(MKSTR(str));
		but.setOpaque(FALSE);
		but.addItemListener(panel);
		grp.add(but);
		comp := panel.add(but)
	END Add;

	(* Panel für Hintergrundfarben *)
	PROCEDURE NewBackgroundPanel(f: Frame): BackgroundPanel;
	VAR layout: Awt.LayoutManager;
		panel: BackgroundPanel;
		btnGrp: Swing.ButtonGroup;
		lbl: Swing.JLabel;
	BEGIN
		NEW(panel);
		panel.frame := frame;
		
		layout := Awt.GridLayout.Init(6, 1)(Awt.LayoutManager);
		panel.setLayout(layout);
		panel.setBounds(20, 20, 140, 160);
		panel.setBorder(Swing.BorderFactory.createLineBorder(Awt.Color.BLACK));
		panel.setOpaque(FALSE);
		(* Label Hintergrund *)
		lbl := Swing.JLabel.Init("Hintergrund");
		lbl.setHorizontalAlignment(SwingConst.CENTER);
		comp := panel.add(lbl);

		(* Buttongruppe Hintergrund *)
		btnGrp := Swing.ButtonGroup.Init();

		panel.Add(btnGrp, panel.btnGruen, "Grün");
		panel.Add(btnGrp, panel.btnBlau, "Blau");
		panel.Add(btnGrp, panel.btnRot, "Rot");
		panel.Add(btnGrp, panel.btnGelb, "Gelb");
		panel.Add(btnGrp, panel.btnGrau, "Grau");

		comp := f.add(panel); 
		RETURN panel
	END NewBackgroundPanel;

	PROCEDURE NewFrame(): Frame;
	VAR f: Frame;
		jFrame: Swing.JFrame;
	BEGIN
		Awt.Toolkit.getDefaultToolkit().beep(); 	
		NEW(f);
		f.setTitle("CheckBox und RadioButton");
		f.setSize(485, 280);
		f.setResizable(FALSE);
		f.setDefaultCloseOperation(Swing.JFrame.EXIT_ON_CLOSE);
		f.setLayout(NIL);
		
		f.bgPanel := NewBackgroundPanel(f);
		RETURN f
END NewFrame;

BEGIN
	Console.WriteString("Hallo CP World"); Console.WriteLn;

	frame := NewFrame();
	frame.setVisible(TRUE)
END FormDemo.
