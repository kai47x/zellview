
MODULE CmdDialogs;

	

	IMPORT
		Lang := java_lang,
		Swing := javax_swing,
		Event := java_awt_event,
		Awt := java_awt,
		CPmain,
		Console;

	CONST
		Rsrc = "images/";
(*		Rsrc = "C:/Users/Alfa/Zellview/Texel/Text/Mod/images/"; *)

	TYPE
		JDialog* = POINTER TO EXTENSIBLE RECORD(Swing.JDialog); END;
	
(*		Listener* = POINTER TO(Event.ActionListener); END; *)
	
		SuchenUndErsetzen* = POINTER TO RECORD(Swing.JDialog ) 
			owner: Awt.Component;
			textArea: Swing.JTextArea;
			lbl1, lbl2: Swing.JLabel;
			tfSuche, tfErsetze: Swing.JTextField;
			checkGrossKlein: Swing.JCheckBox;
			btnBeenden, btnWeitersuchen, btnErsetzen, btnAlleErsetzen: Swing.JButton;
		END;

	VAR dlg: SuchenUndErsetzen;

	PROCEDURE Init*( frame: Awt.Frame): SuchenUndErsetzen, CONSTRUCTOR; 

	PROCEDURE (dlg: SuchenUndErsetzen) SetTextArea* (tArea: Swing.JTextArea), NEW;
	BEGIN
		dlg.textArea := tArea;
	END SetTextArea;

	PROCEDURE (dlg: SuchenUndErsetzen)InitComponents, NEW;
		VAR comp: Awt.Component;
			icon: Swing.ImageIcon;
			iconPath: Lang.String;
			btn: Swing.JButton;
	BEGIN
		dlg.setTitle("Suchen und Ersetzen");
		iconPath := Rsrc + "Search.png";
		dlg.setIconImage(Awt.Toolkit.getDefaultToolkit().getImage(dlg.getClass().getResource(iconPath)));
		dlg.setSize(480, 155);

		dlg.setLayout(NIL);			(* Layout Manager ausschalten *)		
		dlg.setResizable(FALSE);			(* Kein Grössenänderung des Dialogs *)
		dlg.setLocation(Awt.Point.Init(150, 150));			(* Position auf Screen *)
		dlg.setDefaultCloseOperation(Swing.WindowConstants.DISPOSE_ON_CLOSE);

		(* Label "Suchen nach" *)
		dlg.lbl1 := Swing.JLabel.Init("Suchen nach:");
		dlg.lbl1.setBounds(5, 5, 90, 25);
		comp := dlg.add(dlg.lbl1);

		(* Textfeld "Suchen nach"  *)
		dlg.tfSuche := Swing.JTextField.Init();
		dlg.tfSuche.setBounds(110, 5, 240, 25);
		comp := dlg.add(dlg.tfSuche);

		(* Label "Ersetzen durch" *)
		dlg.lbl2 := Swing.JLabel.Init("Ersetzen durch:");
		dlg.lbl2.setBounds(5, 35, 90, 25);
		comp := dlg.add(dlg.lbl2);

		(* Textfeld "Ersetzen durch" *)
		dlg.tfErsetze := Swing.JTextField.Init();
		dlg.tfErsetze.setBounds(110, 35, 240, 25);
		comp := dlg.add(dlg.tfErsetze);

		(* Checkbox "Groß/Kleinschreibung nicht beachten" *)
		dlg.checkGrossKlein := Swing.JCheckBox.Init("Groß-/Kleinschreibung beachten");
		dlg.checkGrossKlein.setBounds(5, 90, 300, 25);
		comp := dlg.add(dlg.checkGrossKlein);

		(* Button "Weitersuchen" *)
		btn := Swing.JButton.Init("Weitersuchen");
		btn.setFont(btn.getFont().deriveFont(Awt.Font.PLAIN, 11));
		btn.setBounds(350, 5, 120, 25);
(*		btn.addActionListener(dlg);		 *)
		comp := dlg.add(btn);
		dlg.btnWeitersuchen := btn;

		(* Button "Ersetzen *)
		btn := Swing.JButton.Init("Ersetzen");
		btn.setFont(btn.getFont().deriveFont(Awt.Font.PLAIN, 11));
		btn.setBounds(350, 35, 120, 25);
(*		btn.addActionListener(dlg); *)
		comp := dlg.add(btn);
		dlg.btnErsetzen := btn;
		
		(* Button "Alle ersetzen" *)
		btn := Swing.JButton.Init("Alle Ersetzen");
		btn.setFont(btn.getFont().deriveFont(Awt.Font.PLAIN, 11));
		btn.setBounds(350, 65, 120, 25);
(*		btn.addActionListener(dlg); *)
		comp := dlg.add(btn);
		dlg.btnAlleErsetzen := btn;

		(* Button "Beenden" mit Icon *)
		icon := Swing.ImageIcon.Init(dlg.getClass().getResource(Rsrc + "Exit.png"));
		btn := Swing.JButton.Init("Beenden", icon);
		btn.setFont(btn.getFont().deriveFont(Awt.Font.PLAIN, 11));
		btn.setBounds(350, 95, 120, 25);
(*		btn.addActionListener(dlg); *)
		comp := dlg.add(btn);

	END InitComponents;


(*
	PROCEDURE (dlg: SuchenUndErsetzen)actionPerformed* (e: Event.ActionEvent), NEW;
	BEGIN

	END actionPerformed;
*)
(*
	PROCEDURE (dlg: SuchenUndErsetzen) Init*(): SuchenUndErsetzen;
	BEGIN
		dlg := Swing.JDialog.Init()(SuchenUndErsetzen); 				
		RETURN
	END Init;


PROCEDURE MkMyRect*(Formals) : MyRect,BASE(actuals);
(* Local-declarations *)
BEGIN
(* Constructor body code *)
RETURN SELF;
END MkMyRect;

	PROCEDURE MkJDialog(): JDialog, BASE();
	BEGIN	
		RETURN SELF;	
	END MkJDialog;

*)

	PROCEDURE MkSuchenUndErsetzen* ( f: Awt.Frame; i:INTEGER ): SuchenUndErsetzen, BASE( f );
	BEGIN
		RETURN SELF;
	END MkSuchenUndErsetzen;

	PROCEDURE NewDialog* (): SuchenUndErsetzen;
	VAR d: SuchenUndErsetzen;
		f: Awt.Frame;
	BEGIN
		f:= NIL;
		d := MkSuchenUndErsetzen(f, 0);
		d.InitComponents;
		RETURN d
	END NewDialog;

BEGIN
	Console.WriteString("Hier ist CmdDialogs.main"); Console.WriteLn;
	dlg := NewDialog();
	dlg.SetTextArea(Swing.JTextArea.Init());
	dlg.setVisible(TRUE)
END CmdDialogs.;
