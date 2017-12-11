package CP.zellview.org;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

public class FrameDialogModal extends JDialog implements ActionListener
{

	private JButton btnBeenden;
	
	private Component owner;
	
	
	public FrameDialogModal()
	{
		initializeComponents();
	}
	
	private void initializeComponents()
	{
		
		this.setTitle("Dialog");
		this.setSize(300, 160);
		
		// Kein Layout Manager
		this.setLayout(null);
		
		// keine Grössenänderung des Dialogs
		this.setResizable(false);
		
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		
		
		btnBeenden = new JButton("Beenden");
		btnBeenden.setBounds(90,  80, 120,  30);
		btnBeenden.addActionListener(this);
		this.add(btnBeenden);
		
		
		
	}
	
	private void initDialog()
	{
		
		// Den Dialog modal anzeigen
		this.setModal(true);
		
		this.setLocationRelativeTo(owner);
		
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
		if (e.getSource() == btnBeenden)
			this.dispose();
		
		
	}
	
	
	
}
