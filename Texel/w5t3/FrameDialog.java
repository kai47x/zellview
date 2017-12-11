package w5t3;

import java.awt.Font;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.Files;
import java.text.NumberFormat;
import java.util.Scanner;

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.filechooser.FileNameExtensionFilter;

public class FrameDialog extends JFrame implements WindowListener, ActionListener, ItemListener, DocumentListener
{

	private JTextArea textArea;
	private JScrollPane textAreaScrollPane;
	private JCheckBox checkLineWrap, checkWordWrap;
	private JButton btnBeenden, btnDialog, btnDatei, btnSuchenUndErsetzen;

	private File fcFile;

	private boolean hasChanged;
	
	private int hashCode;
	
	
	public FrameDialog() {
		initializeComponents();
	}

	private void initializeComponents()
	{

		ImageIcon icon;
		
		this.setTitle("Hauptfenster des Programms");
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("/w5t3/images/Editor.png")));
		
		
		this.setSize(760, 420);

		// Keine Grössenänderung des Frames
		this.setResizable(false);

		// Layout Manager ausschalten
		this.setLayout(null);

		
		// Der WindowListener überwacht das Schließen des Frames
		this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
		this.addWindowListener(this);

		checkLineWrap = new JCheckBox("Automatische Zeilenumbruch");
		checkLineWrap.setFont(checkLineWrap.getFont().deriveFont(Font.PLAIN, 9F));
		checkLineWrap.setBounds(590, 5, 200, 20);
		checkLineWrap.addItemListener(this);
		this.add(checkLineWrap);

		checkWordWrap = new JCheckBox("Auf Wortgrenze");
		checkWordWrap.setFont(checkWordWrap.getFont().deriveFont(Font.PLAIN, 9F));
		checkWordWrap.setBounds(590, 25, 200, 20);
		checkWordWrap.addItemListener(this);
		this.add(checkWordWrap);

		textArea = new JTextArea();

		// Freien Bereich zwischen Rahmen und Text definieren
		textArea.setMargin(new Insets(10, 10, 10, 10));

		
		textArea.getDocument().addDocumentListener(this);
		
		// Stellt horizontale und vertikale Laufleisten zum Blättern
		// zur Verfügung, wenn der Inhalt der Komponente größer
		// ist als die definierte Sicht (Grösse).
		textAreaScrollPane = new JScrollPane(textArea);

		textAreaScrollPane.setBounds(5, 5, 580, 360);

		this.add(textAreaScrollPane);

		// Datei Button mit Icon
		icon = new ImageIcon(this.getClass().getResource("/w5t3/images/Open.png"));
		//btnDatei = new JButton("Datei öffnen...", icon);
		
		// oder
		btnDatei = new JButton("Datei öffnen...");
		btnDatei.setIcon(icon);
		
		btnDatei.setFont(btnDatei.getFont().deriveFont(Font.PLAIN, 11f));
		btnDatei.setBounds(595, 230, 150, 30);
		btnDatei.addActionListener(this);
		this.add(btnDatei);

		btnDialog = new JButton("Dialog anzeigen");
		btnDialog.setFont(btnDialog.getFont().deriveFont(Font.PLAIN, 11f));
		btnDialog.setBounds(595, 265, 150, 30);
		btnDialog.addActionListener(this);
		this.add(btnDialog);

		// Suchen und Ersetzenen Button mit Icon
		icon = new ImageIcon(this.getClass().getResource("/w5t3/images/Search.png"));
		btnSuchenUndErsetzen = new JButton("Suchen / Ersetzen", icon);
		btnSuchenUndErsetzen.setFont(btnSuchenUndErsetzen.getFont().deriveFont(Font.PLAIN, 11f));
		btnSuchenUndErsetzen.setBounds(595, 300, 150, 30);
		btnSuchenUndErsetzen.addActionListener(this);
		this.add(btnSuchenUndErsetzen);
		
		// Beenden Button mit Icon
		icon = new ImageIcon(this.getClass().getResource("/w5t3/images/Exit.png"));
		btnBeenden = new JButton("Beenden", icon);
		btnBeenden.setFont(btnBeenden.getFont().deriveFont(Font.PLAIN, 11f));
		btnBeenden.setBounds(595, 335, 150, 30);
		btnBeenden.addActionListener(this);
		this.add(btnBeenden);

	}

	private void initFrame()
	{
		// In der Mitte des Desktops anzeigen
		this.setLocationRelativeTo(null);

		textArea.setText("Mit dem JTextArea-Steuerelement kann der Benutzer Text in einer Anwendung eingeben. "
				+ "Dieses Steuerelement bietet eine Funktionalität, die über das Standard-JTextField-Steuerelement von Java hinausgeht. "
				+ "Dazu gehören Mehrzeilenbearbeitung und Zeichenmaskierung für Kennwörter. "
				+ "Normalerweise wird ein JTextField-Steuerelement für die Anzeige oder Eingabe einer einzelnen Textzeile verwendet.");

		// Autmatischer Zeilenumbruch auf WOrtgrenze
		checkWordWrap.setSelected(true);
		
		
		hasChanged = false;
		
		hashCode = textArea.getText().hashCode();

	}

	public void showFrame()
	{

		initFrame();
		this.setVisible(true);
	}

	private void anzeigeDialog()
	{

		FrameDialogModal dlg = new FrameDialogModal();
		dlg.showDialog(this);

		// System.out.println("Nach Aufruf von showDialog()");

	}

	private void dateiLesen()
	{

		JFileChooser fc = new JFileChooser();

		fc.setCurrentDirectory(fcFile);

		fc.setFileFilter(new FileNameExtensionFilter("Textdokument (*.txt)", "txt"));
		fc.addChoosableFileFilter(new FileNameExtensionFilter("CMD-Datei (*.cmd)", "cmd"));
		fc.addChoosableFileFilter(new FileNameExtensionFilter("CSV-Datei (*.csv)", "csv"));
		fc.addChoosableFileFilter(new FileNameExtensionFilter("BAT-Datei (*.bat)", "bat"));

		// oder
		fc.addChoosableFileFilter(new FileNameExtensionFilter("Alle Textdateien", "txt", "cmd", "csv", "bat"));

		// Alle Dateien (*.*) als Dateifilter wird nicht angeboten. Standard =
		// true.
		fc.setAcceptAllFileFilterUsed(false);

		fc.setDialogTitle("Textdokument öffnen");

		if (fc.showOpenDialog(this) != JFileChooser.APPROVE_OPTION)
			return;

		fcFile = fc.getSelectedFile();

		// dateiLesen(fcFile.toString());

		//dateiLesenStringBuilder(fcFile.toString());

		//dateiLesenPuffer(fcFile.toString());
		
		//dateiLesenPuffer1(fcFile.toString());
		
		//dateiLesenFileChannel(fcFile.toString());
		
		dateiLesenReadAllBytes(fcFile.toString());
		
		//dateiLesenTextAreaRead(fcFile.toString());
		
		hasChanged = false;
		
		hashCode = textArea.getText().hashCode();
		
	}

	private void dateiLesen(String Dateiname)
	{

		long start = 0, ende = 0;

		// Inhalt der TextArea löschen
		textArea.setText(null);

		// try-catch-Anweisung mit Resourcen
		// Voraussetzung: Die Resource (Klasse zum Lesen/Schreiben der Datei) implementiert die Schnittstelle
		// AutoClosable

		try (Scanner in = new Scanner(new FileInputStream(Dateiname)))
		{
			start = System.currentTimeMillis();

			while (in.hasNextLine())
				textArea.append(in.nextLine() + "\n");

			ende = System.currentTimeMillis();

		}
		catch (Exception ex)
		{

			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden", "Lesen zeilenweise mit Scanner",
				JOptionPane.INFORMATION_MESSAGE);

	}

	private void dateiLesenStringBuilder(String Dateiname)
	{

		StringBuilder sb = new StringBuilder();
		long start = 0, ende = 0;

		// Inhalt der TextArea löschen
		textArea.setText(null);

		// try-catch-Anweisung mit Resourcen
		// Voraussetzung: Die Resource (Klasse zum Lesen/Schreiben der Datei) implementiert die Schnittstelle
		// AutoClosable

		try (Scanner in = new Scanner(new FileInputStream(Dateiname)))
		{
			start = System.currentTimeMillis();

			while (in.hasNextLine())
				sb.append(in.nextLine() + "\n");

			textArea.setText(sb.toString());

			ende = System.currentTimeMillis();

		}
		catch (Exception ex)
		{

			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen zeilenweise mit StringBuilder", JOptionPane.INFORMATION_MESSAGE);

	}

	private void dateiLesenPuffer(String Dateiname)
	{
		StringBuilder sb = new StringBuilder();
		long start = 0, ende = 0;
		int length = 0;
		
		char[] chars = new char[8192];
		
		// Inhalt der TextArea löschen
		textArea.setText(null);

		try (BufferedReader br = new BufferedReader(new FileReader(Dateiname)))
		{
			start = System.currentTimeMillis();
			
			while(true)
			{
				
				length = br.read(chars, 0, chars.length);
				sb.append(new String(chars, 0, length));
				
				if (length < chars.length)
					break;
				
			}
			
			textArea.setText(sb.toString());

			ende = System.currentTimeMillis();

		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen mit Puffer", JOptionPane.INFORMATION_MESSAGE);

	}

	private void dateiLesenPuffer1(String Dateiname)
	{
		StringBuilder sb = new StringBuilder();
		long start = 0, ende = 0;
		
		
		
		File file = new File(Dateiname);
		
		if (file.length() > Integer.MAX_VALUE)
		{
			
//			JOptionPane.showMessageDialog(this, "Die Datei ist zu groß, um mit dieser Methode eingelesen werden zu können", "Lesefehler", JOptionPane.ERROR_MESSAGE);
//			return;
			
			// oder
			dateiLesenPuffer(Dateiname);
			return;
			
		}
		
		
		// Inhalt der TextArea löschen
		textArea.setText(null);

		try (FileInputStream fr = new FileInputStream(Dateiname))
		{
			start = System.currentTimeMillis();
			
			byte[] bytes = new byte[(int)file.length()];
			fr.read(bytes);
			
			textArea.setText(new String(bytes));

			ende = System.currentTimeMillis();

		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen mit Puffer1", JOptionPane.INFORMATION_MESSAGE);

	}
	
	
	private void dateiLesenFileChannel(String Dateiname)
	{
		
		long start = 0, ende = 0;
		
		File file = new File(Dateiname);
		
		if (file.length() > Integer.MAX_VALUE)
		{
			
//			JOptionPane.showMessageDialog(this, "Die Datei ist zu groß, um mit dieser Methode eingelesen werden zu können", "Lesefehler", JOptionPane.ERROR_MESSAGE);
//			return;
			
			// oder
			dateiLesenPuffer(Dateiname);
			return;
			
		}
		
		// Inhalt der TextArea löschen
		textArea.setText(null);
		
		try(FileChannel fileChannel = FileChannel.open(file.toPath()))
		{
			
			start = System.currentTimeMillis();
			
			ByteBuffer buffer = ByteBuffer.allocate((int) file.length());
			fileChannel.read(buffer);
			
			if (buffer.hasArray())
				textArea.setText(new String(buffer.array()));
						
			ende = System.currentTimeMillis();
			
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen mit FileChannel", JOptionPane.INFORMATION_MESSAGE);
		

	}

	private void dateiLesenReadAllBytes(String Dateiname)
	{
		
		long start = 0, ende = 0;
		
		File file = new File(Dateiname);
		
		if (file.length() > Integer.MAX_VALUE)
		{
			
//			JOptionPane.showMessageDialog(this, "Die Datei ist zu groß, um mit dieser Methode eingelesen werden zu können", "Lesefehler", JOptionPane.ERROR_MESSAGE);
//			return;
			
			// oder
			dateiLesenPuffer(Dateiname);
			return;
			
		}
		
		
		// Inhalt der TextArea löschen
		textArea.setText(null);
				
		try
		{
			
			start = System.currentTimeMillis();
			
			byte[] byteArray = Files.readAllBytes(file.toPath());
			textArea.setText(new String(byteArray));
			
			ende = System.currentTimeMillis();
			
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen mit ReadAllBytes", JOptionPane.INFORMATION_MESSAGE);	
		
	}
	
	
	private void dateiLesenTextAreaRead(String Dateiname)
	{
		long start = 0, ende = 0;
		
		// Inhalt der TextArea löschen
		textArea.setText("");
		
		
		try(FileReader fr = new FileReader(Dateiname))
		{
			
			start = System.currentTimeMillis();
			
			textArea.read(fr, null);
			
			ende = System.currentTimeMillis();
			
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, "Fehler beim Einlesen der Datei: " + ex.getMessage(), "Lesefehler", JOptionPane.ERROR_MESSAGE);
		}

		JOptionPane.showMessageDialog(this, "Dauer: " + NumberFormat.getInstance().format(ende - start) + " Millisekunden",
				"Lesen mit TextArea.read()", JOptionPane.INFORMATION_MESSAGE);	
		
		
		
	}
	
	// Gibt 'true' zurück, wenn das Programm beendet werden soll.
	private boolean queryExit()
	{
		
		boolean retValue = true;
		
//		String[] options = {"Ja", "Nein"};
//		
//		
//		int optionValue = JOptionPane.showOptionDialog(this, "Soll das Programm wirklich beendet werden", "Programm beenden", JOptionPane.YES_NO_OPTION, 
//				                     JOptionPane.QUESTION_MESSAGE, null, options, options[1]);
//		
//		if (optionValue == JOptionPane.YES_OPTION)
//			retValue = true;
		
		
//		if (hasChanged)
//			retValue = saveFile();
		
		
		if (textArea.getText().hashCode() != hashCode)
			retValue = saveFile();
			
		
		return retValue;
		
	}
	
	private boolean saveFile()
	{
	
		boolean retValue = false;
		
		// Benutzerdefiniert Button-Texte
		String[] options = {"Ja", "Nein", "Abbrechen"};
		
		
		int optionValue = JOptionPane.showOptionDialog(this, "Daten wurden geändert.\nSollen die Daten vorher gespeichert werden", "Achtung", 
									JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[2]);
		
		if (optionValue == JOptionPane.NO_OPTION)
		{
			// Nein - nicht speichern, aber Beenden des Programms
			System.out.println("Datei wird nicht gespeichert");
			retValue = true;
		}
		else if (optionValue == JOptionPane.YES_OPTION)
		{
			// Ja - speichern und falls erfolgreich Beenden des Programms.
			System.out.println("Datei wird gespeichert");
			retValue = true;
			
		}
			
		return retValue;
	}
	
	private void anzeigeSuchenUndErsetzen()
	{
		FrameDialogSuchenUndErsetzen dlg = new FrameDialogSuchenUndErsetzen(textArea);
		dlg.showDialog(this);
		
	}
	
	public static void main(String[] args)
	{
		FrameDialog f = new FrameDialog();
		f.showFrame();

	}

	@Override
	public void actionPerformed(ActionEvent e)
	{

		if (e.getSource() == btnBeenden)
			windowClosing(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
		else if (e.getSource() == btnDialog)
			anzeigeDialog();
		else if (e.getSource() == btnDatei)
			dateiLesen();
		else if (e.getSource() == btnSuchenUndErsetzen)
			anzeigeSuchenUndErsetzen();

	}

	@Override
	public void itemStateChanged(ItemEvent e)
	{

		if (e.getSource() == checkLineWrap)
		{

			textArea.setLineWrap(checkLineWrap.isSelected());

			// Wenn autom. Zeilenumbruch nicht gesetzt ist,
			// evtl. gesetzten Wert für Umbruch auf Wortgrenze ebenfalls
			// ausschalten.
			if (!checkLineWrap.isSelected())
				checkWordWrap.setSelected(false);

		}
		else if (e.getSource() == checkWordWrap)
		{

			textArea.setWrapStyleWord(checkWordWrap.isSelected());

			// Wenn autom. Zeilenumbruch nicht gesetzt ist und Umbruch auf
			// Wortgrenze eingeschaltet wurde , den autom. Zeilenumbruch ebenfalls setzen.
			if (checkWordWrap.isSelected())
				checkLineWrap.setSelected(true);

		}

	}

	// Nur wenn Attribute der Komponente verändert wurden, nicht der Inhalt.
	
	@Override
	public void changedUpdate(DocumentEvent e)
	{
	
		//System.out.println("DocumentListener - changedUpdate()");
		
	}

	@Override
	public void insertUpdate(DocumentEvent e)
	{
		//System.out.println("DocumentListener - insertUpdate()");
		hasChanged = true;
		
	}

	@Override
	public void removeUpdate(DocumentEvent e)
	{
		//System.out.println("DocumentListener - removeUpdate()");
		hasChanged = true;
		
		
	}

	@Override
	public void windowActivated(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosed(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosing(WindowEvent e)
	{
		
		// dispose() ruft die Methode windowClosed() auf
		
		if (queryExit())
			this.dispose();
		
	}

	@Override
	public void windowDeactivated(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowDeiconified(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowIconified(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowOpened(WindowEvent arg0)
	{
		// TODO Auto-generated method stub
		
	}

}
