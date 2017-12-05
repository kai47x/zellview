package w5t3_Dozent;

import java.awt.Component;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

public class FrameDialogSuchenUndErsetzen extends JDialog implements ActionListener
{
	private JLabel lbl1, lbl2;
	private JTextField tfSuche, tfErsetze;
	private JCheckBox checkGrossKlein;
	private JButton btnBeenden, btnWeitersuchen, btnErsetzen, btnAlleErsetzen;
	
	private Component owner;
	
	// Wir bekommen vom aufrufenden Fenster eine Referenz auf seine TextArea übergeben
	private JTextArea textArea;
	
	public FrameDialogSuchenUndErsetzen(JTextArea textArea)
	{
		initializeComponents();
		this.textArea = textArea;
	}
	
	private void initializeComponents()
	{
		
		ImageIcon icon;
		
		this.setTitle("Suchen und Ersetzen");
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("/images/Search.png")));
		this.setSize(480, 155);
		
		// Layout Manager ausschalten
		this.setLayout(null);
		
		// Kein Grössenänderung des Dialogs
		this.setResizable(false);
		
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		
		lbl1 = new JLabel("Suchen nach:");
		lbl1.setBounds(5, 5, 90, 25);
		this.add(lbl1);
		
		tfSuche = new JTextField();
		tfSuche.setBounds(110, 5, 240, 25);
		this.add(tfSuche);
		
		lbl2 = new JLabel("Ersetzen durch:");
		lbl2.setBounds(5, 35, 90, 25);
		this.add(lbl2);
		
		tfErsetze = new JTextField();
		tfErsetze.setBounds(110, 35, 240, 25);
		this.add(tfErsetze);
		
		checkGrossKlein = new JCheckBox("Groß-/Kleinschreibung beachten");
		checkGrossKlein.setBounds(5, 90, 300, 25);
		this.add(checkGrossKlein);
		
		btnWeitersuchen = new JButton("Weitersuchen");
		btnWeitersuchen.setFont(btnWeitersuchen.getFont().deriveFont(Font.PLAIN, 11f));
		btnWeitersuchen.setBounds(350, 5, 120, 25);
		btnWeitersuchen.addActionListener(this);
		this.add(btnWeitersuchen);
		
		btnErsetzen = new JButton("Ersetzen");
		btnErsetzen.setFont(btnErsetzen.getFont().deriveFont(Font.PLAIN, 11f));
		btnErsetzen.setBounds(350, 35, 120, 25);
		btnErsetzen.addActionListener(this);
		this.add(btnErsetzen);
		
		btnAlleErsetzen = new JButton("Alle Ersetzen");
		btnAlleErsetzen.setFont(btnAlleErsetzen.getFont().deriveFont(Font.PLAIN, 11f));
		btnAlleErsetzen.setBounds(350, 65, 120, 25);
		btnAlleErsetzen.addActionListener(this);
		this.add(btnAlleErsetzen);
		
		// Beenden Button mit Icon
		icon = new ImageIcon(this.getClass().getResource("/images/Exit.png"));
		btnBeenden = new JButton("Beenden", icon);
		btnBeenden.setFont(btnBeenden.getFont().deriveFont(Font.PLAIN, 11f));
		btnBeenden.setBounds(350, 95, 120, 25);
		btnBeenden.addActionListener(this);
		this.add(btnBeenden);
		
		
	}
	
	private void initDialog()
	{
		// Den Dialog modal anzeigen
		this.setModal(true);
				
		this.setLocationRelativeTo(owner);
		
		
		if (textArea.getSelectedText() != null)
		{
			// Den markierten Text aus der TextArea in das Suchfeld übernehmen
			// und dort ebenfalls markieren.
			tfSuche.setText(textArea.getSelectedText());
			
		}
		
		// Text im Suchfeld markieren
		tfSuche.selectAll();
		
		
		
		// Text im Suchfeld markieren
//		textArea.setSelectionStart(33);
//		textArea.setSelectionEnd(54);
		
		// oder
		textArea.select(32, 56);
		
		
		
		// Wichtig!
		// Durch den Fokusverlust ist der markierte Text im Hauptfenster
		// nicht mehr sichtbar.
		// Durch das erneute Setzen des Eingabefokus wird die Markierung wieder
		// angezeigt.
		textArea.requestFocusInWindow();
		
	}
	
	public void showDialog()
	{
		initDialog();
		this.setVisible(true);
		
	}

	
	public void showDialog(Component owner)
	{
		this.owner = owner;
		showDialog();
		
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		// TODO Auto-generated method stub
		
	}

}
