
// Client.java 
import java.io.DataInputStream; 
import java.io.DataOutputStream; 
import java.io.IOException; 
import java.net.Socket; 
import java.util.Scanner; 

public class Client {  
	private Socket sockfd;  
	private DataOutputStream enSortie;  
	private DataInputStream enEntree;
 
 
	public Client(String hote) 
	{ 
		int portno = 5001; 
		byte[] buffer; // on lit/écrit des octets dans la socket 
		try { 
			sockfd = new Socket(hote, portno); 
			enSortie= new DataOutputStream(sockfd.getOutputStream()); 
			
			Scanner clavier = new Scanner(System.in); 
			System.out.printf("Please enter the message: "); 
 
			String str = clavier.nextLine();   
			buffer=str.getBytes(); 
			enSortie.write(buffer,0,buffer.length); 
					
			enEntree=new DataInputStream(sockfd.getInputStream());
			buffer=new byte[256];
			
			
			//n est la taille du buffer une fois lu sur la socket:
						
			int n=enEntree.read(buffer,0,255);			
			
			String message=new String(buffer,0,n);
			
			System.out.println(message);
			
			} catch(IOException e){
				e.printStackTrace();
			}
			
	}
			
		public static void main(String[] args) {
			new Client(args[0]);
		}
		
}